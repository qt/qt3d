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
#include <Qt3DAnimation/private/lerpclipblend_p.h>
#include <Qt3DAnimation/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

BuildBlendTreesJob::BuildBlendTreesJob()
    : Qt3DCore::QAspectJob()
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::BuildBlendTree, 0);
}

void BuildBlendTreesJob::setBlendedClipAnimators(const QVector<HBlendedClipAnimator> &blendedClipAnimatorHandles)
{
    m_blendedClipAnimatorHandles = blendedClipAnimatorHandles;
}

namespace {

template<typename MappingDataType>
QVector<BlendingMappingData> buildBlendMappingDataForNode(const QVector<MappingDataType> &mappingInNode1,
                                                          const QVector<MappingDataType> &mappingInNode2)
{
    // We can only blend channels that are in both clips
    // If a channel is present in one clip and not the other, we use 100% of its value (no blending)
    QVector<BlendingMappingData> blendingMappingData;
    const int mappingInNode1Size = mappingInNode1.size();
    blendingMappingData.reserve(mappingInNode1Size);

    // Find mappings that are in both vectors and build mappingData out of that
    for (const MappingDataType &mappingDataInNode1 : mappingInNode1) {
        BlendingMappingData mappingData;
        mappingData.channelIndicesClip1 = mappingDataInNode1.channelIndices;
        mappingData.propertyName = mappingDataInNode1.propertyName;
        mappingData.targetId = mappingDataInNode1.targetId;
        mappingData.type = mappingDataInNode1.type;
        mappingData.blendAction = BlendingMappingData::NoBlending;
        blendingMappingData.push_back(mappingData);
    }

    for (const MappingDataType &mappingDataInNode2 : mappingInNode2) {
        bool sharedChannel = false;
        for (int i = 0; i < mappingInNode1Size; ++i) {
            BlendingMappingData &mappingDataInNode1 = blendingMappingData[i];
            if ((strcmp(mappingDataInNode1.propertyName, mappingDataInNode2.propertyName) == 0) &&
                    mappingDataInNode1.targetId == mappingDataInNode2.targetId &&
                    mappingDataInNode1.type == mappingDataInNode2.type) {
                // We have a channel shared in both clips
                mappingDataInNode1.channelIndicesClip2 = mappingDataInNode2.channelIndices;
                mappingDataInNode1.blendAction = BlendingMappingData::ClipBlending;
                sharedChannel = true;
                break;
            }
        }
        if (!sharedChannel) { // We have a channel defined in only one of the clips
            BlendingMappingData mappingData;
            mappingData.channelIndicesClip2 = mappingDataInNode2.channelIndices;
            mappingData.propertyName = mappingDataInNode2.propertyName;
            mappingData.targetId = mappingDataInNode2.targetId;
            mappingData.type = mappingDataInNode2.type;
            mappingData.blendAction = BlendingMappingData::NoBlending;
            blendingMappingData.push_back(mappingData);
        }
    }

    // Final indices (indices into the final blended result vector of floats for the node)
    int idx = 0;
    for (BlendingMappingData &mapping : blendingMappingData) {
        switch (mapping.blendAction) {
        case BlendingMappingData::ClipBlending: {
            Q_ASSERT(mapping.channelIndicesClip1.size() == mapping.channelIndicesClip2.size());
            for (int i = 0, m = mapping.channelIndicesClip1.size(); i < m; ++i)
                mapping.channelIndices.push_back(idx++);
            break;
        }
        case BlendingMappingData::NoBlending: {
            const bool useClip1 = !mapping.channelIndicesClip1.empty();
            const QVector<int> channelIndices = useClip1 ? mapping.channelIndicesClip1 : mapping.channelIndicesClip2;
            for (int i = 0, m = channelIndices.size(); i < m; ++i) {
                mapping.channelIndices.push_back(idx++);
            }
            break;
        }
        default:
            Q_UNREACHABLE();
            break;
        }
    }

    return blendingMappingData;
}

void buildEntryForBlendClipNode(Handler *handler, const ChannelMapper *mapper, BlendedClipAnimator::BlendNodeData &nodeData)
{
    // Retrieve Animation clips
    const AnimationClipLoader *clip1 = handler->animationClipLoaderManager()->lookupResource(nodeData.left);
    const AnimationClipLoader *clip2 = handler->animationClipLoaderManager()->lookupResource(nodeData.right);

    Q_ASSERT(clip1 && clip2);

    // Build mappings for the 2 clips
    const QVector<MappingData> mappingDataClip1 = buildPropertyMappings(handler, clip1, mapper);
    const QVector<MappingData> mappingDataClip2 = buildPropertyMappings(handler, clip2, mapper);

    // We can only blend channels that are in both clips
    // If a channel is present in one clip and not the other, we use 100% of its value (no blending)
    const QVector<BlendingMappingData> blendingMappingData = buildBlendMappingDataForNode(mappingDataClip1, mappingDataClip2);
    nodeData.mappingData = blendingMappingData;
}

void buildEntryForBlendNodeNode(BlendedClipAnimator::BlendNodeData &nodeData, const QHash<Qt3DCore::QNodeId, BlendedClipAnimator::BlendNodeData> &blendingNodeTable)
{
    const BlendedClipAnimator::BlendNodeData &node1Data = blendingNodeTable.value(nodeData.left);
    const BlendedClipAnimator::BlendNodeData &node2Data = blendingNodeTable.value(nodeData.right);

    // Build mappings for the 2 nodes
    const QVector<BlendingMappingData> mappingDataNode1 = node1Data.mappingData;
    const QVector<BlendingMappingData> mappingDataNode2 = node2Data.mappingData;

    // We can only blend channels that are in both clips
    // If a channel is present in one clip and not the other, we use 100% of its value (no blending)
    const QVector<BlendingMappingData> blendingMappingData = buildBlendMappingDataForNode(mappingDataNode1, mappingDataNode2);
    nodeData.mappingData = blendingMappingData;
}

} // anonymous

