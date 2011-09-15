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

#include "qgraphicsembedscene.h"
#include "qglnamespace.h"
#include <QtOpenGL/qglframebufferobject.h>
#include <QtGui/qapplication.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsEmbedScene
    \brief The QGraphicsEmbedScene class provides a method to render a QGraphicsScene into a texture.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview

    \section1 Rendering the scene into a texture

    Embedded scenes are rendered into textures with renderToTexture()
    before the main 3D scene painting with QGLPainter starts.  The following
    example shows the sequence of operations for a QGLWidget:

    \code
    void MyGLWidget::paintGL()
    {
        GLuint textureId = scene.renderToTexture();

        QGLPainter painter(this);
        painter.glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        ... // draw the 3D object
        painter.glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    \endcode

    When using QGLView, override the QGLWidget::paintGL() function
    explicitly to render scenes into textures before QGLView::paintGL()
    is called:

    \code
    void MyGLView::paintGL()
    {
        textureId = scene.renderToTexture();
        QGLView::paintGL();
    }

    void MyGLView::paintGL(QGLPainter *painter)
    {
        painter->glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        ... // draw the 3D object
        painter->glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    \endcode

    The texture is oriented so that (0, 0) corresponds to the bottom-left
    of the scene, and (1, 1) corresponds to the top-right of the scene.
    The texture may be scaled up to a power of two in size for better
    performance.

    \section1 Delivering mouse and keyboard events

    Mouse and keyboard events originate with the widget that is
    displaying the 3D scene.  The application needs to intercept
    these events and decide which 3D object in the scene should
    get the event.

    The application then performs an intersection between the 3D
    object and a line in 3D space (usually a QRay3D) to determine
    the texture co-ordinate of where the user clicked on the object.
    How the intersection is determined is application-specific; different
    algorithms are needed based on the type of object (cube, sphere,
    parameterized mesh, etc).

    Once the texture co-ordinate has been determined, the application
    passes the widget event and the texture co-ordinate to deliverEvent().
    QGraphicsEmbedScene will then route the event to the appropriate
    graphics item in the embedded scene.

    Keyboard, focus in/out, and other non-mouse events do not need a
    texture co-ordinate.  Pass (0, 0) as the texture co-ordinate for
    those event types.

    \section1 Updating the 3D scene

    The application will probably want to perform an update() whenever
    something changes in the embedded scene.  The application should
    listen for the QGraphicsScene::changed() and
    QGraphicsScene::sceneRectChanged() signals.

    \section1 Performance notes

    QGraphicsEmbedScene renders the scene into a framebuffer object,
    which will involve GL state switching.  The scene will be
    effectively triple-buffered: render into the fbo, then render
    the fbo into the GL window's back buffer, and finally copy to the
    GL window's front buffer.

    If the scene is showing a video item or something else that is
    highly animated, then it may be better to render the source into a
    texture some other way.

    If the embedded scene is itself a 3D scene drawn with GL, then use
    QGLPainter::pushSurface() and a QGLFramebufferObjectSurface instead
    of QGraphicsEmbedScene.

    \section1 Limitations

    When rendered into a texture, the scene does not have a QGraphicsView
    or a viewport QWidget associated with it.  Graphics items that require a
    widget rendering surface will not work correctly.
*/

class QGraphicsEmbedScenePrivate : public QObject
{
    Q_OBJECT
public:
    QGraphicsEmbedScenePrivate(QGraphicsScene *scene, QObject *parent = 0)
        : QObject(parent)
        , dirty(true)
        , fbo(0)
    {
        format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);

        connect(scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(update()));
        connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(update()));
    }
    ~QGraphicsEmbedScenePrivate()
    {
        delete fbo;
    }

    bool dirty;
    QGLFramebufferObject *fbo;
    QGLFramebufferObjectFormat format;
    QPoint pressedPos;

public Q_SLOTS:
    void update() { dirty = true; }
};

/*!
    Constructs an embedded graphics scene and attaches it to \a parent.
*/
QGraphicsEmbedScene::QGraphicsEmbedScene(QObject *parent)
    : QGraphicsScene(parent)
    , d_ptr(new QGraphicsEmbedScenePrivate(this))
{
}

/*!
    Constructs an embedded graphics scene, using \a sceneRect for its
    scene rectangle, and attaches it to \a parent.
*/
QGraphicsEmbedScene::QGraphicsEmbedScene(const QRectF &sceneRect, QObject *parent)
    : QGraphicsScene(sceneRect, parent)
    , d_ptr(new QGraphicsEmbedScenePrivate(this))
{
}

