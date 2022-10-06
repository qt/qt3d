// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    for (const auto &clipAnimatorHandle : std::as_const(m_clipAnimatorHandles)) {
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
