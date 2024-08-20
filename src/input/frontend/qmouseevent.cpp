// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmouseevent.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace {

template<typename EventClass, typename QtEventClass>
typename EventClass::Modifiers modifiersForEvent(const QtEventClass *event)
{
    const Qt::KeyboardModifiers eventModifiers = event->modifiers();
    int modifiers = EventClass::NoModifier;

    if (eventModifiers & Qt::ShiftModifier)
        modifiers |= EventClass::ShiftModifier;

    if (eventModifiers & Qt::ControlModifier)
        modifiers |= EventClass::ControlModifier;

    if (eventModifiers & Qt::AltModifier)
        modifiers |= EventClass::AltModifier;

    if (eventModifiers & Qt::MetaModifier)
        modifiers |= EventClass::MetaModifier;

    if (eventModifiers & Qt::KeypadModifier)
        modifiers |= EventClass::KeypadModifier;

    // Abuse the int used to store an enum to store multiple
    // modifiers into one
    return static_cast<typename EventClass::Modifiers>(modifiers);
}

} // anonymous

// Notes:
// Maybe we should provide the world pos of the intersection
// The distance t along the segment line at which the intersection occurs
// Screen Pos / Mouse Pos / Viewport Pos
// The intersection Ray
// These can always be added in follow up commits once the input API takes shape

/*!
 * \qmltype MouseEvent
 * \nativetype Qt3DInput::QMouseEvent
 * \inqmlmodule Qt3D.Input
 * \since 5.5
 * \brief Provides parameters that describe a mouse event.
 *
 * Mouse events occur when a mouse button is pressed and the ray
 * traversing the view, originating from the mouse position intersects with one
 * or more elements of the scene.
 *
 * \sa KeyEvent, WheelEvent, MouseHandler
 */

/*!
 * \qmlproperty int Qt3D.Input::MouseEvent::x
 * Specifies The X coordinate of the mouse event
 * \readonly
 */

/*!
 * \qmlproperty int Qt3D.Input::MouseEvent::y
 * Specifies The Y coordinate of the mouse event
 * \readonly
 */

/*!
 * \qmlproperty bool Qt3D.Input::MouseEvent::wasHeld
 * Specifies if a mouse button was held down during the mouse event
 * \readonly
 */

/*!
 * \qmlproperty Buttons Qt3D.Input::MouseEvent::button
 * Specifies the button triggering the mouse event
 * \readonly
 */

/*!
 * \qmlproperty int Qt3D.Input::MouseEvent::buttons
 * Specifies the button triggering the mouse event
 * \readonly
 */

/*!
 * \qmlproperty Modifiers Qt3D.Input::MouseEvent::modifiers
 * Specifies if any modifiers were applied to the mouse event
 * \readonly
 */

/*!
 * \qmlproperty bool Qt3D.Input::MouseEvent::accepted
 * Specifies if the mouse event has been accepted
 */

/*!
 * \class Qt3DInput::QMouseEvent
 * \inheaderfile Qt3DInput/QMouseEvent
 * \inmodule Qt3DInput
 *
 * \brief The Qt3DCore::QMouseEvent contains parameters that describe a mouse event.
 *
 * Mouse events occur when a mouse button is pressed and the ray
 * traversing the view, originating from the mouse position intersects with one
 * or more elements of the scene.
 *
 * \since 5.5
 *
 * \sa QKeyEvent, QWheelEvent, QMouseHandler
 *
 */

/*!
 * \property Qt3DInput::QMouseEvent::x
 * Specifies The X coordinate of the mouse event
 * \readonly
 */

/*!
 * \property Qt3DInput::QMouseEvent::y
 * Specifies The y coordinate of the mouse event
 * \readonly
 */

/*!
 * \property Qt3DInput::QMouseEvent::wasHeld
 * Specifies if a mouse button was held down during the mouse event
 * \readonly
 */

/*!
 * \property Qt3DInput::QMouseEvent::button
 * Specifies the button triggering the mouse event
 * \readonly
 */

/*!
 * \property Qt3DInput::QMouseEvent::buttons
 * Specifies the button triggering the mouse event
 * \readonly
 */

/*!
 * \property Qt3DInput::QMouseEvent::modifiers
 * Specifies if any modifiers were applied to the mouse event
 * \readonly
 */

