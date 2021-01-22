/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qcollisionqueryresult_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

QCollisionQueryResultPrivate::QCollisionQueryResultPrivate()
    : QSharedData()
    , m_handle(0)
{
}

QCollisionQueryResultPrivate::QCollisionQueryResultPrivate(const QCollisionQueryResultPrivate &copy)
    : QSharedData(copy)
    , m_handle(copy.m_handle)
    , m_hits(copy.m_hits)
{
}

void QCollisionQueryResultPrivate::addEntityHit(Qt3DCore::QNodeId entity, const Vector3D& intersection,
                                                float distance, const Vector3D& uvw)
{
    m_hits.append(QCollisionQueryResult::Hit(entity, intersection, distance, uvw));
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

QVector<QCollisionQueryResult::Hit> QCollisionQueryResult::hits() const
{
    Q_D(const QCollisionQueryResult);
    return d->m_hits;
}

QVector<Qt3DCore::QNodeId> QCollisionQueryResult::entitiesHit() const
{
    Q_D(const QCollisionQueryResult);
    QVector<Qt3DCore::QNodeId> result;
    for (const Hit& hit : d->m_hits)
        result << hit.m_entityId;
    return result;
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

} // RayCasting
} // Qt3DRender

QT_END_NAMESPACE
