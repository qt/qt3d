// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_ABSTRACTEVALUATECLIPANIMATORJOB_P_H
#define QT3DANIMATION_ANIMATION_ABSTRACTEVALUATECLIPANIMATORJOB_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DAnimation/private/animationutils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class AbstractEvaluateClipAnimatorJobPrivate;

class AbstractEvaluateClipAnimatorJob : public Qt3DCore::QAspectJob
{
protected:
    AbstractEvaluateClipAnimatorJob();

    void setPostFrameData(const AnimationRecord &record, const QVector<AnimationCallbackAndValue> &callbacks);

private:
    Q_DECLARE_PRIVATE(AbstractEvaluateClipAnimatorJob)
};

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_ABSTRACTEVALUATECLIPANIMATORJOB_P_H