/*!
    Constructs an embedded graphics scene, using (\a x, \a y, \a width,
    \a height) for its scene rectangle, and attaches it to \a parent.
*/
QGraphicsEmbedScene::QGraphicsEmbedScene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
    : QGraphicsScene(x, y, width, height, parent)
    , d_ptr(new QGraphicsEmbedScenePrivate(this))
{
}

/*!
    Destroys this embedded graphics scene.
*/
QGraphicsEmbedScene::~QGraphicsEmbedScene()
{
}

/*!
    Returns the framebuffer object format to use when renderToTexture()
    is called.  The default is a framebuffer format with depth and
    stencil buffers, RGBA format, and no multisampling.

    \sa setFormat()
*/
QGLFramebufferObjectFormat QGraphicsEmbedScene::format() const
{
    Q_D(const QGraphicsEmbedScene);
    return d->format;
}

/*!
    Sets the framebuffer object \a format to use when renderToTexture()
    is called.

    \sa format()
*/
void QGraphicsEmbedScene::setFormat(const QGLFramebufferObjectFormat &format)
{
    Q_D(QGraphicsEmbedScene);
    d->format = format;
}

/*!
    Renders this scene to a GL texture and returns the texture identifier.
    If the scene has not changed since the last call, then the same
    texture identifier will be returned without repainting the scene.

    The \a levelOfDetail is a hint that allows some control over the
    texture size.  The default value of 1 indicates that the texture size
    should be the same or similar to the size of the scene.  A value of 2
    would indicate that the texture size should have twice the width and
    height.  A value of 0.5 would indicate half the size.

    The primary use for \a levelOfDetail is to render the scene with a
    greater level of detail when the 3D object showing the texture is
    larger on-screen than the scene's natural size.  The parameter can
    also be used to reduce the level of detail in the texture when the
    3D object is very small.

    It is assumed that a QGLContext is bound when this function is
    called.  Usually this is the QGLContext that is bound by a QPainter
    for 2D painting into a GL surface.  Note however that a QGLPainter must
    not be active on the QGLContext when this function is called.

    \sa format()
*/
GLuint QGraphicsEmbedScene::renderToTexture(qreal levelOfDetail)
{
    Q_D(QGraphicsEmbedScene);

    // Determine the fbo size we will need.
    QSize size = (sceneRect().size() * levelOfDetail).toSize();
    QSize fboSize = QGL::nextPowerOfTwo(size);
    if (fboSize.isEmpty())
        fboSize = QSize(16, 16);

    // Create or re-create the fbo.
    if (!d->fbo || d->fbo->size() != fboSize) {
        delete d->fbo;
        d->fbo = new QGLFramebufferObject(fboSize, d->format);
        if (!d->fbo->isValid()) {
            delete d->fbo;
            d->fbo = 0;
            return 0;
        }
        d->dirty = true;
    }

    // Return the previous texture contents if the scene hasn't changed.
    if (!d->dirty && d->fbo)
        return d->fbo->texture();

    // Render the scene into the fbo, scaling the QPainter's view
    // transform up to the power-of-two fbo size.
    QPainter painter(d->fbo);
    painter.setWindow(0, 0, size.width(), size.height());
    painter.setViewport(0, 0, fboSize.width(), fboSize.height());
    render(&painter);
    painter.end();
    d->dirty = false;
    return d->fbo->texture();
}

