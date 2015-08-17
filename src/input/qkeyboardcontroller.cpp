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

#include "qkeyboardcontroller.h"
#include "qkeyboardcontroller_p.h"
#include "qkeyboardinput.h"
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QKeyboardControllerPrivate
    \internal
*/
QKeyboardControllerPrivate::QKeyboardControllerPrivate()
    : QNodePrivate()
{
}

/*!
    \class Qt3D::QKeyboardController
    \inmodule Qt3DInput
    \since 5.5
*/

/*!
    \qmltype KeyboardController
    \inqmlmodule Qt3D.Input
    \since 5.5
    \instantiates Qt3D::QKeyboardController
    \inherits Node
*/

QKeyboardController::QKeyboardController(QNode *parent)
    : QNode(*new QKeyboardControllerPrivate, parent)
{
}

QKeyboardController::~QKeyboardController()
{
    QNode::cleanup();
}

/*!
    \qmlproperty KeyboardInput Qt3D.Input::KeyboardController::activeInput
    \readonly
*/
QKeyboardInput *QKeyboardController::activeInput() const
{
    Q_D(const QKeyboardController);
    return d->m_activeInput;
}

/*! \internal */
QKeyboardController::QKeyboardController(QKeyboardControllerPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QKeyboardController::sceneChangeEvent(const QSceneChangePtr &change)
{
    Q_D(QKeyboardController);
    QBackendScenePropertyChangePtr e = qSharedPointerCast<QBackendScenePropertyChange>(change);
    if (e->type() == NodeUpdated && e->propertyName() == QByteArrayLiteral("activeInput")) {
        QNodeId activeInputId = e->value().value<QNodeId>();
        setActiveInput(qobject_cast<QKeyboardInput *>(d->scene()->lookupNode(activeInputId)));
    }
}

void QKeyboardController::setActiveInput(QKeyboardInput *activeInput)
{
    Q_D(QKeyboardController);
    if (d->m_activeInput != activeInput) {
        d->m_activeInput = activeInput;
        emit activeInputChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
