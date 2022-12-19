// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qkeyevent.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

//Qt6: Move this into a QtQmlGui module and merge it with QQuickKeyEvent

/*!
    \class Qt3DInput::QKeyEvent
    \inmodule Qt3DInput
    \brief QKeyEvent event type send by KeyBoardHandler.
    \since 5.5
    \brief Contains parameters that describe a key event
*/

/*!
    \qmltype KeyEvent
    \inqmlmodule Qt3D.Input
    \instantiates Qt3DInput::QKeyEvent
    \brief QML frontend for QKeyEvent C++ class.
    \since 5.5
    \brief Contains parameters that describe a key event

    The KeyEvent QML type cannot be directly created. Objects of this type
    are used as signal parameters in KeyboardHandler.
*/

/*!
 * \typedef Qt3DInput::QKeyEventPtr
 * \relates Qt3DInput::QKeyEvent
 *
 * A shared pointer for QKeyEvent.
 */

QKeyEvent::QKeyEvent(QEvent::Type type, int key, Qt::KeyboardModifiers modifiers, const QString &text, bool autorep, ushort count)
    : QObject()
      , m_event(new QT_PREPEND_NAMESPACE(QKeyEvent){type, key, modifiers, text, autorep, count})
{
    m_event->setAccepted(false);
}

QKeyEvent::QKeyEvent(const QT_PREPEND_NAMESPACE(QKeyEvent) &ke)
    : QObject()
    , m_event(static_cast<QT_PREPEND_NAMESPACE(QKeyEvent) *>(ke.clone()))
{
    m_event->setAccepted(false);
}

/*! \internal */
QKeyEvent::~QKeyEvent()
{
}

/*!
    \qmlproperty int Qt3D.Input::KeyEvent::key
    \readonly

    This property holds the code of the key that was pressed or released.

    See \l [CPP] {Qt::Key}{Qt.Key} for the list of keyboard codes.

    \sa {QtQuick::KeyEvent::key}{KeyEvent.key}
*/

/*!
    \qmlproperty string Qt3D.Input::KeyEvent::text
    \readonly

    This property holds the Unicode text that the key generated. The text
    returned can be an empty string in cases where modifier keys, such as
    Shift, Control, Alt, and Meta, are being pressed or released. In such
    cases \l key will contain a valid value.
*/

/*!
    \qmlproperty int Qt3D.Input::KeyEvent::modifiers
    \readonly

    This property holds the keyboard modifier flags that existed immediately
    before the event occurred.

    \sa {QtQuick::KeyEvent::modifiers}{KeyEvent.modifiers}
*/

/*!
    \qmlproperty bool Qt3D.Input::KeyEvent::isAutoRepeat
    \readonly

    Holds whether this event comes from an auto-repeating key.
*/

/*!
    \qmlproperty int Qt3D.Input::KeyEvent::count
    \readonly

    Holds the number of keys involved in this event. If \l text is not empty,
    this is simply the length of the string.
*/

/*!
    \qmlproperty quint32 Qt3D.Input::KeyEvent::nativeScanCode
    \readonly

    This property contains the native scan code of the key that was pressed.
    It is passed through from QKeyEvent unchanged.

    \sa QKeyEvent::nativeScanCode()
*/

/*!
    \qmlproperty bool Qt3D.Input::KeyEvent::accepted

    Setting \e accepted to \c true prevents the key event from being propagated
    to the item's parent.

    Generally, if the item acts on the key event then it should be accepted so
    that ancestor items do not also respond to the same event.
*/

/*!
    \qmlmethod bool Qt3D.Input::KeyEvent::matches(StandardKey key)

    Returns \c true if the key event matches the given standard \a key; otherwise
    returns \c false.

    \sa QKeySequence::StandardKey
*/

/*!
    \property Qt3DInput::QKeyEvent::key
    \readonly

    This property holds the code of the key that was pressed or released.

    See \l [CPP] {Qt::Key}{Qt.Key} for the list of keyboard codes.

    \b {See also} \l [QtGui] {QKeyEvent::key()}.
*/

/*!
    \property Qt3DInput::QKeyEvent::text
    \readonly

    This property holds the Unicode text that the key generated. The text
    returned can be an empty string in cases where modifier keys, such as
    Shift, Control, Alt, and Meta, are being pressed or released. In such
    cases \l key will contain a valid value.
*/

/*!
    \property Qt3DInput::QKeyEvent::modifiers
    \readonly

    This property holds the keyboard modifier flags that existed immediately
    before the event occurred.

    \b {See also} \l [QtGui] {QKeyEvent::modifiers()}.
*/

/*!
    \property Qt3DInput::QKeyEvent::isAutoRepeat
    \readonly

    Holds whether this event comes from an auto-repeating key.
*/

/*!
    \property Qt3DInput::QKeyEvent::count
    \readonly

    Holds the number of keys involved in this event. If \l text is not empty,
    this is simply the length of the string.
*/

/*!
    \property Qt3DInput::QKeyEvent::nativeScanCode
    \readonly

    This property contains the native scan code of the key that was pressed.
    It is passed through from QKeyEvent unchanged.

*/

/*!
    \property  Qt3DInput::QKeyEvent::accepted

    Setting \e accepted to \c true prevents the key event from being propagated
    to the item's parent.

    Generally, if the item acts on the key event then it should be accepted so
    that ancestor items do not also respond to the same event.
*/

/*!
    \fn bool Qt3DInput::QKeyEvent::matches(QKeySequence::StandardKey key_) const

    Returns \c true if the key event matches the given standard key \a key_; otherwise
    returns \c false.

    \sa QKeySequence::StandardKey
*/

/*!
    \fn QEvent::Type Qt3DInput::QKeyEvent::type() const
    Returns the type of the event.

*/

} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_qkeyevent.cpp"
