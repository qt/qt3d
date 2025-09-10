// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QT3DLOGIC_LOGIC_JOB_COMMON_P_H
#define QT3DLOGIC_LOGIC_JOB_COMMON_P_H

#include <Qt3DCore/private/qaspectjob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {

namespace Logic {

namespace JobTypes {

    enum JobType {
        Callback = 2048
    };

} // JobTypes

} // Logic

} // Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_JOB_COMMON_P_H