/*!
 * \property Qt3DInput::QMouseEvent::accepted
 * Specifies if the mouse event has been accepted
 */

/*!
 * \enum Qt3DInput::QMouseEvent::Buttons
 *
 * \value LeftButton
 * \value RightButton
 * \value MiddleButton
 * \value BackButton
 * \value NoButton
 */

/*!
 * \enum Qt3DInput::QMouseEvent::Modifiers
 *
 * \value NoModifier
 * \value ShiftModifier
 * \value ControlModifier
 * \value AltModifier
 * \value MetaModifier
 * \value KeypadModifier
 */

/*!
 * \typedef Qt3DInput::QMouseEventPtr
 * \relates Qt3DInput::QMouseEvent
 *
 * A shared pointer for QMouseEvent.
 */

/*!
 * \fn int Qt3DInput::QMouseEvent::x() const
 *
 *  Returns the x position of the mouse event.
 */

/*!
 * \fn int Qt3DInput::QMouseEvent::y() const
 *
 *  Returns the y position of the mouse event.
 */

/*!
 * \fn bool Qt3DInput::QMouseEvent::isAccepted() const
 *
 *  Returns whether the event was accepted.
 */

/*!
 * \fn void Qt3DInput::QMouseEvent::setAccepted(bool accepted)
 *
 *  Sets the event as accepted if \a accepted is true.
 *
 * \note When an event is accepted, it will prevent further propagation to other
 * listeners.
 */

/*!
 * \fn QEvent::Type Qt3DInput::QMouseEvent::type() const
 *
 *  Returns the QEvent::Type of the event.
 */

/*!
 * Constructs a new QMouseEvent instance for the QMouseEvent \a e.
 */
QMouseEvent::QMouseEvent(const QT_PREPEND_NAMESPACE(QMouseEvent) &e)
    : QObject()
      , m_event(static_cast<QT_PREPEND_NAMESPACE(QMouseEvent)*>(e.clone()))
{
}

QMouseEvent::~QMouseEvent()
{
}

/*!
 * Returns the mouse button of the mouse event.
 */
QMouseEvent::Buttons QMouseEvent::button() const
{
    switch (m_event->button()) {
    case Qt::LeftButton:
        return QMouseEvent::LeftButton;
    case Qt::RightButton:
        return QMouseEvent::RightButton;
    case Qt::MiddleButton:
        return QMouseEvent::MiddleButton;
    case Qt::BackButton:
        return QMouseEvent::BackButton;
    default:
        return QMouseEvent::NoButton;
    }
}

/*!
 * Returns a bitfield to be used to check for mouse buttons that may be
 * accompanying the mouse event.
 */
int QMouseEvent::buttons() const
{
    return m_event->buttons();
}

/*!
 * Returns the keyboard modifiers that may be accompanying the mouse event.
 */
QMouseEvent::Modifiers QMouseEvent::modifiers() const
{
    return modifiersForEvent<QMouseEvent, QT_PREPEND_NAMESPACE(QMouseEvent)>(m_event.get());
}

/*!
 * \qmltype WheelEvent
 * \nativetype Qt3DInput::QWheelEvent
 * \inqmlmodule Qt3D.Input
 * \since 5.5
 * \brief Contains parameters that describe a mouse wheel event.
 *
 * Mouse wheel events occur when the mouse wheel is rotated.
 *
 * \sa KeyEvent, MouseEvent, MouseHandler
 *
 */

/*!
 * \qmlproperty int Qt3D.Input::WheelEvent::x
 * Specifies The X coordinate of the mouse wheel event
 * \readonly
 */

/*!
 * \qmlproperty int Qt3D.Input::WheelEvent::y
 * Specifies The Y coordinate of the mouse wheel event
 * \readonly
 */

/*!
 * \qmlproperty Point Qt3D.Input::WheelEvent::angleDelta
 * Specifies The change wheel angle of the mouse wheel event
 * \readonly
 */

/*!
 * \qmlproperty int Qt3D.Input::WheelEvent::buttons
 * Specifies the button if present in the mouse wheel event
 * \readonly
 */

/*!
 * \qmlproperty Modifiers Qt3D.Input::WheelEvent::modifiers
 * Specifies if any modifiers were applied to the mouse wheel event
 * \readonly
 */

