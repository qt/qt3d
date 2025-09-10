// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_EVALUATEBLENDCLIPANIMATORJOB_P_H
#define QT3DANIMATION_ANIMATION_EVALUATEBLENDCLIPANIMATORJOB_P_H

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

#include <Qt3DAnimation/private/abstractevaluateclipanimatorjob_p.h>
#include <Qt3DAnimation/private/handle_types_p.h>
#include <Qt3DAnimation/private/animationutils_p.h>
#include <Qt3DAnimation/private/blendedclipanimator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Handler;
class ClipBlendNode;
class EvaluateBlendClipAnimatorJobPrivate;

class EvaluateBlendClipAnimatorJob : public AbstractEvaluateClipAnimatorJob
{
public:
    EvaluateBlendClipAnimatorJob();

    void setHandler(Handler *handler) { m_handler = handler; }
    Handler *handler() const { return m_handler; }

    void setBlendClipAnimator(const HBlendedClipAnimator &blendClipAnimatorHandle) { m_blendClipAnimatorHandle = blendClipAnimatorHandle; }
    HBlendedClipAnimator blendClipAnimator() const { return m_blendClipAnimatorHandle; }

protected:
    void run() override;

private:
    HBlendedClipAnimator m_blendClipAnimatorHandle;
    Handler *m_handler;
};

typedef QSharedPointer<EvaluateBlendClipAnimatorJob> EvaluateBlendClipAnimatorJobPtr;

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_EVALUATEBLENDCLIPANIMATORJOB_P_H
