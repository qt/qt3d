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

void QKeyboardInput::sceneChangeEvent(const QSceneChangePtr &change)
{
    QBackendScenePropertyChangePtr e = qSharedPointerCast<QBackendScenePropertyChange>(change);
    if (e->type() == NodeUpdated && e->propertyName() == QByteArrayLiteral("focus")) {
        bool block = blockNotifications(true);
        setFocus(e->value().toBool());
        blockNotifications(block);
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
