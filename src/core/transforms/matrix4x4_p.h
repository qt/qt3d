// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_MATRIX4X4_P_H
#define QT3DCORE_MATRIX4X4_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qsimd_p.h>
#include <Qt3DCore/private/qt3dcore-config_p.h>


// We check if sse or avx config option was enabled as it could
// be disabled even though a given platform supports SSE2 or AVX2 instructions
#if QT_CONFIG(qt3d_simd_avx2) && defined(__AVX2__) && defined(QT_COMPILER_SUPPORTS_AVX2)

#include <Qt3DCore/private/matrix4x4_avx2_p.h>

QT_BEGIN_NAMESPACE
using Matrix4x4 = Qt3DCore::Matrix4x4_AVX2;
QT_END_NAMESPACE

#elif QT_CONFIG(qt3d_simd_sse2) && defined(__SSE2__) && defined(QT_COMPILER_SUPPORTS_SSE2)

#include <Qt3DCore/private/matrix4x4_sse_p.h>

QT_BEGIN_NAMESPACE
using Matrix4x4 = Qt3DCore::Matrix4x4_SSE;
QT_END_NAMESPACE

#else

#include <QMatrix4x4>

QT_BEGIN_NAMESPACE
using Matrix4x4 = QMatrix4x4;
QT_END_NAMESPACE

#endif

template<typename UsingType>
Q_ALWAYS_INLINE QMatrix4x4 convertToQMatrix4x4(const UsingType &v)
{
    return v.toQMatrix4x4();
}

template<>
Q_ALWAYS_INLINE QMatrix4x4 convertToQMatrix4x4<QMatrix4x4>(const QMatrix4x4 &v)
{
    return v;
}

#endif // QT3DCORE_MATRIX4X4_P_H