/*!
 * \qmlproperty bool Qt3D.Input::WheelEvent::accepted
 * Specifies if the mouse wheel event has been accepted
 */

/*!
 * \class Qt3DInput::QWheelEvent
 * \inheaderfile Qt3DInput/QWheelEvent
 * \inmodule Qt3DInput
 *
 * \brief The QWheelEvent class contains parameters that describe a mouse wheel event.
 *
 * Mouse wheel events occur when the mouse is rotated.
 *
 * \since 5.5
 *
 * \sa QKeyEvent, QMouseEvent, QMouseHandler
 *
 */

/*!
 * \property Qt3DInput::QWheelEvent::x
 * Specifies The X coordinate of the mouse wheel event
 * \readonly
 */

/*!
 * \property Qt3DInput::QWheelEvent::y
 * Specifies The Y coordinate of the mouse wheel event
 * \readonly
 */

/*!
 * \property Qt3DInput::QWheelEvent::angleDelta
 * Specifies The change wheel angle of the mouse wheel event
 * \readonly
 */

/*!
 * \property Qt3DInput::QWheelEvent::buttons
 * Specifies the button if present in the mouse wheel event
 * \readonly
 */

/*!
 * \property  Qt3DInput::QWheelEvent::modifiers
 * Specifies if any modifiers were applied to the mouse wheel event
 * \readonly
 */

/*!
 * \property Qt3DInput::QWheelEvent::accepted
 * Specifies if the mouse wheel event has been accepted
 */

/*!
 * \enum Qt3DInput::QWheelEvent::Buttons
 *
 * \value LeftButton
 * \value RightButton
 * \value MiddleButton
 * \value BackButton
 * \value NoButton
 */

/*!
 * \enum Qt3DInput::QWheelEvent::Modifiers
 *
 * \value NoModifier
 * \value ShiftModifier
 * \value ControlModifier
 * \value AltModifier
 * \value MetaModifier
 * \value KeypadModifier
 */


/*!
 * \typedef Qt3DInput::QWheelEventPtr
 * \relates Qt3DInput::QWheelEvent
 *
 * A shared pointer for QWheelEvent.
 */

/*!
 * \fn int Qt3DInput::QWheelEvent::x() const
 *
 *  Returns the x position of the mouse event.
 */

/*!
 * \fn int Qt3DInput::QWheelEvent::y() const
 *
 *  Returns the x position of the mouse event.
 */

/*!
 * \fn QPoint Qt3DInput::QWheelEvent::angleDelta() const
 *
 * Returns the distance that the wheel is rotated, in eighths of a degree. A
 * positive value indicates that the wheel was rotated forward (away from the
 * user), a negative value indicates the wheel was rotated backward (toward the
 * user).
 */

/*!
 * \fn bool Qt3DInput::QWheelEvent::isAccepted() const
 *
 *  Returns whether the event was accepted.
 */

/*!
 * \fn void Qt3DInput::QWheelEvent::setAccepted(bool accepted)
 *
 *  Sets the event as accepted if \a accepted is true.
 *
 * \note When an event is accepted, it will prevent further propagation to other
 * listeners.
 */

/*!
 * \fn QEvent::Type Qt3DInput::QWheelEvent::type() const
 *
 *  Returns the QEvent::Type of the event.
 */

#if QT_CONFIG(wheelevent)
/*!
 * Constructs a new QWheelEvent instance from the QWheelEvent \a e.
 */
QWheelEvent::QWheelEvent(const QT_PREPEND_NAMESPACE(QWheelEvent) &e)
    : QObject()
      , m_event(static_cast<QT_PREPEND_NAMESPACE(QWheelEvent)*>(e.clone()))
{
}

QWheelEvent::~QWheelEvent()
{
}

/*!
 * Returns a bitfield to be used to check for mouse buttons that may be
 * accompanying the wheel event.
 */
int QWheelEvent::buttons() const
{
    return m_event->buttons();
}

/*!
 * Returns the keyboard modifiers that may be accompanying the wheel event.
 */
QWheelEvent::Modifiers QWheelEvent::modifiers() const
{
    return modifiersForEvent<QWheelEvent, QT_PREPEND_NAMESPACE(QWheelEvent)>(m_event.get());
}
#endif // QT_CONFIG(wheelevent)

} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_qmouseevent.cpp"
