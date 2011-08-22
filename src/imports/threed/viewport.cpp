/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "viewport.h"
#include "qdeclarativeeffect.h"

#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglcamera.h"
#include "qglview.h"
#include "qglsubsurface.h"
#include "qray3d.h"
#include "qglframebufferobjectsurface.h"
#include "skybox.h"

#include <QtGui/qpainter.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qevent.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qcoreapplication.h>
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QSGCanvas>
#include <QSGEngine>
#include <QtOpenGL/qglbuffer.h>
#include <QtCore/qthread.h>

/*!
    \qmlclass Viewport Viewport
    \brief The Viewport item defines the logical viewport for a 3D scene.  It includes all necessary
    references and parameters for the contents of the scene, as well as drawing and painting functions
    \since 4.8
    \ingroup qt3d::qml3d

    The Viewport item is usually the outermost in a 3D scene, specifying
    the size of the view, the camera position, lights, and the main 3D object:

    \code
    import QtQuick 1.0
    import Qt3D 1.0

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

class ViewportPrivate
{
public:
    ViewportPrivate();
    ~ViewportPrivate();

    bool picking;
    bool showPicking;
    bool navigation;
    bool fovzoom;
    bool blending;
    bool itemsInitialized;
    bool needsPick;
    bool needsRepaint;
    QGLCamera *camera;
    QGLLightParameters *light;
    QGLLightModel *lightModel;
    QWidget *viewWidget;
    int pickId;
    QGLFramebufferObject *pickFbo;
    QMap<int, QObject *> objects;
    QObject *pressedObject;
    Qt::MouseButton pressedButton;
    QObject *enteredObject;
    bool panning;
    QPointF startPan;
    QPointF lastPan;
    QVector3D startEye;
    QVector3D startCenter;
    QVector3D startUpVector;
    Qt::KeyboardModifiers panModifiers;
    QMap<int, QObject*> earlyDrawList;
    Viewport::RenderMode renderMode;

    QSGCanvas* canvas;

    void setDefaults(QGLPainter *painter);
    void setRenderSettings(QGLPainter *painter);
};

ViewportPrivate::ViewportPrivate()
    : picking(false)
    , showPicking(false)
    , navigation(true)
    , fovzoom(true)
    , blending(false)
    , itemsInitialized(false)
    , needsPick(true)
    , needsRepaint(true)
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
    , panModifiers(Qt::NoModifier)
    , renderMode(Viewport::UnknownRender)
    , canvas(0)
{
}

ViewportPrivate::~ViewportPrivate()
{
    delete pickFbo;
}

void ViewportPrivate::setDefaults(QGLPainter *painter)
{
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

    painter->setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*!
    \internal
    Construct the class and assign it a \a parent QSGItem.
*/
Viewport::Viewport(QSGItem *parent)
    : QSGPaintedItem(parent)
    , d(new ViewportPrivate())
{
    // Has to be set or we crash on render.  Intention of this is that
    // the update() function gets called when a re-render is needed.
    setFlags(QSGItem::ItemHasContents);

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
        \o UnknownRender  The mode is not specified.
        \o DirectRender  Render to the GL context directly.  This is the default for top-level viewports.
        \o BufferedRender  Render to an offscreen buffer.  This is the default for a viewport that is a child.
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
        Q_ASSERT(d->canvas != 0);
        QSGEngine* engine =  d->canvas->sceneGraphEngine();
        d->renderMode = mode;
        if (d->renderMode == BufferedRender)
        {
            setRenderTarget(QSGPaintedItem::FramebufferObject);
            //setFlags(flags() | QSGItem::ItemHasContents);
            // TODO: buggy if theres another viewport
            // if (engine)
            //     engine->setClearBeforeRendering(true);
        }
        else
        {
            //setFlags(flags() & ~QSGItem::ItemHasContents);

            if (!engine)
            {
                qmlInfo(this) << tr("Unable to get QSGEngine. Will not be able to render!");
                return;
            }
            connect((QObject*)engine, SIGNAL(beforeRendering()),
                    this, SLOT(beforeRendering()), Qt::DirectConnection);
            engine->setClearBeforeRendering(false);
        }
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
    ViewportSubsurface(QGLAbstractSurface *surface, const QRect &region,
                       qreal adjust)
        : QGLSubsurface(surface, region), m_adjust(adjust) {}

    qreal aspectRatio() const;

private:
    qreal m_adjust;
};

