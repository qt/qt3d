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

#include "qglgraphicsviewportitem.h"
#include "qglpainter.h"
#include "qglsubsurface.h"
#include <QtGui/qpainter.h>
#include <QtGui/qgraphicsscene.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLGraphicsViewportItem
    \brief The QGLGraphicsViewportItem class provides a rectangular viewport for arbitrary OpenGL painting.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview
*/

class QGLGraphicsViewportItemPrivate : public QObject
{
    Q_OBJECT
public:
    QGLGraphicsViewportItemPrivate(QGLGraphicsViewportItem *item)
        : q(item)
    {
        defaultCamera = camera = new QGLCamera(this);
        connect(camera, SIGNAL(projectionChanged()),
                this, SLOT(cameraChanged()));
        connect(camera, SIGNAL(viewChanged()),
                this, SLOT(cameraChanged()));
    }

    void changeCamera(QGLCamera *c);
    void setDefaults(QGLPainter *painter);

    QGLGraphicsViewportItem *q;
    QRectF rect;
    QGLCamera *camera;
    QGLCamera *defaultCamera;
    QColor backgroundColor;

private Q_SLOTS:
    void cameraChanged();
};

void QGLGraphicsViewportItemPrivate::changeCamera(QGLCamera *c)
{
    disconnect(camera, SIGNAL(projectionChanged()),
               this, SLOT(cameraChanged()));
    disconnect(camera, SIGNAL(viewChanged()),
               this, SLOT(cameraChanged()));
    camera = c;
    connect(camera, SIGNAL(projectionChanged()),
            this, SLOT(cameraChanged()));
    connect(camera, SIGNAL(viewChanged()),
            this, SLOT(cameraChanged()));
}

void QGLGraphicsViewportItemPrivate::cameraChanged()
{
    q->update();
}

void QGLGraphicsViewportItemPrivate::setDefaults(QGLPainter *painter)
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
    Constructs a new GL graphics viewport and attaches it to \a parent.
*/
QGLGraphicsViewportItem::QGLGraphicsViewportItem(QGraphicsItem *parent)
    : QGraphicsItem(parent), d_ptr(new QGLGraphicsViewportItemPrivate(this))
{
}

/*!
    Constructs a new GL graphics viewport and attaches it to \a parent.
    The boundingRect() is initially set to \a rect.
*/
QGLGraphicsViewportItem::QGLGraphicsViewportItem(const QRectF& rect, QGraphicsItem *parent)
    : QGraphicsItem(parent), d_ptr(new QGLGraphicsViewportItemPrivate(this))
{
    d_ptr->rect = rect;
}

