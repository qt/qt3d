// Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRAY3D_H
#define QT3DRENDER_QRAY3D_H

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
#include <Qt3DCore/private/matrix4x4_p.h>
#include <Qt3DCore/private/vector3d_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

class Q_3DRENDERSHARED_EXPORT QRay3D
{
public:
    QRay3D();
    explicit QRay3D(const Vector3D &origin, const Vector3D &direction = Vector3D(0.0f, 0.0f, 1.0f), float distance = 1.0f);
    ~QRay3D();

    Vector3D origin() const;
    void setOrigin(const Vector3D &value);

    Vector3D direction() const;
    void setDirection(const Vector3D &value);

    float distance() const;
    void setDistance(float distance);

    bool contains(const Vector3D &point) const;
    bool contains(const QRay3D &ray) const;

    Vector3D point(float t) const;
    float projectedDistance(const Vector3D &point) const;

    Vector3D project(const Vector3D &vector) const;

    float distance(const Vector3D &point) const;

    QRay3D &transform(const Matrix4x4 &matrix);
    QRay3D transformed(const Matrix4x4 &matrix) const;

    bool operator==(const QRay3D &other) const;
    bool operator!=(const QRay3D &other) const;

    bool isValid() const { return !m_direction.isNull() && !qFuzzyIsNull(m_distance); }

private:
    Vector3D m_origin;
    Vector3D m_direction;
    float m_distance;
};
QT3D_DECLARE_TYPEINFO_2(Qt3DRender, RayCasting, QRay3D, Q_RELOCATABLE_TYPE)


#ifndef QT_NO_DEBUG_STREAM
Q_3DRENDERSHARED_EXPORT QDebug operator<<(QDebug dbg, const QRay3D &ray);
#endif

#ifndef QT_NO_DATASTREAM
Q_3DRENDERSHARED_EXPORT QDataStream &operator<<(QDataStream &stream, const QRay3D &ray);
Q_3DRENDERSHARED_EXPORT QDataStream &operator>>(QDataStream &stream, QRay3D &ray);
#endif

} // namespace RayCasting
} // namespace Qt3DRender
QT_END_NAMESPACE

inline bool qFuzzyCompare(const Qt3DRender::RayCasting::QRay3D &ray1, const Qt3DRender::RayCasting::QRay3D &ray2)
{
    return qFuzzyCompare(ray1.origin(), ray2.origin()) &&
           qFuzzyCompare(ray1.direction(), ray2.direction());
}

Q_DECLARE_METATYPE(Qt3DRender::RayCasting::QRay3D) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QRAY3D_H
