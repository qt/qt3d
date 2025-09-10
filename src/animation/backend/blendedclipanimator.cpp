// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "blendedclipanimator_p.h"
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/qabstractclipblendnode.h>
#include <Qt3DAnimation/private/qblendedclipanimator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

BlendedClipAnimator::BlendedClipAnimator()
    : BackendNode(ReadWrite)
    , m_running(false)
    , m_lastGlobalTimeNS(0)
    , m_lastLocalTime(0.0)
    , m_currentLoop(0)
    , m_loops(1)
    , m_normalizedLocalTime(-1.0f)
    , m_lastNormalizedLocalTime(-1.0)
{
}

double BlendedClipAnimator::lastLocalTime() const
{
    return m_lastLocalTime;
}

void BlendedClipAnimator::setLastLocalTime(double lastLocalTime)
{
    m_lastLocalTime = lastLocalTime;
}

void BlendedClipAnimator::setLastNormalizedLocalTime(float normalizedTime)
{
    m_lastNormalizedLocalTime = normalizedTime;
}

void BlendedClipAnimator::setLastGlobalTimeNS(const qint64 &lastGlobalTimeNS)
{
    m_lastGlobalTimeNS = lastGlobalTimeNS;
}

qint64 BlendedClipAnimator::nsSincePreviousFrame(qint64 currentGlobalTimeNS)
{
    return currentGlobalTimeNS - m_lastGlobalTimeNS;
}

void BlendedClipAnimator::cleanup()
{
    setEnabled(false);
    m_handler = nullptr;
    m_blendTreeRootId = Qt3DCore::QNodeId();
    m_mapperId = Qt3DCore::QNodeId();
    m_clockId = Qt3DCore::QNodeId();
    m_running = false;
    m_lastGlobalTimeNS = 0;
    m_lastLocalTime = 0.0;
    m_currentLoop = 0;
    m_loops = 1;
}

void BlendedClipAnimator::setBlendTreeRootId(Qt3DCore::QNodeId blendTreeId)
{
    m_blendTreeRootId = blendTreeId;
    setDirty(Handler::BlendedClipAnimatorDirty);
}

void BlendedClipAnimator::setMapperId(Qt3DCore::QNodeId mapperId)
{
    m_mapperId = mapperId;
    setDirty(Handler::BlendedClipAnimatorDirty);
}

void BlendedClipAnimator::setClockId(Qt3DCore::QNodeId clockId)
{
    m_clockId = clockId;
    setDirty(Handler::BlendedClipAnimatorDirty);
}

void BlendedClipAnimator::setRunning(bool running)
{
    m_running = running;
    setDirty(Handler::BlendedClipAnimatorDirty);
}


Qt3DCore::QNodeId BlendedClipAnimator::blendTreeRootId() const
{
    return m_blendTreeRootId;
}

void BlendedClipAnimator::setNormalizedLocalTime(float normalizedTime, bool allowMarkDirty)
{
    m_normalizedLocalTime = normalizedTime;
    if (isValidNormalizedTime(m_normalizedLocalTime) && allowMarkDirty)
        setDirty(Handler::BlendedClipAnimatorDirty);
}

void BlendedClipAnimator::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QBlendedClipAnimator *node = qobject_cast<const QBlendedClipAnimator *>(frontEnd);
    if (!node)
        return;

    auto id = Qt3DCore::qIdForNode(node->blendTree());
    if (id != m_blendTreeRootId)
        setBlendTreeRootId(id);
    id = Qt3DCore::qIdForNode(node->channelMapper());
    if (m_mapperId != id)
        setMapperId(id);
    id = Qt3DCore::qIdForNode(node->clock());
    if (m_clockId != id)
        setClockId(id);

    if (m_running != node->isRunning())
        setRunning(node->isRunning());
    if (m_loops != node->loopCount())
        m_loops = node->loopCount();
    if (!qFuzzyCompare(m_normalizedLocalTime, node->normalizedTime()))
        setNormalizedLocalTime(node->normalizedTime());

    if (firstTime)
        setDirty(Handler::BlendedClipAnimatorDirty);
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