/*!
    Constructs a new GL graphics viewport and attaches it to \a parent.
    The boundingRect() is initially set to (\a x, \a y, \a w, \a h).
*/
QGLGraphicsViewportItem::QGLGraphicsViewportItem
        (qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsItem(parent), d_ptr(new QGLGraphicsViewportItemPrivate(this))
{
    d_ptr->rect = QRectF(x, y, w, h);
}

/*!
    Destroys this GL graphics viewport.
*/
QGLGraphicsViewportItem::~QGLGraphicsViewportItem()
{
}

/*!
    Returns the rectangular area of the view that is occupied by
    this GL graphics viewport.

    \sa setRect(), boundingRect()
*/
QRectF QGLGraphicsViewportItem::rect() const
{
    Q_D(const QGLGraphicsViewportItem);
    return d->rect;
}

/*!
    Sets the rectangular area of the view that is occupied by
    this GL graphics viewport to \a rect.

    \sa rect(), boundingRect()
*/
void QGLGraphicsViewportItem::setRect(const QRectF &rect)
{
    Q_D(QGLGraphicsViewportItem);
    if (d->rect == rect)
        return;
    prepareGeometryChange();
    d->rect = rect;
    update();
}

/*!
    \fn void QGLGraphicsViewportItem::setRect(qreal x, qreal y, qreal w, qreal h)
    \overload

    Sets the rectangular area of the view that is occupied by
    this GL graphics viewport to (\a x, \a y, \a w, \a h).

    \sa rect(), boundingRect()
*/

/*!
    Returns the bounding rectangle for this GL graphics viewport,
    which is the same as rect().

    \sa rect()
*/
QRectF QGLGraphicsViewportItem::boundingRect() const
{
    Q_D(const QGLGraphicsViewportItem);
    return d->rect;
}

/*!
    Returns the camera parameters.  The camera defines the projection
    to apply to convert eye co-ordinates into window co-ordinates,
    and the position and orientation of the viewer's eye.

    This item will be updated whenever the camera's parameters change.

    \sa setCamera()
*/
QGLCamera *QGLGraphicsViewportItem::camera() const
{
    Q_D(const QGLGraphicsViewportItem);
    return d->camera;
}

/*!
    Sets the camera parameters to \a camera.  The camera defines the
    projection to apply to convert eye co-ordinates into window
    co-ordinates, and the position and orientation of the viewer's eye.

    If \a camera is null, then the default camera object will be used.

    This function will call update() to force the viewport to update
    with the new camera parameters upon the next event loop.  Afterwards,
    this item will be updated whenever the camera's parameters change.

    \sa camera()
*/
void QGLGraphicsViewportItem::setCamera(QGLCamera *camera)
{
    Q_D(QGLGraphicsViewportItem);
    if (!camera)
        camera = d->defaultCamera;
    if (d->camera != camera) {
        d->changeCamera(camera);
        update();
    }
}

/*!
    Returns the background color, which is used to clear the viewport
    before calling paintGL().  The default value is an invalid QColor,
    which indicates that the viewport should not be cleared.

    \sa setBackgroundColor()
*/
QColor QGLGraphicsViewportItem::backgroundColor() const
{
    Q_D(const QGLGraphicsViewportItem);
    return d->backgroundColor;
}

/*!
    Sets the background \a color, which is used to clear the viewport
    before calling paintGL().

    \sa backgroundColor()
*/
void QGLGraphicsViewportItem::setBackgroundColor(const QColor& color)
{
    Q_D(QGLGraphicsViewportItem);
    d->backgroundColor = color;
}

/*!
    Paints this GL graphics viewport onto \a painter, with the specified
    \a option and \a widget parameters.

    This override creates a QGLPainter for \a painter and passes
    it to paintGL().

    \sa paintGL()
*/
void QGLGraphicsViewportItem::paint
    (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_D(QGLGraphicsViewportItem);

    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (d->rect.isEmpty())
        return;

    // Initialize a QGLPainter for the surface and bail out if not active.
    QGLPainter glpainter;
    if (!glpainter.begin(painter)) {
        qWarning("GL graphics system is not active; cannot use 3D items");
        return;
    }

    // Set up the GL viewport to limit drawing to the bounds of this item.
    QRect viewport = painter->deviceTransform().mapRect(rect()).toRect();
    QGLSubsurface surface(glpainter.currentSurface(), viewport);
    glpainter.pushSurface(&surface);

    // Set up the desired drawing options.
    glDisable(GL_CULL_FACE);
    d->setDefaults(&glpainter);
    if (d->backgroundColor.isValid()) {
        // We clear the background by drawing a triangle fan so
        // that the background color will blend with the underlying
        // screen content if it has an alpha component.
        glDisable(GL_DEPTH_TEST);
        if (d->backgroundColor.alpha() != 255)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
        QVector2DArray array;
        array.append(-1, -1);
        array.append(1, -1);
        array.append(1, 1);
        array.append(-1, 1);
        glpainter.projectionMatrix().setToIdentity();
        glpainter.modelViewMatrix().setToIdentity();
        glpainter.setStandardEffect(QGL::FlatColor);
        glpainter.setColor(d->backgroundColor);
        glpainter.setVertexAttribute(QGL::Position, array);
        glpainter.draw(QGL::TriangleFan, 4);
    }
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // Apply the camera.
    glpainter.setEye(QGL::NoEye);
    glpainter.setCamera(d->camera);

    // Paint the GL contents.
    paintGL(&glpainter);

    // Disable the current drawing effect so that QGLPainter will
    // forcibly update the GL context the next time QGLPainter is used.
    glpainter.disableEffect();

    // Try to restore the GL state to something paint-engine compatible.
    glDisable(GL_CULL_FACE);
    d->setDefaults(&glpainter);
    glDisable(GL_DEPTH_TEST);

    glpainter.popSurface();
}

/*!
    \fn void QGLGraphicsViewportItem::paintGL(QGLPainter *painter)

    Paints this GL graphics viewport onto \a painter.  This function is called
    from paint().

    \sa paint()
*/

QT_END_NAMESPACE

#include "qglgraphicsviewportitem.moc"
