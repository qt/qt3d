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

#include "findrunningclipanimatorsjob_p.h"
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/animationlogging_p.h>
#include <Qt3DAnimation/private/animationutils_p.h>
#include <Qt3DAnimation/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

FindRunningClipAnimatorsJob::FindRunningClipAnimatorsJob()
    : Qt3DCore::QAspectJob()
    , m_handler(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::FindRunningClipAnimator, 0)
}

void FindRunningClipAnimatorsJob::setDirtyClipAnimators(const QVector<HClipAnimator> &clipAnimatorHandles)
{
    m_clipAnimatorHandles = clipAnimatorHandles;
}

void FindRunningClipAnimatorsJob::run()
{
    Q_ASSERT(m_handler);

    ClipAnimatorManager *clipAnimatorManager = m_handler->clipAnimatorManager();
    for (const auto &clipAnimatorHandle : qAsConst(m_clipAnimatorHandles)) {
        ClipAnimator *clipAnimator = clipAnimatorManager->data(clipAnimatorHandle);
        Q_ASSERT(clipAnimator);
        if (!clipAnimator->isEnabled())
            continue;

        const bool canRun = clipAnimator->canRun();
        const bool running = clipAnimator->isRunning();
        const bool seeking = clipAnimator->isSeeking();
        m_handler->setClipAnimatorRunning(clipAnimatorHandle, canRun && (seeking || running));

        // TODO: Actually check if this is needed first, currently we re-build this every time
        // canRun (or the normalized time) is true.
        if (!canRun || !(seeking || running))
            continue;

        // The clip animator needs to know how to map fcurve values through to properties on QNodes.
        // Now we know this animator can run, build the mapping table. Even though this could be
        // done a little simpler in the non-blended case, we follow the same code path as the
        // blended clip animator for consistency and ease of maintenance.
        const ChannelMapper *mapper = m_handler->channelMapperManager()->lookupResource(clipAnimator->mapperId());
        Q_ASSERT(mapper);
        const QVector<ChannelMapping *> channelMappings = mapper->mappings();

        const QVector<ChannelNameAndType> channelNamesAndTypes
                = buildRequiredChannelsAndTypes(m_handler, mapper);
        const QVector<ComponentIndices> channelComponentIndices
                = assignChannelComponentIndices(channelNamesAndTypes);

        const AnimationClip *clip = m_handler->animationClipLoaderManager()->lookupResource(clipAnimator->clipId());
        Q_ASSERT(clip);
        const ClipFormat format = generateClipFormatIndices(channelNamesAndTypes,
                                                            channelComponentIndices,
                                                            clip);
        clipAnimator->setClipFormat(format);

        const QVector<MappingData> mappingData = buildPropertyMappings(channelMappings,
                                                                       channelNamesAndTypes,
                                                                       format.formattedComponentIndices,
                                                                       format.sourceClipMask);
        clipAnimator->setMappingData(mappingData);
    }

    qCDebug(Jobs) << "Running clip animators =" << m_handler->runningClipAnimators();

    // Clear the handles to process ready for next frame
    m_clipAnimatorHandles.clear();
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
