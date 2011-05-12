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

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtDeclarative/qdeclarativeitem.h>

#include "qdeclarativeitem3d.h"
#include "qdeclarativeviewport.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class ViewportPrivate;
class QGLCamera;
class QGLLightModel;
class QGLLightParameters;
class QDeclarativeEffect;

class Viewport : public QDeclarativeItem, public QDeclarativeViewport
{
    Q_OBJECT
    Q_PROPERTY(bool picking READ picking WRITE setPicking NOTIFY viewportChanged)
    Q_PROPERTY(bool showPicking READ showPicking WRITE setShowPicking NOTIFY viewportChanged)   
    Q_PROPERTY(bool navigation READ navigation WRITE setNavigation NOTIFY viewportChanged)
    Q_PROPERTY(bool fovzoom READ fovzoom WRITE setFovzoom NOTIFY viewportChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY viewportChanged)
    Q_PROPERTY(QGLCamera *camera READ camera WRITE setCamera)
    Q_PROPERTY(QGLLightParameters *light READ light WRITE setLight NOTIFY viewportChanged)
    Q_PROPERTY(QGLLightModel *lightModel READ lightModel WRITE setLightModel NOTIFY viewportChanged)
public:
    Viewport(QDeclarativeItem *parent = 0);
    ~Viewport();

    bool picking() const;
    void setPicking(bool value);

    bool showPicking() const;
    void setShowPicking(bool value);

    bool navigation() const;
    void setNavigation(bool value);

    bool fovzoom() const;
    void setFovzoom(bool value);

    bool blending() const;
    void setBlending(bool value);

    QGLCamera *camera() const;
    void setCamera(QGLCamera *value);

    QGLLightParameters *light() const;
    void setLight(QGLLightParameters *value);

    QGLLightModel *lightModel() const;
    void setLightModel(QGLLightModel *value);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    int registerPickableObject(QObject *obj);

    Q_INVOKABLE QObject *objectForPoint(qreal x, qreal y);

Q_SIGNALS:
    void viewportChanged();

public Q_SLOTS:
    void update3d();

private Q_SLOTS:
    void cameraChanged();
    void switchToOpenGL();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    ViewportPrivate *d;

    void earlyDraw(QGLPainter *painter);
    void draw(QGLPainter *painter);
    void initializeGL(QGLPainter *painter);

    bool hoverEvent(QGraphicsSceneHoverEvent *event);

    QObject *objectForPoint(const QPointF &pos)
    {
        return objectForPoint(pos.x(), pos.y());
    }

    void wheel(qreal delta);
    void pan(qreal deltax, qreal deltay);
    void rotate(qreal deltax, qreal deltay);
    QPointF viewDelta(qreal deltax, qreal deltay);
};

QML_DECLARE_TYPE(Viewport)

QT_END_NAMESPACE

QT_END_HEADER

#endif
