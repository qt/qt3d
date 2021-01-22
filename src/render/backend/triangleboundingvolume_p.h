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

#ifndef QT3DRENDER_RENDER_TRIANGLEBOUNDINGVOLUME_P_H
#define QT3DRENDER_RENDER_TRIANGLEBOUNDINGVOLUME_P_H

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

#include <Qt3DRender/private/qboundingvolume_p.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/matrix4x4_p.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

Q_AUTOTEST_EXPORT bool intersectsSegmentTriangle(const RayCasting::QRay3D &ray,
                                                 const Vector3D &a,
                                                 const Vector3D &b,
                                                 const Vector3D &c,
                                                 Vector3D &uvw,
                                                 float &t);

class Q_AUTOTEST_EXPORT TriangleBoundingVolume : public RayCasting::QBoundingVolume
{
public:
    TriangleBoundingVolume();
    explicit TriangleBoundingVolume(Qt3DCore::QNodeId id,
                                    const Vector3D &a,
                                    const Vector3D &b,
                                    const Vector3D &c);

    Qt3DCore::QNodeId id() const final;
    bool intersects(const RayCasting::QRay3D &ray, Vector3D *q, Vector3D *uvw) const final;
    Type type() const final;

    Vector3D a() const;
    Vector3D b() const;
    Vector3D c() const;

    void setA(const Vector3D &a);
    void setB(const Vector3D &b);
    void setC(const Vector3D &c);

    TriangleBoundingVolume transformed(const Matrix4x4 &mat) const;

    inline TriangleBoundingVolume &transform(const Matrix4x4 &mat)
    {
        *this = transformed(mat);
        return *this;
    }

private:
    Qt3DCore::QNodeId m_id;
    Vector3D m_a, m_b, m_c;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Render::TriangleBoundingVolume*) // LCOV_EXCL_LINE

#endif // QT3DRENDER_RENDER_TRIANGLEBOUNDINGVOLUME_P_H
