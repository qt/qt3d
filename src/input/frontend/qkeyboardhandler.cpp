/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qkeyboardhandler.h"
#include "qkeyboardhandler_p.h"
#include "qkeyboarddevice.h"
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {

namespace {


// SigMap and the sigMap table are taken from QQ2 QQuickKeysAttached
struct SigMap {
    int key;
    const char *sig;
};

const SigMap sigMap[] = {
    { Qt::Key_Left, "leftPressed" },
    { Qt::Key_Right, "rightPressed" },
    { Qt::Key_Up, "upPressed" },
    { Qt::Key_Down, "downPressed" },
    { Qt::Key_Tab, "tabPressed" },
    { Qt::Key_Backtab, "backtabPressed" },
    { Qt::Key_Asterisk, "asteriskPressed" },
    { Qt::Key_NumberSign, "numberSignPressed" },
    { Qt::Key_Escape, "escapePressed" },
    { Qt::Key_Return, "returnPressed" },
    { Qt::Key_Enter, "enterPressed" },
    { Qt::Key_Delete, "deletePressed" },
    { Qt::Key_Space, "spacePressed" },
    { Qt::Key_Back, "backPressed" },
    { Qt::Key_Cancel, "cancelPressed" },
    { Qt::Key_Select, "selectPressed" },
    { Qt::Key_Yes, "yesPressed" },
    { Qt::Key_No, "noPressed" },
    { Qt::Key_Context1, "context1Pressed" },
    { Qt::Key_Context2, "context2Pressed" },
    { Qt::Key_Context3, "context3Pressed" },
    { Qt::Key_Context4, "context4Pressed" },
    { Qt::Key_Call, "callPressed" },
    { Qt::Key_Hangup, "hangupPressed" },
    { Qt::Key_Flip, "flipPressed" },
    { Qt::Key_Menu, "menuPressed" },
    { Qt::Key_VolumeUp, "volumeUpPressed" },
    { Qt::Key_VolumeDown, "volumeDownPressed" },
    { 0, 0 }
};

const QByteArray keyToSignal(int key)
{
    QByteArray keySignal;
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        keySignal = "digit0Pressed";
        keySignal[5] = '0' + (key - Qt::Key_0);
    } else {
        int i = 0;
        while (sigMap[i].key && sigMap[i].key != key)
            ++i;
        keySignal = sigMap[i].sig;
    }
    return keySignal;
}

} // anonymous

QKeyboardHandlerPrivate::QKeyboardHandlerPrivate()
    : QComponentPrivate()
    , m_keyboardDevice(Q_NULLPTR)
    , m_focus(false)
{
    m_shareable = false;
}

void QKeyboardHandlerPrivate::keyEvent(QKeyEvent *event)
{
    Q_Q(QKeyboardHandler);
    if (event->type() == QEvent::KeyPress) {
        emit q->pressed(event);

        QByteArray keySignal = keyToSignal(event->key());
        if (!keySignal.isEmpty()) {
            keySignal += "(Qt3DInput::QKeyEvent*)";
            // TO DO: Finding if the signal is connected to anything before doing the invocation
            // could be an improvement
            // That's what QQ2 does but since it accesses QML private classes to do so, that may not be
            // applicable in our case
            int idx = QKeyboardHandler::staticMetaObject.indexOfSignal(keySignal);
            q->metaObject()->method(idx).invoke(q, Qt::DirectConnection, Q_ARG(QKeyEvent *, event));
        }
    } else if (event->type() == QEvent::KeyRelease) {
        emit q->released(event);
    }
}


/*!
    \class Qt3DInput::QKeyboardHandler
    \inmodule Qt3DInput
    \since 5.5
*/

/*!
    \qmltype KeyboardHandler
    \inqmlmodule Qt3D.Input
    \instantiates Qt3DInput::QKeyboardHandler
    \inherits Component3D
    \since 5.5
*/

/*!
    Constructs a new QKeyboardHandler instance with parent \a parent.
 */
QKeyboardHandler::QKeyboardHandler(QNode *parent)
    : QComponent(*new QKeyboardHandlerPrivate, parent)
{
}

QKeyboardHandler::~QKeyboardHandler()
{
    QNode::cleanup();
}

void QKeyboardHandler::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QKeyboardHandler *input = static_cast<const QKeyboardHandler *>(ref);

    // TO DO: We may want to store the keyboard device id and only send a clone when we are the parent
    // of the keyboard device
    if (input->d_func()->m_keyboardDevice != Q_NULLPTR && input->d_func()->m_keyboardDevice->parent() == ref)
        setSourceDevice(qobject_cast<QKeyboardDevice *>(QNode::clone(input->d_func()->m_keyboardDevice)));
}

void QKeyboardHandler::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QKeyboardHandler);
    QBackendScenePropertyChangePtr e = qSharedPointerCast<QBackendScenePropertyChange>(change);
    if (e->type() == NodeUpdated) {
        if (e->propertyName() == QByteArrayLiteral("focus")) {
            bool block = blockNotifications(true);
            setFocus(e->value().toBool());
            blockNotifications(block);
        } else if (e->propertyName() == QByteArrayLiteral("event")) {
            QKeyEventPtr ev = e->value().value<QKeyEventPtr>();
            d->keyEvent(ev.data());
        }
    }
}

/*!
    \qmlproperty KeyboardDevice Qt3D.Input::KeyboardHandler::device
*/

/*!
    Sets the keyboard device to \a keyboardDevice. Without a valid device,
    the QKeyboardHandler won't receive any event.
 */
void QKeyboardHandler::setSourceDevice(QKeyboardDevice *keyboardDevice)
{
    Q_D(QKeyboardHandler);
    if (d->m_keyboardDevice != keyboardDevice) {

        if (keyboardDevice && !keyboardDevice->parent())
            keyboardDevice->setParent(this);

        d->m_keyboardDevice = keyboardDevice;
        emit sourceDeviceChanged(keyboardDevice);
    }
}

/*!
    Returns the current keyboard device.
 */
QKeyboardDevice *QKeyboardHandler::sourceDevice() const
{
    Q_D(const QKeyboardHandler);
    return d->m_keyboardDevice;
}

/*!
    \qmlproperty bool Qt3D.Input::KeyboardHandler::focus
*/

/*!
    Returns the current focus.
 */
bool QKeyboardHandler::focus() const
{
    Q_D(const QKeyboardHandler);
    return d->m_focus;
}

/*!
    Sets the focus to \a focus. If focus is not currently set to \c true,
    this component will receive keyboard focus.
 */
void QKeyboardHandler::setFocus(bool focus)
{
    Q_D(QKeyboardHandler);
    if (d->m_focus != focus) {
        d->m_focus = focus;
        emit focusChanged(focus);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QKeyboardHandler::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QKeyboardHandlerData>::create(this);
    auto &data = creationChange->data;

    Q_D(const QKeyboardHandler);
    data.keyboardDeviceId = qIdForNode(d->m_keyboardDevice);

    return creationChange;
}

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit0Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit1Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit2Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit3Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit4Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit5Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit6Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit7Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit8Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::digit9Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::leftPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::rightPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::upPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::downPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::tabPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::backtabPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::asteriskPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::numberSignPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::backtabPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::escapePressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::returnPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::enterPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::deletePressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::spacePressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::backPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::cancelPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::selectPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::yesPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::noPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::context1Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::context2Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::context3Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::context4Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::callPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::hangupPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::flipPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::menuPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::volumeUpPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::volumeDownPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardHandler::released(KeyEvent event)
*/

} // namespace Qt3DInput

QT_END_NAMESPACE
