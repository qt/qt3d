/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qkeyboardinput.h"
#include "qkeyboardinput_p.h"
#include "qkeyboardcontroller.h"
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QKeyboardInputPrivate
    \internal
*/
QKeyboardInputPrivate::QKeyboardInputPrivate()
    : QComponentPrivate()
    , m_controller(Q_NULLPTR)
    , m_focus(false)
{
    m_shareable = false;
}

/*!
    \class Qt3D::QKeyboardInput
    \inmodule Qt3DInput
    \since 5.5
*/

/*!
    \qmltype KeyboardInput
    \inqmlmodule Qt3D.Input
    \instantiates Qt3D::QKeyboardInput
    \inherits Component3D
    \since 5.5
*/

/*!
    Constructs a new QKeyboardInput instance with parent \a parent.
 */
QKeyboardInput::QKeyboardInput(QNode *parent)
    : QComponent(*new QKeyboardInputPrivate, parent)
{
}

QKeyboardInput::~QKeyboardInput()
{
    QNode::cleanup();
}

/*! \internal */
QKeyboardInput::QKeyboardInput(QKeyboardInputPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

void QKeyboardInput::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QKeyboardInput *input = static_cast<const QKeyboardInput *>(ref);

    // TO DO: We may want to store the controller id and only send a clone when we are the parent
    // of the controller
    if (input->d_func()->m_controller != Q_NULLPTR && input->d_func()->m_controller->parent() == ref)
        setController(qobject_cast<QKeyboardController *>(QNode::clone(input->d_func()->m_controller)));
}

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

void QKeyboardInput::sceneChangeEvent(const QSceneChangePtr &change)
{
    QBackendScenePropertyChangePtr e = qSharedPointerCast<QBackendScenePropertyChange>(change);
    if (e->type() == NodeUpdated) {
        if (e->propertyName() == QByteArrayLiteral("focus")) {
            bool block = blockNotifications(true);
            setFocus(e->value().toBool());
            blockNotifications(block);
        } else if (e->propertyName() == QByteArrayLiteral("event")) {
            Q3DKeyEventPtr ev = e->value().value<Q3DKeyEventPtr>();
            keyEvent(ev.data());
        }
    }
}

void QKeyboardInput::keyEvent(Q3DKeyEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        emit pressed(event);

        QByteArray keySignal = keyToSignal(event->key());
        if (!keySignal.isEmpty()) {
            keySignal += "(Qt3D::Q3DKeyEvent*)";
            // TO DO: Finding if the signal is connected to anything before doing the invocation
            // could be an improvement
            // That's what QQ2 does but since it accesses QML private classes to do so, that may not be
            // applicable in our case
            int idx = QKeyboardInput::staticMetaObject.indexOfSignal(keySignal);
            metaObject()->method(idx).invoke(this, Qt::DirectConnection, Q_ARG(Q3DKeyEvent *, event));
        }
    } else if (event->type() == QEvent::KeyRelease) {
        emit released(event);
    }
}

/*!
    \qmlproperty KeyboardController Qt3D.Input::KeyboardInput::controller
*/

/*!
    Sets the keyboard controller to \a controller. Without a valid controller,
    the QKeyboardInput won't receive any event.
 */
void QKeyboardInput::setController(QKeyboardController *controller)
{
    Q_D(QKeyboardInput);
    if (d->m_controller != controller) {

        if (controller && !controller->parent())
            controller->setParent(this);

        d->m_controller = controller;
        emit controllerChanged();
    }
}

/*!
    \returns the current keyboard controller.
 */
QKeyboardController *QKeyboardInput::controller() const
{
    Q_D(const QKeyboardInput);
    return d->m_controller;
}

/*!
    \qmlproperty bool Qt3D.Input::KeyboardInput::focus
*/

/*!
    \returns the current focus.
 */
bool QKeyboardInput::focus() const
{
    Q_D(const QKeyboardInput);
    return d->m_focus;
}

/*!
    Sets the focus to \a focus. If focus is not currently set to true, this component will receive keyboard focus.
 */
void QKeyboardInput::setFocus(bool focus)
{
    Q_D(QKeyboardInput);
    if (d->m_focus != focus) {
        d->m_focus = focus;
        emit focusChanged();
    }
}

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit0Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit1Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit2Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit3Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit4Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit5Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit6Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit7Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit8Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::digit9Pressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::leftPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::rightPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::upPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::downPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::tabPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::backtabPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::asteriskPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::numberSignPressed(KeyEvent event)
*/

/*!
    \qmlsignal Qt3D.Input::KeyboardInput::backtabPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::escapePressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::returnPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::enterPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::deletePressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::spacePressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::backPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::cancelPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::selectPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::yesPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::noPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::context1Pressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::context2Pressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::context3Pressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::context4Pressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::callPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::hangupPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::flipPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::menuPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::volumeUpPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::volumeDownPressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::pressed(KeyEvent event)
*/

/*!
    qmlsignal Qt3D.Input::KeyboardInput::released(KeyEvent event)
*/

} // Qt3D

QT_END_NAMESPACE
