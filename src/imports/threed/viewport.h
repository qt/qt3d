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

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtQuick/qquickpainteditem.h>

#include "qquickitem3d.h"
#include "qquickviewport.h"

QT_BEGIN_NAMESPACE

class ViewportPrivate;
class QGLCamera;
class QGLLightModel;
class QGLLightParameters;
class QQuickEffect;
class PickEvent;

class Viewport : public QQuickPaintedItem, public QQuickViewport
{
    Q_OBJECT
    Q_ENUMS(RenderMode)
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY viewportChanged)
    Q_PROPERTY(RenderMode renderMode READ renderMode WRITE setRenderMode NOTIFY viewportChanged)
    Q_PROPERTY(bool picking READ picking WRITE setPicking NOTIFY viewportChanged)
    Q_PROPERTY(bool showPicking READ showPicking WRITE setShowPicking NOTIFY viewportChanged)
    Q_PROPERTY(bool showSceneGraph READ showSceneGraph WRITE setShowSceneGraph NOTIFY showSceneGraphChanged)
    Q_PROPERTY(bool navigation READ navigation WRITE setNavigation NOTIFY viewportChanged)
    Q_PROPERTY(bool fovzoom READ fovzoom WRITE setFovzoom NOTIFY viewportChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY viewportChanged)
    Q_PROPERTY(QGLCamera *camera READ camera WRITE setCamera)
    Q_PROPERTY(QGLLightParameters *light READ light WRITE setLight NOTIFY viewportChanged)
    Q_PROPERTY(QGLLightModel *lightModel READ lightModel WRITE setLightModel NOTIFY viewportChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)

public:
    enum RenderMode
    {
        UnknownRender,
        DirectRender,
        BufferedRender
    };

    Viewport(QQuickItem *parent = 0);
    ~Viewport();

    QColor fillColor() const;
    void setFillColor(const QColor &);

    bool picking() const;
    void setPicking(bool value);

    bool showPicking() const;
    void setShowPicking(bool value);

    bool showSceneGraph() const;
    void setShowSceneGraph(bool show);

    bool navigation() const;
    void setNavigation(bool value);

    bool fovzoom() const;
    void setFovzoom(bool value);

    bool blending() const;
    void setBlending(bool value);

    bool antialiasing() const;
    void setAntialiasing(bool value);

    QGLCamera *camera() const;
    void setCamera(QGLCamera *value);

    QGLLightParameters *light() const;
    void setLight(QGLLightParameters *value);

    QGLLightModel *lightModel() const;
    void setLightModel(QGLLightModel *value);

    int registerPickableObject(QObject *obj);
    virtual void registerEarlyDrawObject(QObject *obj, int order);

    void paint(QPainter *painter);

    RenderMode renderMode() const;
    void setRenderMode(RenderMode mode);

Q_SIGNALS:
    void viewportChanged();
    void showSceneGraphChanged();
    void antialiasingChanged();

public Q_SLOTS:
    void update3d();

private Q_SLOTS:
    void cameraChanged();
    void beforeRendering();
    void sceneGraphInitialized();
    void objectForPoint();
    void canvasDeleted();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void hoverEnterEvent(QHoverEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

    // from QQuickItem
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void itemChange(QQuickItem::ItemChange change, const ItemChangeData &value);

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    void render(QGLPainter *painter);
    PickEvent *initiatePick(QMouseEvent *);
    void setupPickPaint(QGLPainter *painter, const QPointF &pt);
    bool mouseMoveOverflow(QMouseEvent *e) const;

    Q_INVOKABLE void processMousePress(PickEvent *event);
    Q_INVOKABLE void processMouseRelease(PickEvent *event);
    Q_INVOKABLE void processMouseDoubleClick(PickEvent *event);
    Q_INVOKABLE void processMouseMove(PickEvent *event);
    Q_INVOKABLE void processMouseHover(PickEvent *event);

    void processNavEvent(QMouseEvent *event);

    ViewportPrivate *d;

    void earlyDraw(QGLPainter *painter);
    void draw(QGLPainter *painter);
    void initializeGL(QGLPainter *painter);

    bool hoverEvent(QHoverEvent *event);

    void wheel(float delta);
    void pan(float deltax, float deltay);
    void rotate(float deltax, float deltay);
    QPointF viewDelta(float deltax, float deltay);

    static const int FBO_SIZE;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(Viewport)

#endif
