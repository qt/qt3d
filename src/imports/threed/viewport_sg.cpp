/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "viewport_sg.h"
#include "viewportfbonode_sg.h"
#include <QtDeclarative/qsgview.h>
#include "qdeclarativeeffect_p.h"
#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglcamera.h"
#include "qglview.h"
#include "qglsubsurface.h"
#include "qglframebufferobjectsurface.h"
#include <QtDeclarative/node.h>
#include <QtGui/qpainter.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qevent.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

class ViewportSGPrivate
{
public:
    ViewportSGPrivate();
    ~ViewportSGPrivate();

    bool picking;
    bool showPicking;
    bool navigation;
    bool blending;
    bool itemsInitialized;
    bool needsPick;
    bool dirty;
    QGLCamera *camera;
    QGLLightParameters *light;
    QGLLightModel *lightModel;
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

    void setDefaults(QGLPainter *painter);
};

ViewportSGPrivate::ViewportSGPrivate()
    : picking(false)
    , showPicking(false)
    , navigation(true)
    , blending(false)
    , itemsInitialized(false)
    , needsPick(true)
    , dirty(true)
    , camera(0)
    , light(0)
    , lightModel(0)
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

ViewportSGPrivate::~ViewportSGPrivate()
{
    delete pickFbo;
}

