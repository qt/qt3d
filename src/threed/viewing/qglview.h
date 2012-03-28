/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifndef QGLVIEW_H
#define QGLVIEW_H

#include "qglpainter.h"
#include "qglcamera.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLViewPrivate;

class Q_QT3D_EXPORT QGLView : public QGLWidget
{
    Q_OBJECT
public:
    explicit QGLView(QWidget *parent=0);
    explicit QGLView(const QGLFormat& format, QWidget *parent=0);
    ~QGLView();

    enum Option
    {
        ObjectPicking       = 0x0001,
        ShowPicking         = 0x0002,
        CameraNavigation    = 0x0004,
        PaintingLog         = 0x0008,
        FOVZoom             = 0x0010
    };
    Q_DECLARE_FLAGS(Options, Option)

    QGLView::Options options() const;
    void setOptions(QGLView::Options value);
    void setOption(QGLView::Option option, bool value);

    enum StereoType
    {
        Hardware,
        RedCyanAnaglyph,
        LeftRight,
        RightLeft,
        TopBottom,
        BottomTop,
        StretchedLeftRight,
        StretchedRightLeft,
        StretchedTopBottom,
        StretchedBottomTop
    };

    QGLView::StereoType stereoType() const;
    void setStereoType(QGLView::StereoType type);

    void registerObject(int objectId, QObject *object);
    void deregisterObject(int objectId);
    QObject *objectForPoint(const QPoint &point);

    QGLCamera *camera() const;
    void setCamera(QGLCamera *camera);

    QVector3D mapPoint(const QPoint &point) const;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    virtual void initializeGL(QGLPainter *painter);
    virtual void earlyPaintGL(QGLPainter *painter);
    virtual void paintGL(QGLPainter *painter) = 0;

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *e);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *e);
#endif
    void keyPressEvent(QKeyEvent *e);

    QPointF viewDelta(int deltax, int deltay) const;
    QPointF viewDelta(const QPoint &delta) const
        { return viewDelta(delta.x(), delta.y()); }

private Q_SLOTS:
    void cameraChanged();

private:
    QGLViewPrivate *d;

    static void sendEnterEvent(QObject *object);
    static void sendLeaveEvent(QObject *object);

    void wheel(int delta);
    void pan(int deltax, int deltay);
    void rotate(int deltax, int deltay);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLView::Options)

QT_END_NAMESPACE

QT_END_HEADER

#endif
