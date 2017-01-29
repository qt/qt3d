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

#include "buildblendtreesjob_p.h"
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/clipblendnodevisitor_p.h>
#include <Qt3DAnimation/private/clipblendnode_p.h>
#include <Qt3DAnimation/private/lerpblend_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

BuildBlendTreesJob::BuildBlendTreesJob()
    : Qt3DCore::QAspectJob()
{
    // TO DO: Add Profiler ID
}

void BuildBlendTreesJob::setBlendedClipAnimators(const QVector<HBlendedClipAnimator> &blendedClipAnimatorHandles)
{
    m_blendedClipAnimatorHandles = blendedClipAnimatorHandles;
}

void BuildBlendTreesJob::run()
{
    for (const HBlendedClipAnimator blendedClipAnimatorHandle : m_blendedClipAnimatorHandles) {
        // Retrive BlendTree node
        BlendedClipAnimator *blendClipAnimator = m_handler->blendedClipAnimatorManager()->data(blendedClipAnimatorHandle);
        Q_ASSERT(blendClipAnimator);

        // TO DO: Add support for tree of blend nodes
        // For now assumes only one

        const bool canRun = blendClipAnimator->canRun();
        m_handler->setBlendedClipAnimatorRunning(blendedClipAnimatorHandle, canRun);

        // Check if blend clip can run and if so build blend tree
        if (canRun) {
            const ChannelMapper *mapper = m_handler->channelMapperManager()->lookupResource(blendClipAnimator->mapperId());
            Q_ASSERT(mapper);
            ClipBlendNode *node = m_handler->clipBlendNodeManager()->lookupNode(blendClipAnimator->blendTreeRootId());

            const Qt3DCore::QNodeIdVector clipIds = node->clipIds();
            // There must be 2 clips
            if (clipIds.size() != 2) {
                qWarning() << "A Blend Tree requires exactly 2 clips";
                return;
            }

            // Retrieve Animation clips
            const AnimationClip *clip1 = m_handler->animationClipManager()->lookupResource(clipIds.first());
            const AnimationClip *clip2 = m_handler->animationClipManager()->lookupResource(clipIds.last());

            // Build mappings for the 2 clips
            const QVector<AnimationUtils::MappingData> mappingDataClip1 = AnimationUtils::buildPropertyMappings(m_handler, clip1, mapper);
            const QVector<AnimationUtils::MappingData> mappingDataClip2 = AnimationUtils::buildPropertyMappings(m_handler, clip2, mapper);

            // We can only blend channels that are in both clips
            // If a channel is present in one clip and not the other, we use 100% of its value (no blending)
            QVector<AnimationUtils::BlendingMappingData> blendingMappingData;
            const int mappingInClip1Size = mappingDataClip1.size();
            blendingMappingData.reserve(mappingInClip1Size);

            // Find mappings that are in both vectors and build mappingData out of that
            for (const AnimationUtils::MappingData &mappingDataInClip1 : mappingDataClip1) {
                AnimationUtils::BlendingMappingData mappingData;
                mappingData.channelIndicesClip1 = mappingDataInClip1.channelIndices;
                mappingData.propertyName = mappingDataInClip1.propertyName;
                mappingData.targetId = mappingDataInClip1.targetId;
                mappingData.type = mappingDataInClip1.type;
                mappingData.blendAction = AnimationUtils::BlendingMappingData::NoBlending;
                blendingMappingData.push_back(mappingData);
            }

            for (const AnimationUtils::MappingData &mappingDataInClip2 : mappingDataClip2) {
                bool sharedChannel = false;
                for (int i = 0; i < mappingInClip1Size; ++i) {
                    AnimationUtils::BlendingMappingData &mappingDataInClip1 = blendingMappingData[i];
                    if ((strcmp(mappingDataInClip1.propertyName, mappingDataInClip2.propertyName) == 0) &&
                            mappingDataInClip1.targetId == mappingDataInClip2.targetId &&
                            mappingDataInClip1.type == mappingDataInClip2.type) {
                        // We have a channel shared in both clips
                        mappingDataInClip1.channelIndicesClip2 = mappingDataInClip2.channelIndices;
                        mappingDataInClip1.blendAction = AnimationUtils::BlendingMappingData::ClipBlending;
                        sharedChannel = true;
                        break;
                    }
                }
                if (!sharedChannel) { // We have a channel defined in only one of the clips
                    AnimationUtils::BlendingMappingData mappingData;
                    mappingData.channelIndicesClip2 = mappingDataInClip2.channelIndices;
                    mappingData.propertyName = mappingDataInClip2.propertyName;
                    mappingData.targetId = mappingDataInClip2.targetId;
                    mappingData.type = mappingDataInClip2.type;
                    mappingData.blendAction = AnimationUtils::BlendingMappingData::NoBlending;
                    blendingMappingData.push_back(mappingData);
                }
            }

            blendClipAnimator->setMappingData(blendingMappingData);
        }
    }
}

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
