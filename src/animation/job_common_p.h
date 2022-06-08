// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DANIMATION_ANIMATION_JOB_COMMON_P_H
#define QT3DANIMATION_ANIMATION_JOB_COMMON_P_H

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

namespace Qt3DAnimation {

namespace Animation {

namespace JobTypes {

enum JobType {
    BuildBlendTree = 8192,
    EvaluateBlendClipAnimator,
    EvaluateClipAnimator,
    LoadAnimationClip,
    FindRunningClipAnimator
};

} // JobTypes

} // Animation

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_JOB_COMMON_P_H