qreal ViewportSubsurface::aspectRatio() const
{
    return QGLSubsurface::aspectRatio() * m_adjust;
}

/*!
    \reimp
    \internal

    Only used in the case of renderMode() == BufferedRender.

    Called by QSGPaintedItem to refresh the content.

    \sa beforeRendering() setRenderMode()
*/
void Viewport::paint(QPainter *painter)
{
    Q_ASSERT(renderMode() == BufferedRender);

    QGLPainter glPainter;
    if (!glPainter.begin(painter))
    {
        qWarning("GL graphics system is not active; cannot use 3D items");
        return;
    }
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    render(&glPainter);

    d->setDefaults(&glPainter);
}

/*!
    \internal

    Only used in the case of renderMode() == DirectRender.

    Called by beforeRendering signal from QSGContext.

    \sa paint()
*/
void Viewport::beforeRendering()
{
    Q_ASSERT(renderMode() == DirectRender);

    // Note: this slot will be executed in the QSG rendering thread
    // (Qt::DirectConnection) - not in the GUI/main thread of the app.
    // Beware of thread-safety issues.

    if (!isVisible() || !d->needsRepaint)
        return;

    Q_ASSERT(d->canvas && d->canvas->isValid());
    Q_ASSERT(d->canvas->context() != 0);

    QGLPainter painter;
    if (!painter.begin())
    {
        qmlInfo(this) << tr("Unable to paint 3D items!");
        return;
    }

    d->setRenderSettings(&painter);
    render(&painter);

    d->needsRepaint = false;
}

