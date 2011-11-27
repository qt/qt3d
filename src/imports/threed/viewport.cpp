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
#include <QtCore/qpointer.h>

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
    bool showSceneGraph;
    int dumpCount;
    bool navigation;
    bool fovzoom;
    bool blending;
    bool itemsInitialized;
    bool needsPick;
    QGLCamera *camera;
    QGLLightParameters *light;
    QGLLightModel *lightModel;
    QWidget *viewWidget;
    int pickId;
    QGLFramebufferObject *pickFbo;
    QMap<int, QObject *> objects;
    QPointer<QObject> pressedObject;
    Qt::MouseButton pressedButton;
    QPointer<QObject> enteredObject;
    bool panning;
    QPointF startPan;
    QPointF lastPan;
    QVector3D startEye;
    QVector3D startCenter;
    QVector3D startUpVector;
    Qt::KeyboardModifiers panModifiers;
    QMap<int, QObject*> earlyDrawList;

    void setDefaults(QGLPainter *painter);
};

ViewportPrivate::ViewportPrivate()
    : picking(false)
    , showPicking(false)
    , showSceneGraph(false)
    , dumpCount(10)  // maybe this needs to be higher?
    , navigation(true)
    , fovzoom(false)
    , blending(false)
    , itemsInitialized(false)
    , needsPick(true)
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
{
}

ViewportPrivate::~ViewportPrivate()
{
    delete pickFbo;
}

void ViewportPrivate::setDefaults(QGLPainter *painter)
{
    // Set the default depth buffer options.
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

/*!
    \internal
    Construct the class and assign it a \a parent QDeclarativeItem.
*/
Viewport::Viewport(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    d = new ViewportPrivate();
    setFlag(QGraphicsItem::ItemHasNoContents, false);

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
    d->picking = value;
    emit viewportChanged();
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
    \internal
    The main paint function for the Viewport class.  It takes a  QPainter \a p, which performs the
    painting of objects in the 3d environment.

    The paint function is responsible for setting up the viewing transform, as well as other display
    options, before calling the draw function to perform the actual drawing of the scene.

    Note, currently \a style and \a widget are unused, but are reserved for later development.
*/
void Viewport::paint(QPainter *p, const QStyleOptionGraphicsItem * style, QWidget *widget)
{
    Q_UNUSED(style);

    if (!d->viewWidget)
        d->viewWidget = widget;
    d->needsPick = true;

    QGLPainter painter;
    if (!painter.begin(p)) {
        if (widget) {
            // Probably running with a plain QDeclarativeView (e.g. qmlviewer).
            // We need to have switched the surrounding QGraphicsView to use a
            // QGLWidget as its viewport.
            QGraphicsView *view =
                qobject_cast<QGraphicsView *>(widget->parentWidget());
            if (!view) {
                return;
            } else {
                QGLWidget *glw = qobject_cast<QGLWidget *>(view->viewport());
                if (!glw) {
                    qWarning("GL graphics system is not active; cannot use 3D items");
                    return;
                }
            }
        }
    }

    // Initialize the objects in the scene if this is the first paint.
    if (!d->itemsInitialized)
        initializeGL(&painter);

    painter.setEye(QGL::NoEye);
    qreal adjust = 1.0f;

    // Modify the GL viewport to only cover the extent of this QDeclarativeItem.
    QTransform transform = p->combinedTransform();
    QRect viewport = transform.mapRect(boundingRect()).toRect();
    ViewportSubsurface surface(painter.currentSurface(), viewport, adjust);
    painter.pushSurface(&surface);

    // Perform early drawing operations.
    earlyDraw(&painter);

    // Set up the camera the way QGLView would if we were using it.
    if (d->camera) {
        painter.setCamera(d->camera);
    } else {
        QGLCamera defCamera;
        painter.setCamera(&defCamera);
    }

    // Draw the Item3D children.
    painter.setPicking(d->showPicking);
    draw(&painter);
    painter.setPicking(false);
    painter.popSurface();

    // Disable the effect to return control to the GL paint engine.
    painter.disableEffect();

    // Try to restore the GL state to something paint-engine compatible.
    glDisable(GL_CULL_FACE);
    d->setDefaults(&painter);
    glDisable(GL_DEPTH_TEST);

}

/*!
  \internal
*/
void Viewport::earlyDraw(QGLPainter *painter)
{
    // If we have a parent, then assume that the parent has painted
    // the background and overpaint over the top of it.  If we don't
    // have a parent, then clear to black.
    if (parentItem()) {
        glClear(GL_DEPTH_BUFFER_BIT);
    } else {
        painter->setClearColor(Qt::black);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Force the effect to be updated.  The GL paint engine
    // has left the GL state in an unknown condition.
    painter->disableEffect();

#ifdef GL_RESCALE_NORMAL
    glEnable(GL_RESCALE_NORMAL);
#endif

    // Set the default effect for the scene.
    painter->setStandardEffect(QGL::LitMaterial);
    painter->setFaceColor(QGL::AllFaces, Qt::white);
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
    // Set up the initial depth, blend, and other options.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
    glDepthRangef(0.0f, 1.0f);
#else
    glDepthRange(0.0f, 1.0f);
#endif

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
    if (!d->viewWidget)
        return 0;

    // Check the viewport boundaries in case a mouse move has
    // moved the pointer outside the window.
    QRectF rect = boundingRect();
    if (!rect.contains(QPointF(x, y)))
        return 0;

    QPainter qpainter;
    QGLPainter painter;
    QGLWidget *glw = qobject_cast<QGLWidget *>(d->viewWidget);
    bool painterValid = false;
    bool doubleBuffer = false;
    if (glw && painter.begin(glw)) {
        doubleBuffer = glw->doubleBuffer();
        painterValid = true;
    } else if (qpainter.begin(d->viewWidget) && painter.begin(&qpainter)) {
        painterValid = true;
    }

    if (!painterValid && !QGLContext::currentContext()) {
        // Won't be able to read or generate a pick buffer, so bail out.
        return 0;
    }

    int objectId = -1;

    QSize size(qRound(width()), qRound(height()));
    QSize fbosize(QGL::nextPowerOfTwo(size));
    if (!d->needsPick && d->pickFbo && d->pickFbo->size() == fbosize
            && painterValid) {
        // The previous pick fbo contents should still be valid.
        d->pickFbo->bind();
        objectId = painter.pickObject(qRound(x), fbosize.height() - 1 - qRound(y));
        d->pickFbo->release();
    } else {
        // Regenerate the pick fbo contents.
        QGLAbstractSurface *mainSurface = 0;
        QGLAbstractSurface *fboSurface = 0;
        int height;
        if (!doubleBuffer) {
            if (d->pickFbo && d->pickFbo->size() != fbosize) {
                delete d->pickFbo;
                d->pickFbo = 0;
            }
            if (!d->pickFbo) {
                d->pickFbo = new QGLFramebufferObject
                    (fbosize, QGLFramebufferObject::CombinedDepthStencil);
            }
            height = fbosize.height();
            fboSurface = new QGLFramebufferObjectSurface(d->pickFbo);
            mainSurface = fboSurface;
        } else if (painterValid) {
            // Use the QGLWidget's back buffer for picking to avoid the
            // need to create a separate fbo in GPU memory.
            mainSurface = painter.currentSurface();
            height = mainSurface->viewportGL().height();
        }
        QGLSubsurface surface(mainSurface, QRect(QPoint(0, 0), size));
        // If our view is a hijacked QWidget, we're going to have failed
        // to find a QGLWidget, but we'll still have a QGLContext and be able
        // to draw the pick buffer into the pickFbo.
        if (painterValid || painter.begin(&surface)) {
            if (painterValid)
                painter.pushSurface(&surface);
            painter.setPicking(true);
            painter.clearPickObjects();
            painter.setClearColor(Qt::black);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            painter.setEye(QGL::NoEye);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
            glDepthRangef(0.0f, 1.0f);
#else
            glDepthRange(0.0f, 1.0f);
#endif
            glDisable(GL_BLEND);
            if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendColor))
                painter.glBlendColor(0, 0, 0, 0);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
                painter.glBlendEquation(GL_FUNC_ADD);
            else if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
                painter.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glDisable(GL_CULL_FACE);
            if (d->camera) {
                painter.setCamera(d->camera);
            } else {
                QGLCamera defCamera;
                painter.setCamera(&defCamera);
            }
            draw(&painter);
            painter.setPicking(false);
            objectId = painter.pickObject(qRound(x), height - 1 - qRound(y));
            painter.popSurface();
            painter.end();
        }
        delete fboSurface;
    }

    d->needsPick = doubleBuffer;
    return d->objects.value(objectId, 0);
}

/*!
  \internal
*/
void Viewport::update3d()
{
    update();
}

/*!
    \internal
*/
void Viewport::cameraChanged()
{
    update();
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
#ifndef QT_NO_CURSOR
        setCursor(Qt::ClosedHandCursor);
#endif
    } else {
        QDeclarativeItem::mousePressEvent(e);
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
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
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
        QDeclarativeItem::mouseReleaseEvent(e);
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
    QDeclarativeItem::mouseDoubleClickEvent(e);
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
            QDeclarativeItem::mouseMoveEvent(e);
            return;
        }
    } else {
        QDeclarativeItem::mouseMoveEvent(e);
        return;
    }
    e->setAccepted(true);
}