// Note this job is run once for all stopped blended animators that have been marked dirty
// We assume that the structure of blend node tree does not change once a BlendClipAnimator has been set to running
void BuildBlendTreesJob::run()
{
    for (const HBlendedClipAnimator blendedClipAnimatorHandle : m_blendedClipAnimatorHandles) {
        // Retrieve BlendTree node
        BlendedClipAnimator *blendClipAnimator = m_handler->blendedClipAnimatorManager()->data(blendedClipAnimatorHandle);
        Q_ASSERT(blendClipAnimator);

        const bool canRun = blendClipAnimator->canRun();
        m_handler->setBlendedClipAnimatorRunning(blendedClipAnimatorHandle, canRun);

        // Check if blend clip can run and if so build blend tree
        if (canRun) {
            const ChannelMapper *mapper = m_handler->channelMapperManager()->lookupResource(blendClipAnimator->mapperId());
            Q_ASSERT(mapper);

            ClipBlendNodeVisitor visitor(m_handler->clipBlendNodeManager());
            QHash<Qt3DCore::QNodeId, BlendedClipAnimator::BlendNodeData> blendingNodeTable;
            // Build a Binary Tree of BlendNodeData
            Handler *handler = m_handler; // For lambda capture
            visitor.traverse(blendClipAnimator->blendTreeRootId(), [&] (ClipBlendNode *node) {
                BlendedClipAnimator::BlendNodeData nodeData;
                nodeData.blendNodeId = node->peerId();
                nodeData.type = (node->childrenIds().size() > 0)
                        ? BlendedClipAnimator::BlendNodeData::BlendNodeType
                        : BlendedClipAnimator::BlendNodeData::ClipType;

                if (nodeData.type == BlendedClipAnimator::BlendNodeData::BlendNodeType) {
                    Q_ASSERT(node->childrenIds().size() == 2);
                    nodeData.left = node->childrenIds().first();
                    nodeData.right = node->childrenIds().last();
                    buildEntryForBlendNodeNode(nodeData, blendingNodeTable);
                } else { // ClipType
                    Q_ASSERT(node->clipIds().size() == 2);
                    nodeData.left = node->clipIds().first();
                    nodeData.right = node->clipIds().last();
                    buildEntryForBlendClipNode(handler, mapper, nodeData);
                }

                // Note: we assume that ClipType are always leave nodes
                // We will therefore perform blending on all the ClipType nodes
                // Then traverse the tree of BlendNodeType and merge the values
                blendingNodeTable.insert(nodeData.blendNodeId, nodeData);
            });

            blendClipAnimator->setBlendTreeTable(blendingNodeTable);
        }
    }
}

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
