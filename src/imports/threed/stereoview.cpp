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

#include "stereoview.h"
#include "stereovieweffect.h"
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtGui/qgraphicsview.h>
#include <QtCore/qtimer.h>
#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass StereoView StereoView
    \brief The StereoView item defines the layout of a view with left and right stereo eye images.
    \since 4.8
    \ingroup qt3d::qml3d

    StereoView provides the logic to double-render the entire
    application, firstly from the perspective of the left eye,
    and then secondly from the perspective of the right eye.
    By default, if the hardware does not support stereo, the
    view will be drawn using red/cyan anaglyph filters as
    shown in the following example:

    \snippet declarative/stereo/stereo.qml 1
    \dots
    \snippet declarative/stereo/stereo.qml 2

    \image stereo-screenshot-qml.png

    We could change this to double-wide stereo with the
    \l layout property:

    \snippet declarative/stereo/stereo.qml 3
    \snippet declarative/stereo/stereo.qml 4
    \dots
    \snippet declarative/stereo/stereo.qml 2

    \image stereo-screenshot-qml2.png

    2D items are placed within the 3D visual field using the
    FloatingItem element and the \l{FloatingItem::depth}{depth}
    property:

    \snippet declarative/stereo/stereo.qml 5

    Negative depth values define distances behind the
    \i{center of focus} of our scene, and positive depth
    values define distances in front of the center of focus.
    The center of focus corresponds to where the left and right
    eye images coincide at depth 0.  Usually the most important
    item on the screen is placed at depth 0 so that the user's
    attention is immediately drawn to it.  Other items then
    \i{float above} (positive depth) or \i{appear behind}
    (negative depth) the center of focus.

    As a general rule, children of the StereoView element
    should appear in \i{bottom up order}, from the most negative
    depth to the most positive.  This will ensure that the children
    are painted correctly at their respective depths.

    \bold{Note:} StereoView is not supported with Qt Quick 2.0 as the
    standard view classes are already stereo-aware.

    \sa FloatingItem, Viewport, {QML Stereo Viewing Example}
*/

/*!
    \internal
*/
StereoView::StereoView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_layout(StereoView::Default)
    , m_eye(QGL::NoEye)
{
    // We need paint() to be called to switch to an OpenGL viewport.
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    // Create a viewport item that defines the width and height
    // of a single eye image.  Our item children are parented here.
    m_viewport = new QDeclarativeItem(this);

    // Create a QGraphicsEffect for performing the double-render.
    m_effect = new StereoViewEffect(this);
    setGraphicsEffect(m_effect);
}

/*!
    \internal
*/
StereoView::~StereoView()
{
}

/*!
    \qmlproperty enumeration StereoView::layout

    This property defines the layout of the left and right eye
    images within this stereo view, and the relative size of
    the \l viewport.

    \list
    \o \c Default - the stereo layout is determined by the hardware.
       If the hardware is incapable of stereo, \c RedCyan will
       be used instead.  The viewport size is determined by the
       hardware type.  This is the default setting.
    \o \c Hardware - the stereo layout is determined by the
       hardware.  If the hardware is incapable of stereo, then
       stereo rendering will be disabled rather than use \c RedCyan.
       The viewport size is determined by the hardware type.
    \o \c RedCyan - render the left eye image with a red filter and the
       right eye image with a cyan filter, which makes the result
       suitable for viewing with red/cyan anaglyph glasses.
       The viewport occupies the entire StereoView.
    \o \c LeftRight - the left and right eye images are rendered into
       the left and right halves of the StereoView, with the
       viewport occupying half the width and the full height.
    \o \c RightLeft - the left and right eye images are rendered into
       the right and left halves of the StereoView, with the
       viewport occupying half the width and the full height.
    \o \c TopBottom - the left and right eye images are rendered into
       the top and bottom halves of the StereoView, with the
       viewport occupying the full width and half the height.
    \o \c BottomTop - the left and right eye images are rendered into
       the bottom and top halves of the StereoView, with the
       viewport occupying the full width and half the height.
    \o \c StretchedLeftRight - same as LeftRight, but the two eye
       halves are stretched to twice their original width.
    \o \c StretchedRightLeft - same as RightLeft, but the two eye
       halves are stretched to twice their original width.
    \o \c StretchedTopBottom - same as TopBottom, but the two eye
       halves are stretched to twice their original height.
    \o \c StretchedBottomTop - same as BottomTop, but the two eye
       halves are stretched to twice their original height.
    \o \c Disabled - stereo rendering is disabled, depth values on
       FloatingItem elements will be ignored, and the viewport
       occupies the entire StereoView.
    \endlist
*/

