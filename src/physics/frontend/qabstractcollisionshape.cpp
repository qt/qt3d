/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qabstractcollisionshape.h"
#include "qabstractcollisionshape_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DPhysics {

QAbstractCollisionShapePrivate::QAbstractCollisionShapePrivate()
    : Qt3DCore::QNodePrivate()
{
}

QAbstractCollisionShape::QAbstractCollisionShape(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QAbstractCollisionShapePrivate, parent)
{
}

QAbstractCollisionShape::QAbstractCollisionShape(QAbstractCollisionShapePrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

QAbstractCollisionShape::~QAbstractCollisionShape()
{
}

Qt3DCore::QNodeCreatedChangeBasePtr QAbstractCollisionShape::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QAbstractCollisionShapeData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QAbstractCollisionShape);
    // TODO: Send data members in creation change
    return creationChange;
}

} // namespace Qt3DPhysics

QT_END_NAMESPACE
