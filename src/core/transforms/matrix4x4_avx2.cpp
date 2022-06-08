// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "matrix4x4_avx2_p.h"

#ifdef QT_COMPILER_SUPPORTS_AVX2

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QDebug operator<<(QDebug dbg, const Matrix4x4_AVX2 &m)
{
    dbg.nospace() << "Matrix4x4_AVX2(" << Qt::endl
                  << qSetFieldWidth(10)
                  << m.m11() << m.m12() << m.m13() << m.m14() << Qt::endl
                  << m.m21() << m.m22() << m.m23() << m.m24() << Qt::endl
                  << m.m31() << m.m32() << m.m33() << m.m34() << Qt::endl
                  << m.m41() << m.m42() << m.m43() << m.m44() << Qt::endl
                  << qSetFieldWidth(0) << ')';
    return dbg;
}

} // Qt3DCore

QT_END_NAMESPACE

#endif