void StereoView::setLayout(StereoView::Layout layout)
{
    if (m_layout != layout) {
        m_layout = layout;
        updateViewportSize();
        emit layoutChanged();
        update();
    }
}

/*!
    \internal
*/
QDeclarativeListProperty<QObject> StereoView::stereoViewData()
{
    // Return the "data" property for the viewport, which is
    // where we add all of the item children.
    return m_viewport->property("data").value< QDeclarativeListProperty<QObject> >();
}

/*!
    \qmlproperty Item StereoView::viewport

    This property defines the extent and position of the eye
    image viewport.  All child items are parented under the viewport,
    and should be positioned within it.  In the following example,
    the viewport will be the left half of the extent of the StereoView;
    i.e. (0, 0, 360, 640).

    \code
    StereoView {
        layout: StereoView.LeftRight
        width: 720; height: 640
        Rectangle {
            anchors.fill: viewport
            ...
        }
    }
    \endcode

    The viewport property also appears as the \c parent property
    of the children, so the following is equivalent:

    \code
    StereoView {
        layout: StereoView.LeftRight
        width: 720; height: 640
        Rectangle {
            anchors.fill: parent
            ...
        }
    }
    \endcode
*/

/*!
    \internal
    Finds the StereoView surrounding a specific \a item, usually
    an instance of FloatingItem.
*/
StereoView *StereoView::findView(QDeclarativeItem *item)
{
    while (item != 0) {
        StereoView *view = qobject_cast<StereoView *>(item);
        if (view)
            return view;
        item = item->parentItem();
    }
    return 0;
}

/*!
    \internal
*/
void StereoView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *widget)
{
    if (m_layout != RedCyan && m_layout != Default)
        return;

    // Using a red-cyan effect, for which we must have OpenGL rendering.
    // Or using the default layout, which is either hardware stereo
    // or red-cyan.  We need an OpenGL viewport for both.
    QPaintEngine *engine = painter->paintEngine();
    if (engine->type() != QPaintEngine::OpenGL &&
            engine->type() != QPaintEngine::OpenGL2) {
        if (widget) {
            // Switch the surrounding QGraphicsView to use a QGLWidget as its
            // viewport.  We cannot do it here during painting, so schedule a
            // slot to switch it the next time we reach the event loop.
            QGraphicsView *view =
                qobject_cast<QGraphicsView *>(widget->parentWidget());
            if (view) {
                QTimer::singleShot(0, this, SLOT(switchToOpenGL()));
                return;
            }
        }
    }
}

/*!
    \internal
*/
void StereoView::switchToOpenGL()
{
    // If there are multiple StereoView's in the QML, then it is
    // possible that another StereoView has already switched to QGLWidget.
    QList<QGraphicsView *> views = scene()->views();
    if (!views.isEmpty()) {
        QGLWidget *glw = qobject_cast<QGLWidget *>(views[0]->viewport());
        if (glw)
            return;
        bool focused = views[0]->viewport()->hasFocus();
        QGLFormat format = QGLFormat::defaultFormat();
        if (m_layout == Default)
            format.setStereo(true); // Request hardware stereo if available.
        views[0]->setViewport(new QGLWidget(format, views[0]));
        if (focused)
            views[0]->viewport()->setFocus();
    }
}

/*!
    \internal
*/
qreal StereoView::aspectRatioAdjustment() const
{
    switch (m_layout) {
    case StretchedLeftRight: case StretchedRightLeft: return 2.0f;
    case StretchedTopBottom: case StretchedBottomTop: return 0.5f;
    default: return 1.0f;
    }
}

/*!
    \internal
*/
void StereoView::geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry)
{
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
    updateViewportSize();
}

/*!
    \internal
*/
void StereoView::updateViewportSize()
{
    qreal w = width();
    qreal h = height();
    switch (m_layout) {
    case LeftRight: case RightLeft: w /= 2.0f; break;
    case TopBottom: case BottomTop: h /= 2.0f; break;
    default: break;
    }
    m_viewport->setSize(QSizeF(w, h));
}

QT_END_NAMESPACE
