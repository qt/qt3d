/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "clipanimator_p.h"
#include <Qt3DAnimation/qclipanimator.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/private/qclipanimator_p.h>
#include <Qt3DAnimation/private/animationclip_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/animationlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ClipAnimator::ClipAnimator()
    : BackendNode(ReadWrite)
    , m_clipId()
    , m_mapperId()
    , m_clockId()
    , m_running(false)
    , m_loops(1)
    , m_lastGlobalTimeNS(0)
    , m_lastLocalTime(0.0)
    , m_mappingData()
    , m_currentLoop(0)
    , m_normalizedLocalTime(-1.0f)
    , m_lastNormalizedLocalTime(-1.0f)
{
}

void ClipAnimator::setClipId(Qt3DCore::QNodeId clipId)
{
    m_clipId = clipId;
    setDirty(Handler::ClipAnimatorDirty);

    // register at the clip to make sure we are marked dirty when the clip finished loading
    AnimationClip *clip = m_handler->animationClipLoaderManager()->lookupResource(clipId);
    if (clip)
        clip->addDependingClipAnimator(peerId());
}

void ClipAnimator::setMapperId(Qt3DCore::QNodeId mapperId)
{
    m_mapperId = mapperId;
    setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::setClockId(Qt3DCore::QNodeId clockId)
{
    m_clockId = clockId;
    setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::setRunning(bool running)
{
    m_running = running;
    if (!running)
        m_currentLoop = 0;
    setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::setNormalizedLocalTime(float normalizedTime, bool allowMarkDirty)
{
    m_normalizedLocalTime = normalizedTime;
    if (isValidNormalizedTime(normalizedTime) && allowMarkDirty)
        setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::cleanup()
{
    setEnabled(false);
    m_handler = nullptr;
    m_clipId = Qt3DCore::QNodeId();
    m_mapperId = Qt3DCore::QNodeId();
    m_clockId = Qt3DCore::QNodeId();
    m_running = false;
    m_loops = 1;
    m_clipFormat = ClipFormat();
    m_normalizedLocalTime = m_lastNormalizedLocalTime = -1.0f;
}

void ClipAnimator::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QClipAnimator *node = qobject_cast<const QClipAnimator *>(frontEnd);
    if (!node)
        return;

    auto id = Qt3DCore::qIdForNode(node->clip());
    if (m_clipId != id)
        setClipId(id);
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
        setDirty(Handler::ClipAnimatorDirty);
}

qint64 ClipAnimator::nsSincePreviousFrame(qint64 currentGlobalTimeNS)
{
    return currentGlobalTimeNS - m_lastGlobalTimeNS;
}

void ClipAnimator::setLastGlobalTimeNS(qint64 lastGlobalTimeNS)
{
    m_lastGlobalTimeNS = lastGlobalTimeNS;
}

double ClipAnimator::lastLocalTime() const
{
    return m_lastLocalTime;
}

void ClipAnimator::setLastLocalTime(double lastLocalTime)
{
    m_lastLocalTime = lastLocalTime;
}

void ClipAnimator::setLastNormalizedLocalTime(float normalizedTime)
{
    m_lastNormalizedLocalTime = normalizedTime;
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
