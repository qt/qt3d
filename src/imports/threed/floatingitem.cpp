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

#include "floatingitem.h"
#include "floatingitemeffect.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass FloatingItem FloatingItem
    \brief The FloatingItem item defines the depth that a 2D element will be placed at in the 3D visual field.
    \since 4.8
    \ingroup qt3d::qml3d

    Stereo viewing is enabled in QML using the StereoView element:

    \snippet declarative/stereo/stereo.qml 1
    \dots
    \snippet declarative/stereo/stereo.qml 2

    \bold{Note:} StereoView is not supported with Qt Quick 2.0 as the
    standard view classes are already stereo-aware.

    \image stereo-screenshot-qml.png

    2D items are placed within the 3D visual field using the
    FloatingItem element and the \l depth property:

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

    FloatingItem defines the depth for 2D elements.  3D elements
    should be rendered inside a \l Viewport with the camera's
    \l{Camera::eyeSeparation}{eyeSeparation} set to a non-zero value:

    \snippet declarative/stereo/stereo.qml 6
    \dots
    \snippet declarative/stereo/stereo.qml 7

    \sa StereoView, Viewport, {QML Stereo Viewing Example}
*/

/*!
    \internal
*/
FloatingItem::FloatingItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_depth(0.0f)
{
    // The actual stereo adjustment is done with a QGraphicsEffect.
    m_effect = new FloatingItemEffect(this);
    setGraphicsEffect(m_effect);
}

/*!
    \internal
*/
FloatingItem::~FloatingItem()
{
}

/*!
    \qmlproperty real FloatingItem::depth

    This property defines the depth within the 3D field where children
    of this item will appear.  The default value of zero indicates
    that the left and right eye images will co-incide at the center of
    focus.  Items with a negative depth appear behind the center
    of focus and items with a positive depth float above the center
    of focus.

    The value indicates the number of pixels that separate the left
    and right eye images on a 100 dpi display.  The value is corrected
    for the dpi of the actual display if it is not 100 dpi.  Thus,
    a depth of 1 always corresponds to 0.254 millimeters of image
    separation at the surface of the display.

    It is assumed that the distance of the viewer from the display
    is the same for all displays showing the same application.
    For example, a stereoscopic application designed for viewing
    on a handheld device should have the same depth cues on
    another handheld device with a different dpi, when held at
    the same arms-length distance from the viewer.
*/

void FloatingItem::setDepth(qreal depth)
{
    if (m_depth != depth) {
        m_depth = depth;
        emit depthChanged();
        update();
    }
}

QT_END_NAMESPACE
