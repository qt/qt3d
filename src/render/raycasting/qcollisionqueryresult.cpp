// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    m_hits.emplace_back(entity, intersection, distance, uvw);
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

const std::vector<QCollisionQueryResult::Hit> &QCollisionQueryResult::hits() const
{
    Q_D(const QCollisionQueryResult);
    return d->m_hits;
}

std::vector<Qt3DCore::QNodeId> QCollisionQueryResult::entitiesHit() const
{
    Q_D(const QCollisionQueryResult);
    std::vector<Qt3DCore::QNodeId> result;
    result.reserve(d->m_hits.size());
    for (const Hit& hit : d->m_hits)
        result.push_back(hit.m_entityId);
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
