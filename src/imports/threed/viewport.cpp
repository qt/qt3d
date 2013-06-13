/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "viewport.h"
#include "qquickeffect.h"

#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglcamera.h"
#include "qglview.h"
#include "qglsubsurface.h"
#include "qray3d.h"
#include "qglframebufferobjectsurface.h"
#include "skybox.h"

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>

#include <QPainter>
#include <QEvent>
#include <QTimer>
#include <QCoreApplication>
#include <QQmlInfo>
#include <QtQuick/QQuickWindow>
#include <QOpenGLBuffer>
#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qmath.h>

/*!
    \qmltype Viewport
    \instantiates Viewport
    \brief The Viewport item defines the logical viewport for a 3D scene.  It includes all necessary
    references and parameters for the contents of the scene, as well as drawing and painting functions
    \since 4.8
    \ingroup qt3d::qml3d

    The Viewport item is usually the outermost in a 3D scene, specifying
    the size of the view, the camera position, lights, and the main 3D object:

    \code
    import QtQuick 2.0
    import Qt3D 2.0

    Viewport {
        width: 640; height: 480
        camera: Camera {}
        light: Light {}
        Item3D {
            mesh: Mesh { source: "meshes/teapot.bez" }
            effect: Effect {}
        }
    }
    \endcode

    \sa Camera
*/

QT_BEGIN_NAMESPACE

// copied from the top of qsgcanvas.cpp and qqmlglobal_p.h
static bool qmlNoThreadedRenderer()
{
    static enum { Yes, No, Unknown } status = Unknown;
#ifndef QT_NO_THREAD
    if (status == Unknown)
    {
        QByteArray v = qgetenv("QML_NO_THREADED_RENDERER");
        bool value = !v.isEmpty() && v != "0" && v != "false";
        status = (value) ? Yes : No;
    }
#endif
    return status == Yes;
}

class PickEvent
{
public:
    PickEvent() : m_object(0), m_event(0), m_callback(-1), m_id(nextId++) {}
    ~PickEvent()
    {
        delete m_event;
        // we don't own the object so don't delete that
    }
    QObject *object() { return m_object; }
    void setObject(QObject *o) { m_object = o; }
    QMouseEvent *event() { return m_event; }
    void setEvent(QMouseEvent *e) { m_event = e; }
    int callback() const { return m_callback; }
    void setCallback(int callback) { m_callback = callback; }
    quint64 id() const { return m_id; }
private:
    QObject *m_object;
    QMouseEvent *m_event;
    int m_callback;
    quint64 m_id;

    // INVARIANT CONDITION: We only even construct new PickEvent objects from the
    // gui thread (via calls to initiatePick in the mouse handlers) so thus the
    // accesses to this NON-THREAD-SAFE static are always serialized.
    static quint64 nextId;
};

quint64 PickEvent::nextId = 0;

/*
    \internal
    Like QMutexLocker class, except only do anything if qmlThreadedRenderer
    is true.

    Also hide nasty QT_NO_THREAD macros in here.  If built with
    QT_NO_THREAD calls should compile away to nothing.

    This class is not threadsafe.  Only ever use it by creating one on the local
    stack and not passing references to it out of local scope.
*/
class QMutexMaybeLocker
{
public:
#ifndef QT_NO_THREAD
    typedef QMutex Lock;
#else
    typedef int Lock;
#endif

    // Construct the locker.  Also do an RAAI acquire the mutex - but only if the
    // qmlNoThreadedRenderer function returns true.
    QMutexMaybeLocker(Lock *mutex)
        : m_mutex(mutex)
        , m_isLocked(false)
    {
#ifndef QT_NO_THREAD
        if (!qmlNoThreadedRenderer())
        {
            m_mutex->lock();
            m_isLocked = true;
        }
#else
        Q_UNUSED(mutex);
#endif
    }

    // This is here for when an exception or other unusual condition causes
    // control to jump out of the current block.  Also explicitly use the unlock
    // function for the reasons stated.
    ~QMutexMaybeLocker()
    {
#ifndef QT_NO_THREAD
        unlock();
#endif
    }

    // Have a seperate function like QMutexLocker for this.  Use it explicity for two
    // reasons (rather than just nicely exiting the scope and letting the destructor
    // do it) - reason one: documentation -> its clearer what is going on; reason two:
    // syntax checkers and static analysis tools will complain about unused variables
    // if you don't reference the constructed value.
    void unlock()
    {
#ifndef QT_NO_THREAD
        if (!qmlNoThreadedRenderer() && m_isLocked)
        {
            m_mutex->unlock();
            m_isLocked = false;
        }
#endif
    }
private:
    Lock *m_mutex;
    bool m_isLocked;
};

class ViewportPrivate
{
public:
    ViewportPrivate();
    ~ViewportPrivate();

    QColor fillColor;
    bool picking;
    bool showPicking;
    bool showSceneGraph;
    int dumpCount;
    bool navigation;
    bool fovzoom;
    bool blending;
    bool itemsInitialized;
    QGLCamera *camera;
    QGLLightParameters *light;
    QGLLightModel *lightModel;
    QWidget *viewWidget;
    int pickId;
    QOpenGLFramebufferObject *pickFbo;
    QMap<int, QObject *> objects;
    QObject *pressedObject;
    Qt::MouseButton pressedButton;
    QObject *enteredObject;
    bool panning;
    QPointF startPan;
    QPointF lastPan;
    QObject *lastObject;
    QVector3D startEye;
    QVector3D startCenter;
    QVector3D startUpVector;
    Qt::KeyboardModifiers panModifiers;
    QMap<int, QObject*> earlyDrawList;
    Viewport::RenderMode renderMode;
    bool directRenderInitialized;
    bool pickingRenderInitialized;
    QList<PickEvent *> pickEventQueue;

    // This lock is for the pick event queue itself.  All accesses
    // to that data structure must be guarded by this lock.
    QMutexMaybeLocker::Lock pickEventQueueLock;

    // This lock is for all of the class - generally concurrent accesses to any of the
    // class instance data from other threads should be at a bare minimum.
    QMutexMaybeLocker::Lock viewportLock;

    QQuickWindow* canvas;

    void setDefaults(QGLPainter *painter);
    void setRenderSettings(QGLPainter *painter);
    void getOverflow(QMouseEvent *e);
};