/*!
    \internal
*/
void Viewport::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QDeclarativeItem::hoverEnterEvent(e);
}

/*!
    \internal
*/
void Viewport::hoverMoveEvent(QGraphicsSceneHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QDeclarativeItem::hoverMoveEvent(e);
}

/*!
    \internal
*/
void Viewport::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    if (!d->pressedObject && d->enteredObject) {
        sendLeaveEvent(d->enteredObject);
        d->enteredObject = 0;
        e->setAccepted(true);
    } else {
        QDeclarativeItem::hoverLeaveEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::wheelEvent(QGraphicsSceneWheelEvent *e)
{
    if (d->navigation) {
        wheel(e->delta());
        e->setAccepted(true);
    } else {
        QDeclarativeItem::wheelEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::keyPressEvent(QKeyEvent *e)
{
    // Process the "Keys" property on the item first.
    keyPressPreHandler(e);
    if (e->isAccepted())
        return;

    qreal sep;

    if (!d->navigation) {
        QDeclarativeItem::keyPressEvent(e);
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
        QDeclarativeItem::keyPressEvent(e);
        return;
    }

    e->setAccepted(true);
}

/*!
    \internal
*/
bool Viewport::hoverEvent(QGraphicsSceneHoverEvent *e)
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
                 e->screenPos(), Qt::NoButton, Qt::NoButton, e->modifiers());
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
                 e->screenPos(), Qt::NoButton, Qt::NoButton, e->modifiers());
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

QVariant Viewport::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemChildAddedChange)
    {
        QGraphicsItem *newItem = value.value<QGraphicsItem*>();
        if (QDeclarativeItem3D* item3d =
                qobject_cast<QDeclarativeItem3D*>(newItem))
        {
            setItemViewport(item3d);
        }
    };
    return QGraphicsItem::itemChange(change, value);
}
QT_END_NAMESPACE
