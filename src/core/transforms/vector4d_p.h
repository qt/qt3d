// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_VECTOR4D_P_H
#define QT3DCORE_VECTOR4D_P_H

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

// Check if we can use the optimized version of QVector3D
#if defined(__SSE2__)

#include <Qt3DCore/private/vector4d_sse_p.h>

QT_BEGIN_NAMESPACE
using Vector4D = Qt3DCore::Vector4D_SSE;
QT_END_NAMESPACE

#else

#include <QVector4D>

QT_BEGIN_NAMESPACE
using Vector4D = QVector4D;
QT_END_NAMESPACE

#endif

template<typename UsingType>
Q_ALWAYS_INLINE QVector4D convertToQVector4D(const UsingType &v)
{
    return v.toQVector4D();
}

template<>
Q_ALWAYS_INLINE QVector4D convertToQVector4D<QVector4D>(const QVector4D &v)
{
    return v;
}

#endif // QT3DCORE_VECTOR4D_P_H
