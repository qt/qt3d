// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QCOLLISIONQUERYRESULT_P_H
#define QT3DRENDER_QCOLLISIONQUERYRESULT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/vector3d_p.h>
#include <QSharedData>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

typedef int QQueryHandle;
class QCollisionQueryResultPrivate;

class Q_3DRENDERSHARED_EXPORT QCollisionQueryResult
{
public:
    struct Hit {
        enum HitType {
            Entity,
            Point,
            Edge,
            Triangle
        };

        Hit()
            : m_type(Entity)
            , m_distance(-1.f)
            , m_primitiveIndex(0)
        {
            m_vertexIndex[0] = m_vertexIndex[1] = m_vertexIndex[2] = 0;
        }

        Hit(Qt3DCore::QNodeId entity, const Vector3D &intersection, float distance, const Vector3D &uvw)
            : m_entityId(entity)
            , m_type(Entity)
            , m_intersection(intersection)
            , m_distance(distance)
            , m_primitiveIndex(0U)
            , m_uvw(uvw)
        {
        }

        Qt3DCore::QNodeId m_entityId;
        HitType m_type;
        Vector3D m_intersection;
        float m_distance;
        uint m_primitiveIndex;
        uint m_vertexIndex[3];
        Vector3D m_uvw;
    };

    QCollisionQueryResult();
    QCollisionQueryResult(const QCollisionQueryResult &);
    ~QCollisionQueryResult();

    QCollisionQueryResult &operator=(const QCollisionQueryResult &);
#ifdef Q_COMPILER_RVALUE_REFS
    QCollisionQueryResult &operator=(QCollisionQueryResult &&other) noexcept
    {
        swap(other);
        return *this;
    }
#endif

    void swap(QCollisionQueryResult &other) noexcept
    {
        qSwap(d_ptr, other.d_ptr);
    }

    QQueryHandle handle() const;
    const std::vector<Hit> &hits() const;
    std::vector<Qt3DCore::QNodeId> entitiesHit() const;

private:
    friend class QAbstractCollisionQueryService;

    explicit QCollisionQueryResult(QCollisionQueryResultPrivate &p);

    QSharedDataPointer<QCollisionQueryResultPrivate> d_ptr;
    // Q_DECLARE_PRIVATE equivalent for shared data pointers
    QCollisionQueryResultPrivate *d_func();
    inline const QCollisionQueryResultPrivate *d_func() const
    {
        return d_ptr.constData();
    }
};
QT3D_DECLARE_TYPEINFO_2(Qt3DRender, RayCasting, QCollisionQueryResult::Hit, Q_PRIMITIVE_TYPE)
QT3D_DECLARE_SHARED_2(Qt3DRender, RayCasting, QCollisionQueryResult)

class QCollisionQueryResultPrivate : public QSharedData
{
public:
    explicit QCollisionQueryResultPrivate();
    explicit QCollisionQueryResultPrivate(const QCollisionQueryResultPrivate &copy);

    void setHandle(const QQueryHandle &handle);
    void addEntityHit(Qt3DCore::QNodeId entity, const Vector3D& intersection, float distance,
                      const Vector3D& uvw);

    QQueryHandle m_handle;
    std::vector<QCollisionQueryResult::Hit> m_hits;
};

inline bool operator==(const QCollisionQueryResult::Hit& left, const QCollisionQueryResult::Hit& right)
{
    return left.m_entityId == right.m_entityId;
}

} // RayCasting
} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QCOLLISIONQUERYRESULT_P_H
