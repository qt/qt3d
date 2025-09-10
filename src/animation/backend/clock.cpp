// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "clock_p.h"
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/private/qclock_p.h>
#include <Qt3DAnimation/private/animationutils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

Clock::Clock()
    : BackendNode(ReadOnly)
    , m_playbackRate(1.f)
{
}

void Clock::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QClock *node = qobject_cast<const QClock *>(frontEnd);
    if (!node)
        return;

    if (!qFuzzyCompare(m_playbackRate, node->playbackRate())) {
        m_playbackRate = node->playbackRate();
    }
}

void Clock::cleanup()
{
    m_playbackRate = 1.f;
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
