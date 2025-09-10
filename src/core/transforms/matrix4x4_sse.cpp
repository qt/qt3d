// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "matrix4x4_sse_p.h"

#ifdef __SSE2__

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QDebug operator<<(QDebug dbg, const Matrix4x4_SSE &m)
{
    dbg.nospace() << "Matrix4x4_SSE(" << Qt::endl
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

#endif // __SSE2__
