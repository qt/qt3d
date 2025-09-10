// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_EVALUATECLIPANIMATORJOB_P_H
#define QT3DANIMATION_ANIMATION_EVALUATECLIPANIMATORJOB_P_H

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

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Handler;

class EvaluateClipAnimatorJob : public AbstractEvaluateClipAnimatorJob
{
public:
    EvaluateClipAnimatorJob();

    void setHandler(Handler *handler) { m_handler = handler; }
    Handler *handler() const { return m_handler; }

    void setClipAnimator(const HClipAnimator &clipAnimatorHandle)
    {
        m_clipAnimatorHandle = clipAnimatorHandle;
    }

    void clearClipAnimator()
    {
        m_clipAnimatorHandle = HClipAnimator();
    }

protected:
    void run() override;

private:
    HClipAnimator m_clipAnimatorHandle;
    Handler *m_handler;
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_EVALUATECLIPANIMATORJOB_P_H