ViewportPrivate::ViewportPrivate()
    : picking(false)
    , showPicking(false)
    , showSceneGraph(false)
    , dumpCount(10)  // maybe this needs to be higher?
    , navigation(true)
    , fovzoom(true)
    , blending(false)
    , itemsInitialized(false)
    , camera(0)
    , light(0)
    , lightModel(0)
    , viewWidget(0)
    , pickId(1)
    , pickFbo(0)
    , pressedObject(0)
    , pressedButton(Qt::NoButton)
    , enteredObject(0)
    , panning(false)
    , startPan(-1, -1)
    , lastPan(-1, -1)
    , lastObject(0)
    , panModifiers(Qt::NoModifier)
    , renderMode(Viewport::UnknownRender)
    , directRenderInitialized(false)
    , pickingRenderInitialized(false)
#ifdef QT_NO_THREAD
    , pickEventQueueLock(0)
#endif
    , canvas(0)
{
}

ViewportPrivate::~ViewportPrivate()
{
    delete pickFbo;
    qDeleteAll(pickEventQueue);
}

void ViewportPrivate::setDefaults(QGLPainter *painter)
{
    painter->disableEffect();
    // Try to restore the default options
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    // Set the default depth buffer options.
#if defined(QT_OPENGL_ES)
    glClearDepthf(0);
#else
    glClearDepth(0);
#endif
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
    glDepthRangef(0.0f, 1.0f);
#else
    glDepthRange(0.0f, 1.0f);
#endif
    // Set the default blend options.
    glDisable(GL_BLEND);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter->glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter->glBlendEquation(GL_FUNC_ADD);
    else if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void ViewportPrivate::setRenderSettings(QGLPainter *painter)
{
    // Declarative SG sets clearDepth, and other depth buffer options to
    // unexpected values.  Set them up to standard Qt3Dvalues.

    // This seems to be only needed in beforeRendering()
#if defined(QT_OPENGL_ES)
    glClearDepthf(1);
#else
    glClearDepth(1);
#endif
    glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
    glDepthRangef(0.0f, 1.0f);
#else
    glDepthRange(0.0f, 1.0f);
#endif
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    QColor clearColor(Qt::black);
    if (fillColor.isValid())
        clearColor = fillColor;
    painter->setClearColor(clearColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

const int Viewport::FBO_SIZE = 8;

/*!
    \internal
    Construct the class and assign it a \a parent QQuickItem.
*/
Viewport::Viewport(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , d(new ViewportPrivate())
{
    // Has to be set or we crash on render.  Intention of this is that
    // the update() function gets called when a re-render is needed.
    setFlags(QQuickItem::ItemHasContents);

    connect(this, SIGNAL(widthChanged()), this, SIGNAL(viewportChanged()));
    connect(this, SIGNAL(heightChanged()), this, SIGNAL(viewportChanged()));
    connect(this, SIGNAL(viewportChanged()), this, SLOT(update3d()));

    setCamera(new QGLCamera(this));
    setLight(new QGLLightParameters(this));

    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}

/*!
    \internal
    Class destruction and cleanup.
*/
Viewport::~Viewport()
{
    delete d;
}

/*!
    \qmlproperty enumeration Viewport::renderMode

    This property defines the mode of rendering of this viewport as to
    whether it is direct to the GL context, or via a buffer (in this case
    a framebuffer object).

    If the scene occupies most of the display, but the Viewport is
    not the top level item, this property can be set to DirectRender in
    order to improve performance.

    At construction the render mode is set to UnknownRender, but during
    initialization it is updated to an appropriate default.  It is an error
    to set the renderMode() to UnknownRender, and in debug mode an
    assert will be thrown in this case.  In release mode behavior is
    undefined.

    By default if the viewport is a child object of another SGItem, then
    the renderMode() is set to use a buffer.  This is suitable for most
    simple 3D items which are small in size relative to the overall QML content
    displayed.  The buffer is prepared by drawing the 3D items into it and
    then in a second step the buffer is composited into the QML 2D scene.

    Otherwise if the viewport is a top-level item, the renderMode() is set
    by default to use direct rendering, in this case using a "GL Under"
    approach to capture the GL context prior to other QML content being
    rendered.  The 3D items are then rendered directly into the context
    with any 2D QML content being rendered over the top.  This is suitable
    where the 3D components of the scene occupies most or all of the screen.

    \list
        \li UnknownRender  The mode is not specified.
        \li DirectRender  Render to the GL context directly.  This is the default for top-level viewports.
        \li BufferedRender  Render to an offscreen buffer.  This is the default for a viewport that is a child.
    \endlist
    */
Viewport::RenderMode Viewport::renderMode() const
{
    return d->renderMode;
}

void Viewport::setRenderMode(Viewport::RenderMode mode)
{
    Q_ASSERT(mode != UnknownRender);
    if (d->renderMode != mode)
    {
        d->renderMode = mode;
        if (d->renderMode == BufferedRender)
        {
            setRenderTarget(QQuickPaintedItem::InvertedYFramebufferObject);
            if (d->canvas)
            {
                disconnect(d->canvas, SIGNAL(beforeRendering()),
                           this, SLOT(beforeRendering()));
                d->canvas->setClearBeforeRendering(true);
                d->directRenderInitialized = false;
            }
        }
        else
        {
            // If there is no engine at this point the setup will
            // be done in the sceneGraphInitialized handler
            if (d->canvas)
            {
                connect(d->canvas, SIGNAL(beforeRendering()),
                        this, SLOT(beforeRendering()), Qt::DirectConnection);
                d->canvas->setClearBeforeRendering(false);
                d->directRenderInitialized = true;
            }
        }
        emit viewportChanged();
    }
}

/*!
    \qmlproperty bool Viewport::fillColor()

    The color to use for the background of the viewport, if any.  When no color is set
    then the underlying QML background will show through (the viewport will be trans-
    parent).

    If a color is set here, then the viewport will be filled with that color before
    any content is rendered into it.

    The default value for this property is no color (an invalid color) resulting in
    a transparent viewport.

    \sa showPicking
*/
QColor Viewport::fillColor() const
{
    return d->fillColor;
}

void Viewport::setFillColor(const QColor &color)
{
    if (d->fillColor != color)
    {
        d->fillColor = color;
        emit viewportChanged();
    }
}

/*!
    \qmlproperty bool Viewport::picking

    User interaction in QML/3d is handled through the concept of object picking.  Each
    item has a unique picking id which is queried for a given screen click position when the
    mouse is clicked.

    If this property is set to true, picking will be supported for this
    viewport, while if the property is false, no picking will be applied.

    The default value for this property is false.

    \sa showPicking
*/
bool Viewport::picking() const
{
    return d->picking;
}

void Viewport::setPicking(bool value)
{
    if (value != d->picking)
    {
        d->picking = value;
        if (d->picking && d->canvas)
        {
            connect(d->canvas, SIGNAL(beforeRendering()),
                    this, SLOT(objectForPoint()), Qt::DirectConnection);
            d->pickingRenderInitialized = true;
        }
        else
        {
            if (d->canvas)
                disconnect(d->canvas, SIGNAL(beforeRendering()),
                           this, SLOT(objectForPoint()));
            d->pickingRenderInitialized = false;
        }
        emit viewportChanged();
    }
}

/*!
    \qmlproperty bool Viewport::showPicking
    The underlying mechanism for picking is based on painting an off-screen buffer with a flat
    coloured image containing all of the objects with a unique color value.

    Setting this property to true will display this flat-colour picking
    representation in the viewport, which can be useful for debugging
    problems with object selection.

    The default value for this property is false.

    \sa picking
*/

bool Viewport::showPicking() const
{
    return d->showPicking;
}

void Viewport::setShowPicking(bool value)
{
    d->showPicking = value;
    emit viewportChanged();
}

/*!
    \qmlproperty bool Viewport::showSceneGraph

    This property controls whether or not the 3D scenegraph structure is dumped
    to the console when the viewport is first rendered.  Studying the output can be very
    useful for optimising the scene, and for detecting issues with rendering, such
    as misplaced textures, materials, geometry and so on.

    By default the value is set to false.
*/
bool Viewport::showSceneGraph() const
{
    return d->showSceneGraph;
}

void Viewport::setShowSceneGraph(bool show)
{
    if (show != d->showSceneGraph)
    {
        d->showSceneGraph = show;
        emit showSceneGraphChanged();
    }
}

/*!
    \qmlproperty bool Viewport::navigation

    This property is used to set or unset camera navigation in for the viewport.
    Camera navigation allows the user to move the camera position around using the mouse.

    By default, camera navigation is set to true.
*/
bool Viewport::navigation() const
{
    return d->navigation;
}

void Viewport::setNavigation(bool value)
{
    d->navigation = value;
    emit viewportChanged();
}

/*!
    \qmlproperty bool Viewport::fovzoom

    This property is used to set or unset zooming based on field-of-view (fov).  Normally
    zooming is achieved by moving the camera physically closer to the target object.  This
    options achieves zooming by narrowing or broadening the fov.

    By default, fov zooming is set to false.
*/
bool Viewport::fovzoom() const
{
    return d->fovzoom;
}

void Viewport::setFovzoom(bool value)
{
    d->fovzoom = value;
    emit viewportChanged();
}


/*!
    \qmlproperty bool Viewport::blending

    The blending property is used to enable or disable GL_BLEND
    on the viewport, for alpha blending of drawn objects.

    By default, blending is set to false.

    \sa Effect::blending
*/
bool Viewport::blending() const
{
    return d->blending;
}

void Viewport::setBlending(bool value)
{
    d->blending = value;
    emit viewportChanged();
}

/*!
    \qmlproperty bool Viewport::antialiasing

    The antialiasing property is used to enable or disable antialiasing
    on the viewport. This property only has an effect if renderMode() == BufferedRender.
    For renderMode() == DirectRender antialiasing can be enabled by setting the
    QSurfaceFormat.

    By default, antialiasing is set to false.
*/
bool Viewport::antialiasing() const
{
    return QQuickPaintedItem::antialiasing();
}

void Viewport::setAntialiasing(bool value)
{
    if (value != QQuickPaintedItem::antialiasing()) {
        QQuickPaintedItem::setAntialiasing(value);
        Q_EMIT antialiasingChanged();
    }
}


/*!
    \qmlproperty Camera Viewport::camera

    This property sets the camera parameters which will be used for
    the appropriate viewing transforms in OpenGL.  The default is
    a perspective camera with its eye located at (0, 0, 10) looking
    at the center (0, 0, 0), with the y axis as up.
*/
QGLCamera *Viewport::camera() const
{
    return d->camera;
}

void Viewport::setCamera(QGLCamera *value)
{
    if (d->camera != value) {
        if (d->camera) {
            disconnect(d->camera, SIGNAL(projectionChanged()),
                       this, SLOT(cameraChanged()));
            disconnect(d->camera, SIGNAL(viewChanged()),
                       this, SLOT(cameraChanged()));
        }
        d->camera = value;
        if (d->camera) {
            connect(d->camera, SIGNAL(projectionChanged()),
                    this, SLOT(cameraChanged()));
            connect(d->camera, SIGNAL(viewChanged()),
                    this, SLOT(cameraChanged()));
        }
        cameraChanged();
    }
}

/*!
    \qmlproperty Light Viewport::light

    This property defines the main scene light to use for 3D items
    that are drawn in this viewport.

    \sa lightModel
*/

QGLLightParameters *Viewport::light() const
{
    return d->light;
}

void Viewport::setLight(QGLLightParameters *value)
{
    if (d->light != value) {
        if (d->light) {
            disconnect(d->light, SIGNAL(lightChanged()),
                       this, SLOT(update3d()));
        }
        d->light = value;
        if (d->light) {
            connect(d->light, SIGNAL(lightChanged()),
                    this, SLOT(update3d()));
        }
        emit viewportChanged();
    }
}

/*!
    \qmlproperty LightModel Viewport::lightModel
    The user is able to set a lighting model for the 3d environment through the use of the
    lightModel property.  By default the light model is undefined.

    \sa light
*/
QGLLightModel *Viewport::lightModel() const
{
    return d->lightModel;
}

void Viewport::setLightModel(QGLLightModel *value)
{
    if (d->lightModel != value) {
        if (d->lightModel) {
            disconnect(d->lightModel, SIGNAL(lightModelChanged()),
                       this, SLOT(update3d()));
        }
        d->lightModel = value;
        if (d->lightModel) {
            connect(d->lightModel, SIGNAL(lightModelChanged()),
                    this, SLOT(update3d()));
        }
        emit viewportChanged();
    }
}

class ViewportSubsurface : public QGLSubsurface
{
public:
    ViewportSubsurface(QGLAbstractSurface *surface, const QRect &region, float adjust)
        : QGLSubsurface(surface, region)
        , m_adjust(adjust)
    {
    }
    ~ViewportSubsurface()
    {
    }

    float aspectRatio() const;

private:
    float m_adjust;
};

float ViewportSubsurface::aspectRatio() const
{
    return QGLSubsurface::aspectRatio() * m_adjust;
}

/*!
    \reimp
    \internal

    Only used in the case of renderMode() == BufferedRender.

    Called by QQuickPaintedItem to refresh the content.

    \sa beforeRendering(), setRenderMode()
*/
void Viewport::paint(QPainter *painter)
{
    Q_ASSERT(renderMode() == BufferedRender);

    if (!isVisible())
        return;

    QGLPainter glPainter;
    if (!glPainter.begin(painter))
    {
        qWarning("GL graphics system is not active; cannot use 3D items");
        return;
    }
    if (d->fillColor.isValid())
    {
        glPainter.setClearColor(d->fillColor);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    else
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    glEnable(GL_DEPTH_TEST);

    render(&glPainter);

    d->setDefaults(&glPainter);
}

/*!
    \internal

    Only used in the case of renderMode() == DirectRender.

    Called by beforeRendering signal from QQuickContext.

    \sa paint()
*/
void Viewport::beforeRendering()
{
    Q_ASSERT(renderMode() == DirectRender);

    // Note: this slot will be executed in the QSG rendering thread
    // (Qt::DirectConnection) - not in the GUI/main thread of the app.
    // Beware of thread-safety issues.

    if (!isVisible())
        return;

    Q_ASSERT(d->canvas);
    Q_ASSERT(QOpenGLContext::currentContext() != 0);
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    QSurfaceFormat format = ctx->format();
    if (format.depthBufferSize() != 24)
    {
        // in the itemChange() handler below we catch the canvas just after it
        // has been created and ensure it has a 24bit depth buffer.  If somehow
        // this has not been passed to the context we have a problem.  There
        // are likely to be odd rendering artifacts.
        qWarning() << "Problem detected with GL format!!";
    }

    QGLPainter painter;
    if (!painter.begin())
    {
        qmlInfo(this) << tr("Unable to paint 3D items!");
        return;
    }

    d->setRenderSettings(&painter);
    render(&painter);
}

void Viewport::render(QGLPainter *painter)
{
    // Initialize the objects in the scene if this is the first paint.
    if (!d->itemsInitialized)
        initializeGL(painter);

    QGLTexture2D::processPendingResourceDeallocations();

    // No stereo rendering, set the eye as neutral
    painter->setEye(QGL::NoEye);

    // TODO
    // Deal with transforms on the object, which change the viewport?

    // boundingRect is in local coordinates. We need to map it to the scene coordinates
    // in order to render to correct area.
    QRect viewport = mapRectToScene(boundingRect()).toRect();
    // In BufferedRender mode we don't need to shift left upper corner of our rect,
    // because we render to separate render target.
    QRect target_rect( (renderMode() == DirectRender)? viewport.x():0,
                       (renderMode() == DirectRender)? viewport.y():0,
                       viewport.width(),
                       viewport.height() );
    QGLSubsurface surface (painter->currentSurface(), target_rect);
    painter->pushSurface(&surface);

    // Perform early drawing operations.
    earlyDraw(painter);

    // Set up the camera the way QGLView would if we were using it.
    if (d->camera) {
        painter->setCamera(d->camera);
    } else {
        QGLCamera defCamera;
        painter->setCamera(&defCamera);
    }

    // Draw the Item3D children.
    painter->setPicking(d->showPicking);

    // May've been set by early draw
    glDisable(GL_CULL_FACE);

    draw(painter);

    // May've been set by one of the items
    glDisable(GL_CULL_FACE);

    painter->setPicking(false);

    painter->popSurface();

    // Disable the effect to return control to the GL paint engine.
    painter->disableEffect();
}

/*!
  \internal
*/
void Viewport::earlyDraw(QGLPainter *painter)
{
    painter->disableEffect();
#ifdef GL_RESCALE_NORMAL
    // Scale normals by a scale factor derived from modelview matrix.
    // Note: normals need to be unit length.
    glEnable(GL_RESCALE_NORMAL);
#endif
}

/*!
  \internal
  The draw function for the viewport sets up all of the lighting parameters for the scene before
  iterating through the top level items in the scene and drawing them using \a painter.

  As a matter of course each item will draw its child items, and so on, until the entire image has
  been populated.
*/
void Viewport::draw(QGLPainter *painter)
{
    // At present only skyboxes work, with early draw and they are drawn
    // without blending and without a lighting model - just flat texture rendering
    int cnt = d->earlyDrawList.size();
    int order = 0;
    while (cnt)
    {
        QMap<int, QObject *>::const_iterator it = d->earlyDrawList.constFind(order);
        while (it != d->earlyDrawList.constEnd())
        {
            // TODO: make more than just skybox work with early draw
            Skybox *sb = qobject_cast<Skybox *>(*it);
            if (sb)
                sb->draw(painter);
            ++it;
            --cnt;
        }
        ++order;
    }

    if (d->blending)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter->glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter->glBlendEquation(GL_FUNC_ADD);
    else if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glDisable(GL_CULL_FACE);

    painter->setObjectPickId(-1);
    QObjectList list = QObject::children();
    painter->setMainLight(d->light,
                          camera() ? camera()->modelViewMatrix() : QMatrix4x4());
    painter->setLightModel(d->lightModel);
    foreach (QObject *child, list) {
        QQuickItem3D *item = qobject_cast<QQuickItem3D *>(child);
        if (item)
        {
            if (d->showSceneGraph && (d->dumpCount == 0))
                qDumpItem(item);
            item->draw(painter);
        }
    }
    if (d->dumpCount >= 0)
        --d->dumpCount;
}

/*!
  \internal
  Initialize the GL viewport for the first time on \a painter.
*/

void Viewport::initializeGL(QGLPainter *painter)
{
    // Initialize the Item3D objects attached to this scene.
    QObjectList list = QObject::children();
    foreach (QObject *child, list) {
        QQuickItem3D *item = qobject_cast<QQuickItem3D *>(child);
        if (item) {
            setItemViewport(item);
            item->initialize(painter);
        }
        // TODO: make this more generic with some sort of abstract interface
        // so that more than just skybox works with early draw
        Skybox *sb = qobject_cast<Skybox *>(child);
        if (sb)
        {
            sb->setViewport(this);
            registerEarlyDrawObject(sb, 0);
        }
    }
    d->itemsInitialized = true;
}

/*!
  \internal
    Registers \a obj with this viewport as a pickable object and
    return its pick identifier.
*/
int Viewport::registerPickableObject(QObject *obj)
{
    int id = (d->pickId)++;
    d->objects[id] = obj;
    return id;
}

void Viewport::registerEarlyDrawObject(QObject *obj, int order)
{
    d->earlyDrawList.insertMulti(order, obj);
}

PickEvent *Viewport::initiatePick(QMouseEvent *pick)
{
    PickEvent *p = new PickEvent;
    QMouseEvent *copy = new QMouseEvent(pick->type(), pick->localPos(), pick->screenPos(), pick->button(), pick->buttons(), pick->modifiers());
    p->setEvent(copy);
    {
        QMutexMaybeLocker locker(&d->pickEventQueueLock);
        if (d->pickEventQueue.size() > 10)
        {
            static bool warning_given = false;
            if (!warning_given)
            {
                qWarning("Pick event queue overflow");
                warning_given = true;
            }
            delete p;
            p = 0;
        }
        else
        {
            d->pickEventQueue.append(p);
        }
        locker.unlock();
    }

    update3d();
    return p;
}

/*!
    \internal
    Setup the \a painter with a camera focussed down onto the location
    that was picked by the \a pt.  Also set up all the other GL state
    required to paint the pick scene into the FBO.
*/
void Viewport::setupPickPaint(QGLPainter *painter, const QPointF &pt)
{
    painter->setPicking(true);
    painter->clearPickObjects();
    d->setRenderSettings(painter);

    painter->setEye(QGL::NoEye);

    QScopedPointer<QGLCamera> cam;
    if (d->camera)
    {
        cam.reset(d->camera->clone(0));
    } else {
        cam.reset(new QGLCamera);
    }

    float vw = cam->viewSize().width();
    float vh = cam->viewSize().height();
    float asp = 1.0f;
    if (cam->adjustForAspectRatio())
    {
        // see QGLCamera::projectionMatrix for this logic
        asp = width() / height();
        if (asp > 1.0f)
            vw *= asp;
        else
            vh /= asp;
    }

    // make sure that our viewport has the same aspect ratio as our
    // camera viewsize
    Q_ASSERT(qFuzzyCompare((vw / width()), (vh / height())));

    // shrink the camera view size down to the size of the FBO relative
    // to the viewports near plane size - note that the vw / width() and
    // vh / height() should evaluate to the same thing.
    cam->setAdjustForAspectRatio(false);

    // map the pick to coordinate system with origin at center of viewport
    float dx = pt.x() - (width() / 2.0f);
    float dy = pt.y() - (height() / 2.0f);
    dy = -dy;  // near plane coord system is correct, opengl style, not upside down like qt
    dx *= vw / width();
    dy *= vh / height();
    float dim = qMin(width(), height());
    Q_ASSERT(cam->viewSize().width() == cam->viewSize().height());  // viewsize is square

    painter->setCamera(cam.data());

    QVector3D sideVec = QVector3D::crossProduct(cam->center() - cam->eye(), cam->upVector());
    sideVec.normalize();
    sideVec *= -dx;
    QVector3D upVec = cam->upVector() * -dy;
    QVector3D tx = sideVec + upVec;
    QMatrix4x4 m;
    m.translate(tx);

    QMatrix4x4 s;
    float fac = dim / float(FBO_SIZE);
    s.scale(QVector3D(fac, fac, 0.0f));

    painter->projectionMatrix() = s * m * painter->projectionMatrix().top();
}

/*!
  \internal
    Finds the registered object that is under the mouse position
    specified by the queued pick events.  Pick events are posted by
    the various mouse event handlers.

    To do this an FBO is painted using picking mode, and the painter
    returns the object identified by its pick id.  Since the FBO is
    focussed to the part of the viewport where the pick occurred using
    view frustum culling will lead to good efficiencies in picking.

    This function runs in the rendering thread in order to gain access
    to the GL context.
*/
void Viewport::objectForPoint()
{
    QSize fbosize(QSize(FBO_SIZE, FBO_SIZE));
    PickEvent *p = 0;
    while (true)
    {
        {
            p = 0;
            QMutexMaybeLocker locker(&d->pickEventQueueLock);
            if (d->pickEventQueue.size() > 0)
                p = d->pickEventQueue.takeFirst();
            locker.unlock();
        }
        if (!p)
            break;

        QPointF pt = p->event()->pos();
        // Check the viewport boundaries in case a mouse move has
        // moved the pointer outside the window.
        QRectF rect = boundingRect();
        if (!rect.contains(pt)) {
            delete p;
            continue;
        }

        if (!d->pickFbo)
        {
            d->pickFbo = new QOpenGLFramebufferObject(fbosize,
                                                  QOpenGLFramebufferObject::CombinedDepthStencil);
        }
        int objectId = -1;
        QScopedPointer<QGLAbstractSurface> fboSurf(new QGLFramebufferObjectSurface(d->pickFbo));
        {
            QGLPainter painter;
            if (painter.begin(fboSurf.data()))
            {
                int winToFboRatioW = width() / FBO_SIZE;
                int winToFboRatioH = height() / FBO_SIZE;
                setupPickPaint(&painter, pt);
                draw(&painter);
                painter.setPicking(false);
                objectId = painter.pickObject(pt.x() / winToFboRatioW, pt.y() / winToFboRatioH);
                d->setDefaults(&painter);
            } else {
                qWarning() << "Warning: unable to paint into fbo, picking will be unavailable";
                continue;
            }
        }
        QObject *obj = d->objects.value(objectId, 0);
        d->lastObject = obj;
        p->setObject(obj);
        QMetaMethod m = metaObject()->method(p->callback());
        m.invoke(this, Qt::QueuedConnection, Q_ARG(void*, p));
    }
}

/*!
  \internal
*/
void Viewport::update3d()
{
    if (renderMode() == DirectRender) {
        if (d->canvas)
            d->canvas->update();
    }
    else {
        update();
    }
}

/*!
    \internal
*/
void Viewport::cameraChanged()
{
    update3d();
}

static inline void sendEnterEvent(QObject *object)
{
    QEvent event(QEvent::Enter);
    QCoreApplication::sendEvent(object, &event);
}

static inline void sendLeaveEvent(QObject *object)
{
    QEvent event(QEvent::Leave);
    QCoreApplication::sendEvent(object, &event);
}

/*!
    \internal
*/
void Viewport::mousePressEvent(QMouseEvent *e)
{
    static int processMousePressInvocation = -1;
    if (processMousePressInvocation == -1)
    {
        processMousePressInvocation = metaObject()->indexOfMethod("processMousePress(PickEvent*)");
        Q_ASSERT(processMousePressInvocation != -1);
    }
    if (!d->panning && d->picking)
    {
        PickEvent * p = initiatePick(e);
        if (p)
            p->setCallback(processMousePressInvocation);
        e->setAccepted(true); //This is necessary, otherwise we won't get a realese event
        return;
    }
    if (d->navigation && e->button() == Qt::LeftButton)
    {
        processNavEvent(e);
    }
    else
    {
        QQuickItem::mousePressEvent(e);
        return;
    }
    e->setAccepted(true);
}

void Viewport::processMousePress(PickEvent *pick)
{
    QObject *object = pick->object();
    QMouseEvent *e = pick->event();
    if (d->pressedObject)
    {
        // Send the press event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonPress,
             (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(d->pressedObject, &event);
    }
    else if (object)
    {
        // Record the object that was pressed and forward the event.
        d->pressedObject = object;
        d->enteredObject = 0;
        d->pressedButton = e->button();

        // Send a mouse press event for (0, 0).
        QMouseEvent event(QEvent::MouseButtonPress, QPoint(0, 0),
                          e->screenPos(), e->button(), e->buttons(),
                          e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    }

    if (d->navigation && e->button() == Qt::LeftButton)
    {
        processNavEvent(e);
    }
    delete pick;
}

void Viewport::processNavEvent(QMouseEvent *e)
{
    d->panning = true;
    d->lastPan = d->startPan = e->pos();
    d->startEye = d->camera->eye();
    d->startCenter = d->camera->center();
    d->startUpVector = d->camera->upVector();
    d->panModifiers = e->modifiers();
}

/*!
    \internal
*/
void Viewport::mouseReleaseEvent(QMouseEvent *e)
{
    static int processMouseReleaseInvocation = -1;
    if (processMouseReleaseInvocation == -1)
    {
        processMouseReleaseInvocation = metaObject()->indexOfMethod("processMouseRelease(PickEvent*)");
        Q_ASSERT(processMouseReleaseInvocation != -1);
    }
    if (d->panning && e->button() == Qt::LeftButton) {
        d->panning = false;
    }
    if (d->pressedObject) {
        PickEvent *p = initiatePick(e);
        if (p)
            p->setCallback(processMouseReleaseInvocation);
        e->setAccepted(true);
    } else {
        QQuickItem::mouseReleaseEvent(e);
    }
}

void Viewport::processMouseRelease(PickEvent *pick)
{
    Q_ASSERT(d->pressedObject);
    QObject *object = pick->object();
    QMouseEvent *e = pick->event();

    // Notify the previously pressed object about the release.
    QObject *pressed = d->pressedObject;
    if (e->button() == d->pressedButton) {
        d->pressedObject = 0;
        d->pressedButton = Qt::NoButton;
        d->enteredObject = object;

        // Send the release event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonRelease,
             (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(pressed, &event);

        // Send leave and enter events if necessary.
        if (object != pressed) {
            sendLeaveEvent(pressed);
            if (object)
                sendEnterEvent(object);
        }
    } else {
        // Some other button than the original was released.
        // Forward the event to the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonRelease,
             (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(pressed, &event);
    }
    delete pick;
}

/*!
    \internal
*/
void Viewport::mouseDoubleClickEvent(QMouseEvent *e)
{
    static int processMouseDoubleClickInvocation = -1;
    if (processMouseDoubleClickInvocation == -1)
    {
        processMouseDoubleClickInvocation = metaObject()->indexOfMethod("processMouseDoubleClick(PickEvent*)");
        Q_ASSERT(processMouseDoubleClickInvocation != -1);
    }
    if (d->picking) {
        PickEvent * p = initiatePick(e);
        if (p)
            p->setCallback(processMouseDoubleClickInvocation);
    }
    QQuickItem::mouseDoubleClickEvent(e);
}

void Viewport::processMouseDoubleClick(PickEvent *pick)
{
    QObject *object = pick->object();
    QMouseEvent *e = pick->event();
    if (object) {
        // Simulate a double click event for (0, 0).
        QMouseEvent event
            (QEvent::MouseButtonDblClick, QPoint(0, 0),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(object, &event);
        e->setAccepted(true);
    }
    delete pick;
}

/*!
    \internal
    Returns true if the mouse move represented by the event \a e is too
    quickly posted after previous events.  Here "too quickly" is defined
    to be true when the distance from one point to the next is less than
    3 pixels.  If there are no previous events, or if the most recent event
    was not a move, then false is returned.
*/
bool Viewport::mouseMoveOverflow(QMouseEvent *e) const
{
    bool result = false;
    QMutexMaybeLocker locker(&d->pickEventQueueLock);
    if (d->pickEventQueue.size() > 0)
    {
        PickEvent *p = d->pickEventQueue.last();
        QEvent::Type t = p->event()->type();
        if (t == QEvent::MouseMove
                || t == QEvent::HoverMove
                || t == QEvent::HoverEnter
                || t == QEvent::HoverLeave)
        {
            QPointF delta = p->event()->pos() - e->pos();
            result = qRound(delta.manhattanLength()) < 3;
        }
    }
    locker.unlock();
    return result;
}

/*!
    \internal
*/
void Viewport::mouseMoveEvent(QMouseEvent *e)
{
    static int processMouseMoveInvocation = -1;
    if (processMouseMoveInvocation == -1)
    {
        processMouseMoveInvocation = metaObject()->indexOfMethod("processMouseMove(PickEvent*)");
        Q_ASSERT(processMouseMoveInvocation != -1);
    }
    if (d->panning) {
        QPointF delta = e->pos() - d->startPan;
        if (e->modifiers() == d->panModifiers) {
            d->camera->setEye(d->startEye);
            d->camera->setCenter(d->startCenter);
            d->camera->setUpVector(d->startUpVector);
        } else {
            d->startPan = d->lastPan;
            delta = e->pos() - d->startPan;
            d->startEye = d->camera->eye();
            d->startCenter = d->camera->center();
            d->startUpVector = d->camera->upVector();
            d->panModifiers = e->modifiers();
        }
        d->lastPan = e->pos();
        if ((e->modifiers() & Qt::ControlModifier) != 0)
            wheel(delta.y() * -60);
        else if ((e->modifiers() & Qt::ShiftModifier) != 0)
            pan(delta.x(), delta.y());
        else
            rotate(delta.x(), delta.y());
    } else if (d->picking && !mouseMoveOverflow(e)) {
        PickEvent * p = initiatePick(e);
        if (p)
            p->setCallback(processMouseMoveInvocation);
    } else {
        QQuickItem::mouseMoveEvent(e);
        return;
    }
    e->setAccepted(true);
}

void Viewport::processMouseMove(PickEvent *pick)
{
    QObject *object = pick->object();
    QMouseEvent *e = pick->event();
    if (d->pressedObject) {
        // Send the move event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseMove,
             (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(d->pressedObject, &event);
    } else if (object) {
        if (object != d->enteredObject) {
            if (d->enteredObject)
                sendLeaveEvent(d->enteredObject);
            d->enteredObject = object;
            sendEnterEvent(d->enteredObject);
        }
        QMouseEvent event
            (QEvent::MouseMove, QPoint(0, 0),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    } else if (d->enteredObject) {
        sendLeaveEvent(d->enteredObject);
        d->enteredObject = 0;
    } else {
        QQuickItem::mouseMoveEvent(e);
        delete pick;
        return;
    }
    delete pick;
}

/*!
    \internal
*/
void Viewport::hoverEnterEvent(QHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QQuickItem::hoverEnterEvent(e);
}

/*!
    \internal
*/
void Viewport::hoverMoveEvent(QHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QQuickItem::hoverMoveEvent(e);
}

/*!
    \internal
*/
void Viewport::hoverLeaveEvent(QHoverEvent *e)
{
    if (!d->pressedObject && d->enteredObject) {
        sendLeaveEvent(d->enteredObject);
        d->enteredObject = 0;
        e->setAccepted(true);
    } else {
        QQuickItem::hoverLeaveEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::wheelEvent(QWheelEvent *e)
{
    if (d->navigation) {
        wheel(e->delta());
        e->setAccepted(true);
    } else {
        QQuickItem::wheelEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::keyPressEvent(QKeyEvent *e)
{
    // Process the "Keys" property on the item first.
    float sep;

    if (!d->navigation) {
        QQuickItem::keyPressEvent(e);
        return;
    }

    switch (e->key()) {

    case Qt::Key_Left:
    {
        if ((e->modifiers() & Qt::ShiftModifier) != 0) {
            pan(-10, 0);
        } else if ((e->modifiers() & Qt::ControlModifier) != 0) {
            sep = d->camera->eyeSeparation();
            sep -= (sep / 10.0f);
            if (sep < 0.0f)
                sep = 0.0f;
            d->camera->setEyeSeparation(sep);
        } else {
            rotate(-10, 0);
        }
    }
    break;

    case Qt::Key_Right:
    {
        if ((e->modifiers() & Qt::ShiftModifier) != 0) {
            pan(10, 0);
        } else if ((e->modifiers() & Qt::ControlModifier) != 0) {
            sep = d->camera->eyeSeparation();
            sep += (sep / 10.0f);
            d->camera->setEyeSeparation(sep);
        } else {
            rotate(10, 0);
        }
    }
    break;

    case Qt::Key_Up:
    {
        if ((e->modifiers() & Qt::ControlModifier) != 0)
            wheel(120);
        else if ((e->modifiers() & Qt::ShiftModifier) != 0)
            pan(0, -10);
        else
            rotate(0, -10);
    }
    break;

    case Qt::Key_Down:
    {
        if ((e->modifiers() & Qt::ControlModifier) != 0)
            wheel(-120);
        else if ((e->modifiers() & Qt::ShiftModifier) != 0)
            pan(0, 10);
        else
            rotate(0, 10);
    }
    break;

    default:
        QQuickItem::keyPressEvent(e);
        return;
    }

    e->setAccepted(true);
}

/*!
    \internal
*/
bool Viewport::hoverEvent(QHoverEvent *e)
{
    static int processMouseHoverInvocation = -1;
    if (processMouseHoverInvocation == -1)
    {
        processMouseHoverInvocation = metaObject()->indexOfMethod("processMouseHover(PickEvent*)");
        Q_ASSERT(processMouseHoverInvocation != -1);
    }
    if (!d->panning && d->picking) {
        QMouseEvent ev(QMouseEvent::MouseButtonPress, e->posF(), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        if (!mouseMoveOverflow(&ev))
        {
            PickEvent *p = initiatePick(&ev);
            if (p)
                p->setCallback(processMouseHoverInvocation);
        }
        return true;
    }
    return false;
}

void Viewport::processMouseHover(PickEvent *pick)
{
    QObject *object = pick->object();
    QMouseEvent *e = pick->event();

    if (d->pressedObject) {
        // Send the move event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseMove,
             (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->pos(), Qt::NoButton, Qt::NoButton, e->modifiers());
        QCoreApplication::sendEvent(d->pressedObject, &event);
    } else if (object) {
        if (object != d->enteredObject) {
            if (d->enteredObject)
                sendLeaveEvent(d->enteredObject);
            d->enteredObject = object;
            sendEnterEvent(d->enteredObject);
        }
        QMouseEvent event
            (QEvent::MouseMove, QPoint(0, 0),
             e->pos(), Qt::NoButton, Qt::NoButton, e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    } else if (d->enteredObject) {
        sendLeaveEvent(d->enteredObject);
        d->enteredObject = 0;
    }
    delete pick;
}

// Zoom in and out according to the change in wheel delta.
void Viewport::wheel(float delta)
{
    if (d->fovzoom) {
        //Use field-of view as zoom (much like a traditional camera)
        float scale = qAbs(viewDelta(delta, delta).x());
        if (delta < 0)
            scale = -scale;
        if (scale >= 0.0f)
            scale += 1.0f;
        else
            scale = 1.0f / (1.0f - scale);
        float fov = d->camera->fieldOfView();
        if (fov != 0.0f)
            d->camera->setFieldOfView(d->camera->fieldOfView() / scale);
        else
            d->camera->setViewSize(d->camera->viewSize() / scale);
    } else {
        // enable this to get wheel navigation that actually zooms by moving the
        // camera back, as opposed to making the angle of view wider.
        QVector3D viewVector= camera()->eye() - camera()->center();
        float zoomMag = viewVector.length();
        float zoomIncrement = -float(delta) / 100.0f;
        if (!qFuzzyIsNull(zoomIncrement))
        {
            zoomMag += zoomIncrement;
            if (zoomMag < 1.0f)
                zoomMag = 1.0f;

            QRay3D viewLine(camera()->center(), viewVector.normalized());
            camera()->setEye(viewLine.point(zoomMag));
        }
    }
}

// Pan left/right/up/down without rotating about the object.
void Viewport::pan(float deltax, float deltay)
{
    QPointF delta = viewDelta(deltax, deltay);
    QVector3D t = d->camera->translation(delta.x(), -delta.y(), 0.0f);

    // Technically panning the eye left should make the object appear to
    // move off to the right, but this looks weird on-screen where the user
    // actually thinks they are picking up the object and dragging it rather
    // than moving the eye.  We therefore apply the inverse of the translation
    // to make it "look right".
    d->camera->setEye(d->camera->eye() - t);
    d->camera->setCenter(d->camera->center() - t);
}

// Rotate about the object being viewed.
void Viewport::rotate(float deltax, float deltay)
{
    QRectF rect = boundingRect();
    int rotation = d->camera->screenRotation();
    if (rotation == 90 || rotation == 270) {
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    float anglex = deltax * 90.0f / rect.width();
    float angley = deltay * 90.0f / rect.height();
    QQuaternion q = d->camera->pan(-anglex);
    q *= d->camera->tilt(-angley);
    d->camera->rotateCenter(q);
}

// Convert deltas in the X and Y directions into percentages of
// the view width and height.
QPointF Viewport::viewDelta(float deltax, float deltay)
{
    QRectF rect = boundingRect();
    float w = rect.width();
    float h = rect.height();
    bool scaleToWidth;
    float scaleFactor, scaleX, scaleY;
    QSizeF viewSize = d->camera->viewSize();
    if (w >= h) {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = true;
        else
            scaleToWidth = false;
    } else {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = false;
        else
            scaleToWidth = true;
    }
    int rotation = d->camera->screenRotation();
    if (rotation == 90 || rotation == 270) {
        scaleToWidth = !scaleToWidth;
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    if (scaleToWidth) {
        scaleFactor = 2.0f / viewSize.width();
        scaleX = scaleFactor * h / w;
        scaleY = scaleFactor;
    } else {
        scaleFactor = 2.0f / viewSize.height();
        scaleX = scaleFactor;
        scaleY = scaleFactor * w / h;
    }
    return QPointF(deltax * scaleX / w, deltay * scaleY / h);
}

void Viewport::itemChange(QQuickItem::ItemChange change, const ItemChangeData &value)
{
    if (change == QQuickItem::ItemChildAddedChange)
    {
        QQuickItem *newItem = value.item;
        if (QQuickItem3D* item3d =
                qobject_cast<QQuickItem3D*>(newItem))
        {
            setItemViewport(item3d);
        }
    }
    if (change == ItemSceneChange && value.window != d->canvas)
    {
        if (d->canvas)
        {
            d->canvas->disconnect(this);
        }
        d->canvas = value.window;
        d->directRenderInitialized = false;
        if (d->canvas)
        {
            if (d->canvas->openglContext() != NULL)
            {
                sceneGraphInitialized();
            } else {
                connect(d->canvas, SIGNAL(sceneGraphInitialized()),
                        this, SLOT(sceneGraphInitialized()),
                        Qt::DirectConnection);
            }
            connect(d->canvas, SIGNAL(destroyed()),
                    this, SLOT(canvasDeleted()));
            QSurfaceFormat format = d->canvas->format();
            if (format.depthBufferSize() != 24)
            {
                format.setDepthBufferSize(24);
                d->canvas->setFormat(format);
            }
        }
    }

    return QQuickItem::itemChange(change, value);
}

QSGNode* Viewport::updatePaintNode(QSGNode* node, UpdatePaintNodeData* data)
{
    Q_UNUSED(node);
    Q_UNUSED(data);
    if (d->renderMode == BufferedRender)
        return QQuickPaintedItem::updatePaintNode(node, data);
    return 0;
}

void Viewport::sceneGraphInitialized()
{
    // TODO: this function is called from the rendering thread first time thru.
    // Check to see if there are concurrency issues.  Maybe not since the only
    // thing that is running at this point is this thread.

    // This function is the approved SG spot for getting the initialized scene
    // graph canvas.  Its the earliest point when we can be sure the canvas is
    // properly set up.  Note that it will usually occur well after the main
    // objects have been initialised (when the QQuickView is created) and after the
    // initial QML is loaded (which causes the initialisation of specific settings
    // for the render mode).
    Q_ASSERT(d->canvas);
    if (renderMode() == UnknownRender)
    {
        if (d->canvas->contentItem() != parentItem())
        {
#ifdef Q_DEBUG_VIEWPORT
            qWarning() << "Viewport not the top level item - has parent %1:"
                       << parentItem()->metaObject()->className()
                       << "- so switching to FBO composition.  For direct \'GL Under\' "
                          "rendering make this viewport the top-level item or explicitly "
                          "set the renderMode property.";
#endif
            setRenderMode(BufferedRender);
        }
        else
        {
            setRenderMode(DirectRender);
        }
    }
    if (!d->directRenderInitialized && renderMode() == DirectRender)
    {
        // this could happen if the call to setRenderMode occurred when there
        // was no canvas or engine
        connect(d->canvas, SIGNAL(beforeRendering()),
                this, SLOT(beforeRendering()), Qt::DirectConnection);
        d->canvas->setClearBeforeRendering(false);
        d->directRenderInitialized = true;
    }
    if (!d->pickingRenderInitialized && d->picking)
    {
        connect(d->canvas, SIGNAL(beforeRendering()),
                this, SLOT(objectForPoint()), Qt::DirectConnection);
        d->pickingRenderInitialized = true;
    }
}

void Viewport::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    setSize(newGeometry.size());
}

void Viewport::canvasDeleted()
{
    d->canvas = 0;
}

QT_END_NAMESPACE
