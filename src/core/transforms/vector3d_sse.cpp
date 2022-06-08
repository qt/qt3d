// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/qsimd_p.h>

#if defined(__AVX2__) && defined(QT_COMPILER_SUPPORTS_AVX2)
#include "matrix4x4_avx2_p.h"
#else
#include "matrix4x4_sse_p.h"
#endif

#include "vector3d_sse_p.h"
#include "vector4d_sse_p.h"
#include <QDebug>

#ifdef QT_COMPILER_SUPPORTS_SSE2

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QDebug operator<<(QDebug dbg, const Vector3D_SSE &v)
{
    dbg.nospace() << "Vector3D_SSE(" << v.x() << ", " << v.y() << ", " << v.z() << ") ";
    return dbg;
}

Vector3D_SSE::Vector3D_SSE(const Vector4D_SSE &v)
{
    m_xyzw = _mm_mul_ps(v.m_xyzw, _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f));
}

#if defined(__AVX2__) && defined(QT_COMPILER_SUPPORTS_AVX2)

Vector3D_SSE Vector3D_SSE::unproject(const Matrix4x4_AVX2 &modelView, const Matrix4x4_AVX2 &projection, const QRect &viewport) const
{
    const Matrix4x4_AVX2 inverse = (projection * modelView).inverted();

    Vector4D_SSE tmp(*this, 1.0f);
    tmp.setX((tmp.x() - float(viewport.x())) / float(viewport.width()));
    tmp.setY((tmp.y() - float(viewport.y())) / float(viewport.height()));
    tmp = tmp * 2.0f - Vector4D_SSE(1.0f, 1.0f, 1.0f, 1.0f);

    Vector4D_SSE obj = inverse * tmp;
    if (qFuzzyIsNull(obj.w()))
        obj.setW(1.0f);
    obj /= obj.w();
    return Vector3D_SSE(obj);
}

Vector3D_SSE Vector3D_SSE::project(const Matrix4x4_AVX2 &modelView, const Matrix4x4_AVX2 &projection, const QRect &viewport) const
{
    Vector4D_SSE tmp(*this, 1.0f);
    tmp = projection * modelView * tmp;
    if (qFuzzyIsNull(tmp.w()))
        tmp.setW(1.0f);
    tmp /= tmp.w();

    tmp = tmp * 0.5f + Vector4D_SSE(0.5f, 0.5f, 0.5f, 0.5f);
    tmp.setX(tmp.x() * viewport.width() + viewport.x());
    tmp.setY(tmp.y() * viewport.height() + viewport.y());

    return Vector3D_SSE(tmp);
}

#else

Vector3D_SSE Vector3D_SSE::unproject(const Matrix4x4_SSE &modelView, const Matrix4x4_SSE &projection, const QRect &viewport) const
{
    const Matrix4x4_SSE inverse = (projection * modelView).inverted();

    Vector4D_SSE tmp(*this, 1.0f);
    tmp.setX((tmp.x() - float(viewport.x())) / float(viewport.width()));
    tmp.setY((tmp.y() - float(viewport.y())) / float(viewport.height()));
    tmp = tmp * 2.0f - Vector4D_SSE(1.0f, 1.0f, 1.0f, 1.0f);

    Vector4D_SSE obj = inverse * tmp;
    if (qFuzzyIsNull(obj.w()))
        obj.setW(1.0f);
    obj /= obj.w();
    return Vector3D_SSE(obj);
}

Vector3D_SSE Vector3D_SSE::project(const Matrix4x4_SSE &modelView, const Matrix4x4_SSE &projection, const QRect &viewport) const
{
    Vector4D_SSE tmp(*this, 1.0f);
    tmp = projection * modelView * tmp;
    if (qFuzzyIsNull(tmp.w()))
        tmp.setW(1.0f);
    tmp /= tmp.w();

    tmp = tmp * 0.5f + Vector4D_SSE(0.5f, 0.5f, 0.5f, 0.5f);
    tmp.setX(tmp.x() * viewport.width() + viewport.x());
    tmp.setY(tmp.y() * viewport.height() + viewport.y());

    return Vector3D_SSE(tmp);
}

#endif

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT_COMPILER_SUPPORTS_SSE2
