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

#ifndef QT3DINPUT_INPUT_JOB_COMMON_P_H
#define QT3DINPUT_INPUT_JOB_COMMON_P_H

#include <Qt3DCore/private/qaspectjob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

namespace JobTypes {

    enum JobType {
        AssignKeyboardFocus = 1024,
        KeyEventDispatcher,
        MouseEventDispatcher,
        UpdateAxisAction,
        DeviceProxyLoading,
        AxisAccumulatorIntegration
    };

} // JobTypes

} // Input

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_JOB_COMMON_P_H
