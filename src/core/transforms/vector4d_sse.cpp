// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "vector4d_sse_p.h"
#include <QDebug>

#ifdef __SSE2__

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QDebug operator<<(QDebug dbg, const Vector4D_SSE &v)
{
    dbg.nospace() << "Vector4D_SSE(" << v.x() << ", " << v.y() << ", " << v.z() << ", " << v.w() << ") ";
    return dbg;
}

} // Qt3DCore

QT_END_NAMESPACE

#endif // __SSE2__
