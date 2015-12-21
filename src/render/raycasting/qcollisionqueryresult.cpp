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

#include "qcollisionqueryresult_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QCollisionQueryResultPrivate::QCollisionQueryResultPrivate()
    : QSharedData()
{
}

QCollisionQueryResultPrivate::QCollisionQueryResultPrivate(const QCollisionQueryResultPrivate &copy)
    : QSharedData(copy)
    , m_handle(copy.m_handle)
    , m_entitiesHit(copy.m_entitiesHit)
{
}

void QCollisionQueryResultPrivate::addEntityHit(const Qt3DCore::QNodeId &entity)
{
    m_entitiesHit.append(entity);
}

void QCollisionQueryResultPrivate::setHandle(const QQueryHandle &handle)
{
    m_handle = handle;
}

QCollisionQueryResult::QCollisionQueryResult()
    : d_ptr(new QCollisionQueryResultPrivate())
{
}

QCollisionQueryResult::QCollisionQueryResult(const QCollisionQueryResult &result)
    : d_ptr(result.d_ptr)
{
}

QCollisionQueryResult::~QCollisionQueryResult()
{
}

QCollisionQueryResult &QCollisionQueryResult::operator=(const QCollisionQueryResult &result)
{
    d_ptr = result.d_ptr;
    return *this;
}

QVector<Qt3DCore::QNodeId> QCollisionQueryResult::entitiesHit() const
{
    Q_D(const QCollisionQueryResult);
    return d->m_entitiesHit;
}

/*!
    \internal
*/
QCollisionQueryResult::QCollisionQueryResult(QCollisionQueryResultPrivate &p)
    : d_ptr(&p)
{
}

/*!
    \internal
*/
QCollisionQueryResultPrivate *QCollisionQueryResult::d_func()
{
    return d_ptr.data();
}

QQueryHandle QCollisionQueryResult::handle() const
{
    Q_D(const QCollisionQueryResult);
    return d->m_handle;
}

} // Qt3DRender

QT_END_NAMESPACE
