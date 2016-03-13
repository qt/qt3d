/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qnodecreatedchange.h"
#include "qnodecreatedchange_p.h"
#include <Qt3DCore/qnode.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QNodeCreatedChangeBasePrivate::QNodeCreatedChangeBasePrivate(const QNode *node)
    : QSceneChangePrivate()
    , m_parentId(node->parentNode() ? node->parentNode()->id() : QNodeId())
    , m_metaObject(node->metaObject())
    , m_nodeEnabled(node->isEnabled())
{
}

QNodeCreatedChangeBase::QNodeCreatedChangeBase(const QNode *node, Priority priority)
    : QSceneChange(*new QNodeCreatedChangeBasePrivate(node), NodeCreated, QSceneChange::Node, node->id(), priority)
{
}

QNodeCreatedChangeBase::~QNodeCreatedChangeBase()
{
}

QNodeId QNodeCreatedChangeBase::parentId() const Q_DECL_NOEXCEPT
{
    Q_D(const QNodeCreatedChangeBase);
    return d->m_parentId;
}

const QMetaObject *QNodeCreatedChangeBase::metaObject() const Q_DECL_NOEXCEPT
{
    Q_D(const QNodeCreatedChangeBase);
    return d->m_metaObject;
}

bool QNodeCreatedChangeBase::isNodeEnabled() const Q_DECL_NOEXCEPT
{
    Q_D(const QNodeCreatedChangeBase);
    return d->m_nodeEnabled;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
