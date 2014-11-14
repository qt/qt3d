/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
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

QKeyboardInputPrivate::QKeyboardInputPrivate(QKeyboardInput *qq)
    : QComponentPrivate(qq)
    , m_controller(Q_NULLPTR)
    , m_focus(false)
{
    m_shareable = false;
}

QKeyboardInput::QKeyboardInput(QNode *parent)
    : QComponent(*new QKeyboardInputPrivate(this), parent)
{
}

QKeyboardInput::QKeyboardInput(QKeyboardInputPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

void QKeyboardInput::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QKeyboardInput *input = static_cast<const QKeyboardInput *>(ref);

    if (input->d_func()->m_controller != Q_NULLPTR)
        setController(qobject_cast<QKeyboardController *>(QNodePrivate::get(input->d_func()->m_controller)->clone()));
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
            keySignal += "(Q3DKeyEvent*)";
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

void QKeyboardInput::setController(QKeyboardController *controller)
{
    Q_D(QKeyboardInput);
    if (d->m_controller != controller) {
        d->m_controller = controller;
        emit controllerChanged();
    }
}

QKeyboardController *QKeyboardInput::controller() const
{
    Q_D(const QKeyboardInput);
    return d->m_controller;
}

bool QKeyboardInput::focus() const
{
    Q_D(const QKeyboardInput);
    return d->m_focus;
}

void QKeyboardInput::setFocus(bool focus)
{
    Q_D(QKeyboardInput);
    if (d->m_focus != focus) {
        d->m_focus = focus;
        emit focusChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
