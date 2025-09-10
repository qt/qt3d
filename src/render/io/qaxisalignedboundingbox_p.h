// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QAXISALIGNEDBOUNDINGBOX_P_H
#define QT3DRENDER_QAXISALIGNEDBOUNDINGBOX_P_H

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

#include <vector>
#include <QMatrix4x4>
#include <QVector3D>

#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

class QDebug;

namespace Qt3DRender {

class QAxisAlignedBoundingBox
{
public:
    inline QAxisAlignedBoundingBox()
        : m_center()
        , m_radii()
    {}

    inline explicit QAxisAlignedBoundingBox(const std::vector<QVector3D> &points)
        : m_center()
        , m_radii()
    {
        update(points);
    }

    void clear()
    {
        m_center = QVector3D();
        m_radii = QVector3D();
    }

    bool isNull() const { return m_center.isNull() && m_radii.isNull(); }

    void Q_3DRENDERSHARED_PRIVATE_EXPORT update(const std::vector<QVector3D> &points);

    inline QVector3D center() const { return m_center; }
    inline QVector3D radii() const { return m_radii; }

    inline QVector3D minPoint() const { return m_center - m_radii; }
    inline QVector3D maxPoint() const { return m_center + m_radii; }

    inline float xExtent() const { return 2.0f * m_radii.x(); }
    inline float yExtent() const { return 2.0f * m_radii.y(); }
    inline float zExtent() const { return 2.0f * m_radii.z(); }

    inline float maxExtent() const { return qMax( xExtent(), qMax( yExtent(), zExtent() ) ); }
    inline float minExtent() const { return qMin( xExtent(), qMin( yExtent(), zExtent() ) ); }

    inline bool contains( const QVector3D& pt ) const
    {
        QVector3D minP(minPoint()), maxP(maxPoint());
        if ((pt.x() < minP.x()) || (pt.x() > maxP.x()) ||
            (pt.y() < minP.y()) || (pt.y() > maxP.y()) ||
            (pt.z() < minP.z()) || (pt.z() > maxP.z()) )
        {
            return false;
        }
        return true;
    }

    inline void expandToContain(const QVector3D &pt)
    {
        update(std::vector<QVector3D> { pt });
    }

    inline void expandToContain(const QAxisAlignedBoundingBox &other)
    {
        update(std::vector<QVector3D> { other.minPoint(), other.maxPoint() });
    }

    inline QAxisAlignedBoundingBox transformBy(const QMatrix4x4 &mat) const
    {
        QAxisAlignedBoundingBox r;
        r.m_center = mat.map(m_center);
        r.m_radii = mat.map(m_radii);
        return r;
    }

    inline QAxisAlignedBoundingBox &transform(const QMatrix4x4 &mat)
    {
        m_center = mat.map(m_center);
        m_radii = mat.map(m_radii);
        return *this;
    }

private:
    QVector3D m_center;
    QVector3D m_radii;

    friend bool intersects(const QAxisAlignedBoundingBox &a,
                           const QAxisAlignedBoundingBox &b);
};

QDebug operator<<(QDebug dbg, const QAxisAlignedBoundingBox &c);

inline bool intersects(const QAxisAlignedBoundingBox &a,
                       const QAxisAlignedBoundingBox &b)
{
    // Test y axis last as this is often the least discriminatory in OpenGL applications
    // where worlds tend to be mostly in the xz-plane
    if (qAbs(a.m_center[0] - b.m_center[0]) > a.m_radii[0] + b.m_radii[0])
        return false;
    if (qAbs(a.m_center[2] - b.m_center[2]) > a.m_radii[2] + b.m_radii[2])
        return false;
    if (qAbs(a.m_center[1] - b.m_center[1]) > a.m_radii[1] + b.m_radii[1])
        return false;
    return true;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QAXISALIGNEDBOUNDINGBOX_P_H