void Viewport::render(QGLPainter *painter)
{
    d->needsPick = true;

    // Initialize the objects in the scene if this is the first paint.
    if (!d->itemsInitialized)
        initializeGL(painter);

    // No stereo rendering, set the eye as neutral
    painter->setEye(QGL::NoEye);

    // TODO
    // Deal with transforms on the object, which change the viewport?

    // boundingRect is in local coordinates. We need to map it to the scene coordinates
    // in order to render to correct area.
    QRect viewport = mapRectToScene(boundingRect()).toRect();
    qreal adjust = 1.0f;
    ViewportSubsurface surface(painter->currentSurface(), viewport, adjust);
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
    painter->setMainLight(d->light, QMatrix4x4());
    painter->setLightModel(d->lightModel);
    foreach (QObject *child, list) {
        QDeclarativeItem3D *item = qobject_cast<QDeclarativeItem3D *>(child);
        if (item)
            item->draw(painter);
    }
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
        QDeclarativeItem3D *item = qobject_cast<QDeclarativeItem3D *>(child);
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

/*!
  \internal
    Returns the registered object that is under the mouse position
    specified by (\a x, \a y).  This function may need to regenerate
    the contents of the pick buffer by repainting the scene.
*/
QObject *Viewport::objectForPoint(qreal x, qreal y)
{
    // Check the viewport boundaries in case a mouse move has
    // moved the pointer outside the window.
    QRectF rect = boundingRect();
    if (!rect.contains(QPointF(x, y)))
        return 0;

    if (!QGLContext::currentContext()) {
        // Won't be able to read or generate a pick buffer, so bail out.
        return 0;
    }

    QGLPainter painter;
    bool painterBegun = false;
    QSize size(qRound(width()), qRound(height()));
    QSize fbosize(QGL::nextPowerOfTwo(size));

    int objectId = -1;

    if (d->pickFbo && d->pickFbo->size() != fbosize) {
        delete d->pickFbo;
        d->pickFbo = 0;
        d->needsPick = true;
    };

    if (!d->pickFbo) {
        d->pickFbo = new QGLFramebufferObject
                (fbosize, QGLFramebufferObject::CombinedDepthStencil);
        Q_ASSERT(d->needsPick == true);
    }

    QScopedPointer<QGLAbstractSurface> fboSurface(new QGLFramebufferObjectSurface(d->pickFbo));
    // The fbo has to be a power of 2, so use a subsurface to get the
    // actual size we want
    QScopedPointer<QGLSubsurface> subSurface(new QGLSubsurface(fboSurface.data(), QRect(QPoint(0, 0), size)));

    if ( d->needsPick ) {
        // Regenerate the pick fbo contents.
        if (painter.begin(subSurface.data())) {
            painter.setPicking(true);
            painter.clearPickObjects();
            painter.setClearColor(Qt::black);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            painter.setEye(QGL::NoEye);
            if (d->camera) {
                painter.setCamera(d->camera);
            } else {
                QGLCamera defCamera;
                painter.setCamera(&defCamera);
            }
            draw(&painter);
            painter.setPicking(false);
        } else {
            qWarning() << "Warning: unable to paint into fbo, picking will be unavailable";
        }
    }

    Q_ASSERT(d->pickFbo);
    if (!painterBegun)
        painter.begin(subSurface.data());
    objectId = painter.pickObject(qRound(x), fbosize.height() - 1 - qRound(y));
    painter.end();
    d->needsPick = false;

    // surfaces cleaned up here by scoped pointer deleters.
    return d->objects.value(objectId, 0);
}

/*!
  \internal
*/
void Viewport::update3d()
{
    update();
    d->needsRepaint = true;
}

/*!
    \internal
*/
void Viewport::cameraChanged()
{
    update();
    d->needsRepaint = true;
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
void Viewport::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QObject *object;
    if (!d->panning && d->picking)
        object = objectForPoint(e->pos());
    else
        object = 0;
    if (d->pressedObject) {
        // Send the press event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonPress,
             (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(d->pressedObject, &event);
    } else if (object) {
        // Record the object that was pressed and forward the event.
        d->pressedObject = object;
        d->enteredObject = 0;
        d->pressedButton = e->button();

        // Send a mouse press event for (0, 0).
        QMouseEvent event(QEvent::MouseButtonPress, QPoint(0, 0),
                          e->screenPos(), e->button(), e->buttons(),
                          e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    } else if (d->navigation && e->button() == Qt::LeftButton) {
        d->panning = true;
        d->lastPan = d->startPan = e->pos();
        d->startEye = d->camera->eye();
        d->startCenter = d->camera->center();
        d->startUpVector = d->camera->upVector();
        d->panModifiers = e->modifiers();
    } else {
        QSGItem::mousePressEvent(e);
        return;
    }
    e->setAccepted(true);
}

/*!
    \internal
*/
void Viewport::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (d->panning && e->button() == Qt::LeftButton) {
        d->panning = false;
    }
    if (d->pressedObject) {
        // Notify the previously pressed object about the release.
        QObject *object = objectForPoint(e->pos());
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
        e->setAccepted(true);
    } else {
        QSGItem::mouseReleaseEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    if (d->picking) {
        QObject *object = objectForPoint(e->pos());
        if (object) {
            // Simulate a double click event for (0, 0).
            QMouseEvent event
                (QEvent::MouseButtonDblClick, QPoint(0, 0),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
            e->setAccepted(true);
            return;
        }
    }
    QSGItem::mouseDoubleClickEvent(e);
}

/*!
    \internal
*/
void Viewport::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
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
    } else if (d->picking) {
        QObject *object = objectForPoint(e->pos());
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
            QSGItem::mouseMoveEvent(e);
            return;
        }
    } else {
        QSGItem::mouseMoveEvent(e);
        return;
    }
    e->setAccepted(true);
}

/*!
    \internal
*/
void Viewport::hoverEnterEvent(QHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QSGItem::hoverEnterEvent(e);
}

/*!
    \internal
*/
void Viewport::hoverMoveEvent(QHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QSGItem::hoverMoveEvent(e);
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
        QSGItem::hoverLeaveEvent(e);
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
        QSGItem::wheelEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::keyPressEvent(QKeyEvent *e)
{
    // Process the "Keys" property on the item first.
    qreal sep;

    if (!d->navigation) {
        QSGItem::keyPressEvent(e);
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
        QSGItem::keyPressEvent(e);
        return;
    }

    e->setAccepted(true);
}

/*!
    \internal
*/
bool Viewport::hoverEvent(QHoverEvent *e)
{
    if (!d->panning && d->picking) {
        QObject *object = objectForPoint(e->pos());
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
        return true;
    }
    return false;
}

// Zoom in and out according to the change in wheel delta.
void Viewport::wheel(qreal delta)
{
    if (d->fovzoom) {
        //Use field-of view as zoom (much like a traditional camera)
        qreal scale = qAbs(viewDelta(delta, delta).x());
        if (delta < 0)
            scale = -scale;
        if (scale >= 0.0f)
            scale += 1.0f;
        else
            scale = 1.0f / (1.0f - scale);
        qreal fov = d->camera->fieldOfView();
        if (fov != 0.0f)
            d->camera->setFieldOfView(d->camera->fieldOfView() / scale);
        else
            d->camera->setViewSize(d->camera->viewSize() / scale);
    } else {
        // enable this to get wheel navigation that actually zooms by moving the
        // camera back, as opposed to making the angle of view wider.
        QVector3D viewVector= camera()->eye() - camera()->center();
        qreal zoomMag = viewVector.length();
        qreal zoomIncrement = -float(delta) / 100.0f;
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
void Viewport::pan(qreal deltax, qreal deltay)
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
void Viewport::rotate(qreal deltax, qreal deltay)
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
    qreal anglex = deltax * 90.0f / rect.width();
    qreal angley = deltay * 90.0f / rect.height();
    QQuaternion q = d->camera->pan(-anglex);
    q *= d->camera->tilt(-angley);
    d->camera->rotateCenter(q);
}

// Convert deltas in the X and Y directions into percentages of
// the view width and height.
QPointF Viewport::viewDelta(qreal deltax, qreal deltay)
{
    QRectF rect = boundingRect();
    qreal w = rect.width();
    qreal h = rect.height();
    bool scaleToWidth;
    qreal scaleFactor, scaleX, scaleY;
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

void Viewport::itemChange(QSGItem::ItemChange change, const ItemChangeData &value)
{
    if (change == QSGItem::ItemChildAddedChange)
    {
        QSGItem *newItem = value.item;
        if (QDeclarativeItem3D* item3d =
                qobject_cast<QDeclarativeItem3D*>(newItem))
        {
            setItemViewport(item3d);
        }
    }
    if (change == ItemSceneChange && value.canvas != d->canvas)
    {
        if (d->canvas && d->canvas->sceneGraphEngine())
        {
            d->canvas->disconnect(this);
            d->canvas->sceneGraphEngine()->disconnect(this);
        }
        d->canvas = value.canvas;
        if (d->canvas)
        {
            connect(d->canvas, SIGNAL(sceneGraphInitialized()),
                    this, SLOT(sceneGraphInitialized()), Qt::DirectConnection);
            connect(d->canvas, SIGNAL(destroyed()),
                    this, SLOT(canvasDestroyed()));
        }
    }

    return QSGItem::itemChange(change, value);
}

void Viewport::canvasDestroyed()
{
    d->canvas = 0;
}

QSGNode* Viewport::updatePaintNode(QSGNode* node, UpdatePaintNodeData* data)
{
    Q_UNUSED(node);
    Q_UNUSED(data);
    if (d->renderMode == BufferedRender)
        return QSGPaintedItem::updatePaintNode(node, data);
    return 0;
}

void Viewport::sceneGraphInitialized()
{
    // This function is the approved SG spot for getting the initialized scene
    // graph canvas.  Its the earliest point when we can be sure the canvas is
    // properly set up.  Note that it will usually occur well after the main
    // objects have been initialised (when the QSGView is created) and after the
    // initial QML is loaded (which causes the initialisation of specific settings
    // for the render mode).
    if (renderMode() == UnknownRender)
    {
        Q_ASSERT(d->canvas);
        if (d->canvas->rootItem() != parentItem())
        {
            qWarning() << "Viewport has parent %1:"
                       << parentItem()->metaObject()->className()
                       << "so switching to FBO composition.  For direct \'GL Under\' "
                          "rendering make this viewport the top-level item.  If "
                          "renderMode property is explicitly set ignore this warning.";
            setRenderMode(BufferedRender);
        }
        else
        {
            setRenderMode(DirectRender);
        }
    }
}

void Viewport::geometryChanged(const QRectF &newGeometry, const QRectF & /*oldGeometry*/)
{
    setSize(newGeometry.size());
}

QT_END_NAMESPACE
