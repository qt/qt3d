/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "q3dmouseevent.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

// Notes:
// Maybe we should provide the world pos of the intersection
// The distance t along the segment line at which the intersection occurs
// Screen Pos / Mouse Pos / Viewport Pos
// The intersection Ray
// These can always be added in follow up commits once the input API takes shape

/*!
 * \class Qt3D::Q3DMouseEvent
 *
 * \brief The Qt3D::Q3DMouseEvent contains parameters that describe a mouse event.
 *
 * Mouse events occur when a mouse button is pressed and the ray
 * traversing the view, originating from the mouse position intersects with one
 * or more elements of the scene.
 *
 * \since 5.5
 *
 * \sa Qt3D::Q3DKeyEvent, Qt3D::Q3DWheelEvent, Qt3D::QMouseInput
 *
 */

/*!
 * \fn int Qt3D::Q3DMouseEvent::x() const
 *
 *  Returns the x position of the mouse event.
 */

/*!
 * \fn int Qt3D::Q3DMouseEvent::y() const
 *
 *  Returns the y position of the mouse event.
 */

/*!
 * \fn bool Qt3D::Q3DMouseEvent::isAccepted() const
 *
 *  Returns whether the event was accepted.
 */

/*!
 * \fn void Qt3D::Q3DMouseEvent::setAccepted(bool accepted)
 *
 *  Sets the event as accepted if \a accepted is true.
 *
 * \note When an event is accepted, it will prevent further propagation to other
 * listeners.
 */

/*!
 * \fn QEvent::Type Qt3D::Q3DMouseEvent::type() const
 *
 *  Returns the QEvent::Type of the event.
 */

/*!
 * Constructs a new Qt3D::Q3DMouseEvent instance for the QMouseEvent \a e.
 */
Q3DMouseEvent::Q3DMouseEvent(const QMouseEvent &e)
    : QObject()
    , m_event(e)
{
}

Q3DMouseEvent::~Q3DMouseEvent()
{
}

/*!
 * Returns the mouse button of the mouse event.
 */
Q3DMouseEvent::Buttons Q3DMouseEvent::button() const
{
    switch (m_event.button()) {
    case Qt::MouseButton::LeftButton:
        return Q3DMouseEvent::Buttons::LeftButton;
    case Qt::MouseButton::RightButton:
        return Q3DMouseEvent::Buttons::RightButton;
    case Qt::MouseButton::MiddleButton:
        return Q3DMouseEvent::Buttons::MiddleButton;
    case Qt::MouseButton::BackButton:
        return Q3DMouseEvent::Buttons::BackButton;
    default:
        return Q3DMouseEvent::Buttons::NoButton;
    }
}

/*!
 * Returns a bitfield to be used to check for mouse buttons that may be
 * accompanying the mouse event.
 */
int Q3DMouseEvent::buttons() const
{
   return m_event.buttons();
}

/*!
 * Returns the keyboard modifier that may be accompanying the mouse event.
 */
Q3DMouseEvent::Modifiers Q3DMouseEvent::modifier() const
{
    switch (m_event.modifiers()) {
    case Qt::ShiftModifier:
        return Q3DMouseEvent::Modifiers::ShiftModifier;
    case Qt::ControlModifier:
        return Q3DMouseEvent::Modifiers::ControlModifier;
    case Qt::AltModifier:
        return Q3DMouseEvent::Modifiers::AltModifier;
    case Qt::MetaModifier:
        return Q3DMouseEvent::Modifiers::MetaModifier;
    case Qt::KeypadModifier:
        return Q3DMouseEvent::Modifiers::KeypadModifier;
    default:
        return Q3DMouseEvent::Modifiers::NoModifier;
    }
}


/*!
 * \class Qt3D::Q3DWheelEvent
 *
 * \brief The Qt3D::Q3DWheelEvent contains parameters that describe a mouse wheel event.
 *
 * Mouse wheel events occur when the mouse is rotated.
 *
 * \since 5.5
 *
 * \sa Qt3D::Q3DKeyEvent, Qt3D::Q3DMouseEvent, Qt3D::QMouseInput
 *
 */

/*!
 * \fn int Qt3D::Q3DWheelEvent::x() const
 *
 *  Returns the x position of the mouse event.
 */

/*!
 * \fn int Qt3D::Q3DWheelEvent::y() const
 *
 *  Returns the x position of the mouse event.
 */

/*!
 * \fn QPoint Qt3D::Q3DWheelEvent::angleDelta() const
 *
 * Returns the distance that the wheel is rotated, in eighths of a degree. A
 * positive value indicates that the wheel was rotated forward (away from the
 * user), a negative value indicates the wheel was rotated backward (toward the
 * user).
 */

/*!
 * \fn bool Qt3D::Q3DWheelEvent::isAccepted() const
 *
 *  Returns whether the event was accepted.
 */

/*!
 * \fn void Qt3D::Q3DWheelEvent::setAccepted(bool accepted)
 *
 *  Sets the event as accepted if \a accepted is true.
 *
 * \note When an event is accepted, it will prevent further propagation to other
 * listeners.
 */

/*!
 * \fn QEvent::Type Qt3D::Q3DWheelEvent::type() const
 *
 *  Returns the QEvent::Type of the event.
 */

/*!
 * Constructs a new Qt3D::Q3DWheelEvent instance from the QWheelEvent \a e.
 */
Q3DWheelEvent::Q3DWheelEvent(const QWheelEvent &e)
    : QObject()
    , m_event(e)
{
}

Q3DWheelEvent::~Q3DWheelEvent()
{
}

/*!
 * Returns a bitfield to be used to check for mouse buttons that may be
 * accompanying the wheel event.
 */
int Q3DWheelEvent::buttons() const
{
    return m_event.buttons();
}

/*!
 * Returns the keyboard modifier that may be accompanying the wheel event.
 */
Q3DWheelEvent::Modifiers Q3DWheelEvent::modifier() const
{
    switch (m_event.modifiers()) {
    case Qt::ShiftModifier:
        return Q3DWheelEvent::Modifiers::ShiftModifier;
    case Qt::ControlModifier:
        return Q3DWheelEvent::Modifiers::ControlModifier;
    case Qt::AltModifier:
        return Q3DWheelEvent::Modifiers::AltModifier;
    case Qt::MetaModifier:
        return Q3DWheelEvent::Modifiers::MetaModifier;
    case Qt::KeypadModifier:
        return Q3DWheelEvent::Modifiers::KeypadModifier;
    default:
        return Q3DWheelEvent::Modifiers::NoModifier;
    }
}

} // Qt3D

QT_END_NAMESPACE
