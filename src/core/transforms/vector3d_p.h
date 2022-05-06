/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
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
******************************************************************************/

#ifndef QT3DCORE_VECTOR3D_P_H
#define QT3DCORE_VECTOR3D_P_H

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

// We check if sse config option was enabled as it could
// be disabled even though a given platform supports SSE2 instructions
#if QT_CONFIG(qt3d_simd_sse2) && (defined(__AVX2__) || defined(__SSE2__)) && defined(QT_COMPILER_SUPPORTS_SSE2)

#include <Qt3DCore/private/vector3d_sse_p.h>

QT_BEGIN_NAMESPACE
using Vector3D = Qt3DCore::Vector3D_SSE;
QT_END_NAMESPACE

#else

#include <QVector3D>

QT_BEGIN_NAMESPACE
using Vector3D = QVector3D;
QT_END_NAMESPACE

#endif

template<typename UsingType>
Q_ALWAYS_INLINE QVector3D convertToQVector3D(const UsingType &v)
{
    return v.toQVector3D();
}

template<>
Q_ALWAYS_INLINE QVector3D convertToQVector3D<QVector3D>(const QVector3D &v)
{
    return v;
}

#endif // QT3DCORE_VECTOR3D_P_H