void ViewportSGPrivate::setDefaults(QGLPainter *painter)
{
    // Set the default depth buffer options.
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    painter->glDepthRangef(0.0f, 1.0f);

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

ViewportSG::ViewportSG(QSGItem *parent)
    : QSGItem(parent)
{
    d = new ViewportSGPrivate();
    setFlag(ItemHasContents);

    connect(this, SIGNAL(viewportChanged()), this, SLOT(update3d()));

    setCamera(new QGLCamera(this));
    setLight(new QGLLightParameters(this));

    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}

ViewportSG::~ViewportSG()
{
    delete d;
}

bool ViewportSG::picking() const
{
    return d->picking;
}

void ViewportSG::setPicking(bool value)
{
    d->picking = value;
    emit viewportChanged();
}

bool ViewportSG::showPicking() const
{
    return d->showPicking;
}

void ViewportSG::setShowPicking(bool value)
{
    d->showPicking = value;
    emit viewportChanged();
}

bool ViewportSG::navigation() const
{
    return d->navigation;
}

void ViewportSG::setNavigation(bool value)
{
    d->navigation = value;
    emit viewportChanged();
}

bool ViewportSG::blending() const
{
    return d->blending;
}

void ViewportSG::setBlending(bool value)
{
    d->blending = value;
    emit viewportChanged();
}

QGLCamera *ViewportSG::camera() const
{
    return d->camera;
}

void ViewportSG::setCamera(QGLCamera *value)
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

QGLLightParameters *ViewportSG::light() const
{
    return d->light;
}

void ViewportSG::setLight(QGLLightParameters *value)
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

QGLLightModel *ViewportSG::lightModel() const
{
    return d->lightModel;
}

void ViewportSG::setLightModel(QGLLightModel *value)
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

bool ViewportSG::isDirty() const
{
    return d->dirty;
}

void ViewportSG::paint(QGLAbstractSurface *surface, QGL::Eye eye)
{
    d->needsPick = true;
    d->dirty = false;

    QGLPainter painter;
    if (!painter.begin(surface))
        return;

    // Initialize the objects in the scene if this is the first paint.
    if (!d->itemsInitialized)
        initializeGL(&painter);

    // If this Viewport is being double-rendered in stereo, then set the eye.
    painter.setEye(eye);

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

void ViewportSG::earlyDraw(QGLPainter *painter)
{
    // If we have a parent, then assume that the parent has painted
    // the background and overpaint over the top of it.  If we don't
    // have a parent, then clear to black.  If we are rendering into
    // an fbo, then we clear to transparent.
    glDepthMask(GL_TRUE);
    painter->glClearDepthf(1.0f);
    if (painter->currentSurface()->surfaceType() ==
                QGLAbstractSurface::FramebufferObject) {
        if (isViewportRoot())
            painter->setClearColor(Qt::black);
        else
            painter->setClearColor(Qt::transparent);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else if (isViewportRoot()) {
        painter->setClearColor(Qt::black);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
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

void ViewportSG::draw(QGLPainter *painter)
{
    // Set up the initial depth, blend, and other options.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    painter->glDepthRangef(0.0f, 1.0f);
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

void ViewportSG::initializeGL(QGLPainter *painter)
{
    // Initialize the Item3D objects attached to this scene.
    QObjectList list = QObject::children();
    foreach (QObject *child, list) {
        QDeclarativeItem3D *item = qobject_cast<QDeclarativeItem3D *>(child);
        if (item) {
            setItemViewport(item);
            item->initialize(painter);
        }
    }
    d->itemsInitialized = true;
}

int ViewportSG::registerPickableObject(QObject *obj)
{
    int id = (d->pickId)++;
    d->objects[id] = obj;
    return id;
}

QObject *ViewportSG::objectForPoint(qreal x, qreal y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    return 0;
#if 0   // TODO - picking support
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
    bool doubleBuffer;
    if (glw) {
        doubleBuffer = glw->doubleBuffer();
        if (!painter.begin(glw))
            return 0;
    } else {
        doubleBuffer = false;
        qpainter.begin(d->viewWidget);
        if (!painter.begin(&qpainter))
            return 0;
    }

    int objectId = -1;

    QSize size(qRound(width()), qRound(height()));
    QSize fbosize(QGL::nextPowerOfTwo(size));
    if (!d->needsPick && d->pickFbo && d->pickFbo->size() == fbosize) {
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
        } else {
            // Use the QGLWidget's back buffer for picking to avoid the
            // need to create a separate fbo in GPU memory.
            mainSurface = painter.currentSurface();
            height = mainSurface->viewportGL().height();
        }
        QGLSubsurface surface(mainSurface, QRect(QPoint(0, 0), size));
        painter.pushSurface(&surface);
        painter.setPicking(true);
        painter.clearPickObjects();
        painter.setClearColor(Qt::black);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        painter.setEye(QGL::NoEye);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        painter->glDepthRangef(0.0f, 1.0f);
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
        delete fboSurface;
    }

    d->needsPick = doubleBuffer;
    return d->objects.value(objectId, 0);
#endif
}

void ViewportSG::update3d()
{
    d->dirty = true;
    update();
}

void ViewportSG::cameraChanged()
{
    d->dirty = true;
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

void ViewportSG::mousePressEvent(QGraphicsSceneMouseEvent *e)
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

void ViewportSG::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
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

void ViewportSG::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
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

void ViewportSG::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
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

void ViewportSG::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QSGItem::hoverEnterEvent(e);
}

void ViewportSG::hoverMoveEvent(QGraphicsSceneHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QSGItem::hoverMoveEvent(e);
}

void ViewportSG::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    if (!d->pressedObject && d->enteredObject) {
        sendLeaveEvent(d->enteredObject);
        d->enteredObject = 0;
        e->setAccepted(true);
    } else {
        QSGItem::hoverLeaveEvent(e);
    }
}

void ViewportSG::wheelEvent(QGraphicsSceneWheelEvent *e)
{
    if (d->navigation) {
        wheel(e->delta());
        e->setAccepted(true);
    } else {
        QSGItem::wheelEvent(e);
    }
}

void ViewportSG::keyPressEvent(QKeyEvent *e)
{
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

bool ViewportSG::hoverEvent(QGraphicsSceneHoverEvent *e)
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
void ViewportSG::wheel(qreal delta)
{
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
}

// Pan left/right/up/down without rotating about the object.
void ViewportSG::pan(qreal deltax, qreal deltay)
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
void ViewportSG::rotate(qreal deltax, qreal deltay)
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
QPointF ViewportSG::viewDelta(qreal deltax, qreal deltay)
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

Node *ViewportSG::updatePaintNode(Node *node, UpdatePaintNodeData *)
{
    ViewportFboNodeSG *vnode = static_cast<ViewportFboNodeSG *>(node);
    if (!vnode)
        vnode = new ViewportFboNodeSG(this, QSGContext::current);

    vnode->setSize(QSize(qRound(width()), qRound(height())));
    vnode->setUseAlpha(!isViewportRoot());

    return vnode;
}

bool ViewportSG::isViewportRoot() const
{
    QSGView *view = qobject_cast<QSGView *>(canvas());
    if (view && view->rootObject() == this)
        return true;
    else
        return false;
}

QT_END_NAMESPACE
