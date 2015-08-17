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

#include "qabstractcollider.h"
#include "qabstractcollider_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractColliderPrivate
    \internal
*/
QAbstractColliderPrivate::QAbstractColliderPrivate()
    : QComponentPrivate()
{
}

/*! \internal */
QAbstractCollider::QAbstractCollider(QAbstractColliderPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

QAbstractCollider::~QAbstractCollider()
{
    QNode::cleanup();
}

void QAbstractCollider::onEntered(QCollisionEvent *e)
{
    // TODO: Emit with other collider (or event for more info about the collision)
    Q_UNUSED(e);
    emit entered(Q_NULLPTR);
}

void QAbstractCollider::onCollidingWith(QCollisionEvent *e)
{
    // TODO: Emit with other collider (or event for more info about the collision)
    Q_UNUSED(e);
    emit collidingWith(Q_NULLPTR);
}

void QAbstractCollider::onExited(QCollisionEvent *e)
{
    // TODO: Emit with other collider (or event for more info about the collision)
    Q_UNUSED(e);
    emit exited(Q_NULLPTR);
}

QT_END_NAMESPACE

} // namespace Qt3D

