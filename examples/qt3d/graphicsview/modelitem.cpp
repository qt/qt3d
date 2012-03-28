/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "modelitem.h"
#include "qglpainter.h"
#include "qgraphicsembedscene.h"
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qapplication.h>

const qreal CubeSize = 2.0f;

ModelItem::ModelItem(QGraphicsItem *parent)
    : QGLGraphicsViewportItem(parent)
    , mScene(0)
    , mTextureId(0)
    , navigating(false)
    , mPressedFace(-1)
    , pressedButton(Qt::NoButton)
{
    startNavCamera = new QGLCamera();

    setFlag(ItemIsFocusable, true);
}

ModelItem::~ModelItem()
{
    delete startNavCamera;
}

void ModelItem::setScene(QGraphicsEmbedScene *scene)
{
    mScene = scene;
    connect(scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(updateScene()));
}

void ModelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Render the inner scene into a framebuffer object.
    // We do this while the ordinary Qt paint engine has
    // control of the GL context rather than later when the
    // QGLPainter has control of the GL context.
    if (mScene)
        mTextureId = mScene->renderToTexture();

    // Now render the GL parts of the item using QGLPainter.
    QGLGraphicsViewportItem::paint(painter, option, widget);
}

void ModelItem::updateScene()
{
    update();
}

void ModelItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    int face;
    QPointF tc = intersection
        (event->widget(), event->pos().toPoint(), &face);
    if (!navigating && mPressedFace == -1 && face != -1) {
        mPressedFace = face;
        pressedButton = event->button();
        mScene->deliverEvent(event, tc);
        return;
    } else if (!navigating && face == -1) {
        navigating = true;
        pressedButton = event->button();
        pressedPos = event->pos().toPoint();
        startNavCamera->setEye(camera()->eye());
        startNavCamera->setCenter(camera()->center());
        startNavCamera->setUpVector(camera()->upVector());
#ifndef QT_NO_CURSOR
        setCursor(Qt::ClosedHandCursor);
#endif
        return;
    }
    QGraphicsItem::mousePressEvent(event);
}

void ModelItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (navigating) {
        QPoint delta = event->pos().toPoint() - pressedPos;
        int deltax = delta.x();
        int deltay = delta.y();
        QGLCamera *camera = this->camera();
        int rotation = camera->screenRotation();
        if (rotation == 90 || rotation == 270) {
            qSwap(deltax, deltay);
        }
        if (rotation == 90 || rotation == 180) {
            deltax = -deltax;
        }
        if (rotation == 180 || rotation == 270) {
            deltay = -deltay;
        }
        qreal anglex = deltax * 90.0f / rect().width();
        qreal angley = deltay * 90.0f / rect().height();
        QQuaternion q = startNavCamera->pan(-anglex);
        q *= startNavCamera->tilt(-angley);
        camera->setEye(startNavCamera->eye());
        camera->setCenter(startNavCamera->center());
        camera->setUpVector(startNavCamera->upVector());
        camera->rotateCenter(q);
    } else if (mPressedFace != -1) {
        int face;
        QPointF tc = intersection
            (event->widget(), event->pos().toPoint(), &face);
        if (face != mPressedFace)
            tc = QPointF(-1, -1);
        mScene->deliverEvent(event, tc);
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void ModelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (navigating && pressedButton == event->button()) {
        navigating = false;
        pressedButton = Qt::NoButton;
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        return;
    } else if (mPressedFace != -1) {
        int face;
        QPointF tc = intersection
            (event->widget(), event->pos().toPoint(), &face);
        if (face != mPressedFace)
            tc = QPoint(-1, -1);
        if (pressedButton == event->button()) {
            mPressedFace = -1;
            pressedButton = Qt::NoButton;
        }
        mScene->deliverEvent(event, tc);
        return;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void ModelItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    int face;
    QPointF tc = intersection
        (event->widget(), event->pos().toPoint(), &face);
    if (mPressedFace == -1 && face != -1) {
        mPressedFace = face;
        pressedButton = event->button();
        mScene->deliverEvent(event, tc);
        return;
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

QPointF ModelItem::intersection
    (QWidget *widget, const QPoint &point, int *actualFace) const
{
    // Bail out if no scene.
    if (!mScene) {
        *actualFace = -1;
        return QPointF();
    }

    // Get the combined matrix for the projection.
    int dpiX = widget->logicalDpiX();
    int dpiY = widget->logicalDpiY();
    QRectF bounds = boundingRect();
    qreal aspectRatio = (bounds.width() * dpiY) / (bounds.height() * dpiX);
    QMatrix4x4 proj = camera()->projectionMatrix(aspectRatio);

    // Find the relative position of the point within (-1, -1) to (1, 1).
    QPointF relativePoint =
        QPointF((point.x() - bounds.center().x()) * 2 / bounds.width(),
                -(point.y() - bounds.center().y()) * 2 / bounds.height());

    // Get the ray extending from the eye through the point the user selected.
    QVector3D eyept = proj.inverted().map
        (QVector3D(relativePoint.x(), relativePoint.y(), -1.0f));
    QRay3D ray(QVector3D(0, 0, 0), eyept);

    // Intersect with the ray using a model-specific intersection method.
    return intersection(ray, actualFace);
}