/*!
    Delivers \a event to this scene.  If \a event is a mouse event, then
    \a texCoord indicates the texture co-ordinate on the side of the
    3D object where the user clicked.  This \a texCoord is used to determine
    the actual scene co-ordinate to deliver: (0, 0) corresponds to the
    bottom-left corner of the scene and (1, 1) corresponds to the
    top-right corner of the scene.

    The \a event normally originates from a QWidget or QGraphicsItem that
    contains the 3D object.  The caller performs a ray intersection in
    3D space on the position within \a event to determine the \a texCoord
    and then passes \a event on to deliverEvent() for further processing.
*/
void QGraphicsEmbedScene::deliverEvent(QEvent *event, const QPointF &texCoord)
{
    Q_D(QGraphicsEmbedScene);

    // Map the texture co-ordinate into "screen" co-ordinates.
    // Mouse move and release events can extend beyond the boundaries
    // of the scene, for "click and drag off-screen" operations.
    // Mouse press and double-click events need to be constrained.
    QRectF bounds = sceneRect();
    int screenX = qRound(texCoord.x() * bounds.width());
    int screenY = qRound((1.0f - texCoord.y()) * bounds.height());
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
        if (screenX < 0)
            screenX = 0;
        else if (screenX >= bounds.width())
            screenX = qRound(bounds.width() - 1);
        if (screenY < 0)
            screenY = 0;
        else if (screenY >= bounds.height())
            screenY = qRound(bounds.height() - 1);
        d->pressedPos = QPoint(screenX, screenY);
        break;
    default: break;
    }

    // Convert the event and deliver it to the scene.
    switch (event->type()) {
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick: {
        QGraphicsSceneMouseEvent *ev =
            static_cast<QGraphicsSceneMouseEvent *>(event);
        QGraphicsSceneMouseEvent e(ev->type());
        e.setPos(QPointF(screenX, screenY));
        e.setScenePos(QPointF(screenX + bounds.x(), screenY + bounds.y()));
        e.setScreenPos(QPoint(screenX, screenY));
        e.setButtonDownScreenPos(ev->button(), d->pressedPos);
        e.setButtonDownScenePos
            (ev->button(), QPointF(d->pressedPos.x() + bounds.x(),
                                   d->pressedPos.y() + bounds.y()));
        e.setButtons(ev->buttons());
        e.setButton(ev->button());
        e.setModifiers(ev->modifiers());
        e.setAccepted(false);
        QApplication::sendEvent(this, &e);
    }
    break;

#ifndef QT_NO_WHEELEVENT
    case QEvent::GraphicsSceneWheel: {
        QGraphicsSceneWheelEvent *ev =
            static_cast<QGraphicsSceneWheelEvent *>(event);
        QGraphicsSceneWheelEvent e(QEvent::GraphicsSceneWheel);
        e.setPos(QPointF(screenX, screenY));
        e.setScenePos(QPointF(screenX + bounds.x(), screenY + bounds.y()));
        e.setScreenPos(QPoint(screenX, screenY));
        e.setButtons(ev->buttons());
        e.setModifiers(ev->modifiers());
        e.setDelta(ev->delta());
        e.setOrientation(ev->orientation());
        e.setAccepted(false);
        QApplication::sendEvent(this, &e);
    }
    break;
#endif

    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove: {
        QMouseEvent *ev = static_cast<QMouseEvent *>(event);
        QEvent::Type type;
        if (ev->type() == QEvent::MouseButtonPress)
            type = QEvent::GraphicsSceneMousePress;
        else if (ev->type() == QEvent::MouseButtonRelease)
            type = QEvent::GraphicsSceneMouseRelease;
        else if (ev->type() == QEvent::MouseButtonDblClick)
            type = QEvent::GraphicsSceneMouseDoubleClick;
        else
            type = QEvent::GraphicsSceneMouseMove;
        QGraphicsSceneMouseEvent e(type);
        e.setPos(QPointF(screenX, screenY));
        e.setScenePos(QPointF(screenX + bounds.x(), screenY + bounds.y()));
        e.setScreenPos(QPoint(screenX, screenY));
        e.setButtonDownScreenPos(ev->button(), d->pressedPos);
        e.setButtonDownScenePos
            (ev->button(), QPointF(d->pressedPos.x() + bounds.x(),
                                   d->pressedPos.y() + bounds.y()));
        e.setButtons(ev->buttons());
        e.setButton(ev->button());
        e.setModifiers(ev->modifiers());
        e.setAccepted(false);
        QApplication::sendEvent(this, &e);
    }
    break;

#ifndef QT_NO_WHEELEVENT
    case QEvent::Wheel: {
        QWheelEvent *ev = static_cast<QWheelEvent *>(event);
        QGraphicsSceneWheelEvent e(QEvent::GraphicsSceneWheel);
        e.setPos(QPointF(screenX, screenY));
        e.setScenePos(QPointF(screenX + bounds.x(), screenY + bounds.y()));
        e.setScreenPos(QPoint(screenX, screenY));
        e.setButtons(ev->buttons());
        e.setModifiers(ev->modifiers());
        e.setDelta(ev->delta());
        e.setOrientation(ev->orientation());
        e.setAccepted(false);
        QApplication::sendEvent(this, &e);
    }
    break;
#endif

    default: {
        // Send the event directly without any conversion.
        // Typically used for keyboard, focus, and enter/leave events.
        QApplication::sendEvent(this, event);
    }
    break;

    }
}

/*!
    \internal
*/
void QGraphicsEmbedScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if (backgroundBrush().style() == Qt::NoBrush) {
        // Fill the fbo with the transparent color as there won't
        // be a window or graphics item drawing a previous background.
        painter->save();
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->fillRect(rect, Qt::transparent);
        painter->restore();
    } else {
        QGraphicsScene::drawBackground(painter, rect);
    }
}

QT_END_NAMESPACE

#include "qgraphicsembedscene.moc"
