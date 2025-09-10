// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_JOB_COMMON_P_H
#define QT3DCORE_JOB_COMMON_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qaspectjob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace JobTypes {

    enum JobType {
        LoadBuffer = 4096,
        CalcBoundingVolume,
    };

} // JobTypes

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_JOB_COMMON_P_H
