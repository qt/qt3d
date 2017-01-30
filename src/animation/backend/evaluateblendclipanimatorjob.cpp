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

#include "evaluateblendclipanimatorjob_p.h"
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/animationlogging_p.h>
#include <Qt3DAnimation/private/animationutils_p.h>
#include <Qt3DAnimation/private/lerpblend_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

EvaluateBlendClipAnimatorJob::EvaluateBlendClipAnimatorJob()
    : Qt3DCore::QAspectJob()
{
    // TO DO: Add Profiler ID
}

void EvaluateBlendClipAnimatorJob::run()
{
    const qint64 globalTime = m_handler->simulationTime();

    BlendedClipAnimator *blendedClipAnimator = m_handler->blendedClipAnimatorManager()->data(m_blendClipAnimatorHandle);
    Q_ASSERT(blendedClipAnimator);

    // TO DO: Right now we are doing LERP but refactor to handle other types
    ClipBlendNode *blendNode = m_handler->clipBlendNodeManager()->lookupNode(blendedClipAnimator->blendTreeRootId());

    const Qt3DCore::QNodeIdVector clipIds = blendNode->clipIds();

    // Evaluate the fcurves for both clip
    AnimationClip *clip1 = m_handler->animationClipManager()->lookupResource(clipIds.first());
    AnimationClip *clip2 = m_handler->animationClipManager()->lookupResource(clipIds.last());
    Q_ASSERT(clip1 && clip2);

    // Prepare for evaluation (convert global time to local time ....)
    const AnimationUtils::AnimatorEvaluationData animatorEvaluationData = AnimationUtils::animatorEvaluationDataForAnimator(blendedClipAnimator, globalTime);
    const AnimationUtils::ClipPreEvaluationData preEvaluationDataForClip1 = AnimationUtils::evaluationDataForClip(clip1, animatorEvaluationData);
    const AnimationUtils::ClipPreEvaluationData preEvaluationDataForClip2 = AnimationUtils::evaluationDataForClip(clip2, animatorEvaluationData);

    // Evaluate the clips
    const QVector<float> channelResultsClip1 = AnimationUtils::evaluateClipAtLocalTime(clip1, preEvaluationDataForClip1.localTime);
    const QVector<float> channelResultsClip2 = AnimationUtils::evaluateClipAtLocalTime(clip2, preEvaluationDataForClip2.localTime);

    // Update loops and running of the animator
    blendedClipAnimator->setCurrentLoop(std::min(preEvaluationDataForClip1.currentLoop, preEvaluationDataForClip2.currentLoop));
    const bool isFinalFrame = preEvaluationDataForClip1.isFinalFrame && preEvaluationDataForClip2.isFinalFrame;
    if (isFinalFrame)
        blendedClipAnimator->setRunning(false);

    // Perform blending between the two clips
    QVector<AnimationUtils::MappingData> blendedMappingData;
    QVector<float> blendedValues;

    // Build a combined vector of blended value
    const QVector<AnimationUtils::BlendingMappingData> blendingMappingData = blendedClipAnimator->mappingData();
    for (const AnimationUtils::BlendingMappingData &mapping : blendingMappingData) {

        AnimationUtils::MappingData finalMapping;
        finalMapping.type = mapping.type;
        finalMapping.targetId = mapping.targetId;
        finalMapping.propertyName = mapping.propertyName;

        switch (mapping.blendAction) {
        case AnimationUtils::BlendingMappingData::ClipBlending: {
            Q_ASSERT(mapping.channelIndicesClip1.size() == mapping.channelIndicesClip2.size());
            for (int i = 0, m = mapping.channelIndicesClip1.size(); i < m; ++i) {
                const float value1 = channelResultsClip1.at(mapping.channelIndicesClip1[i]);
                const float value2 = channelResultsClip2.at(mapping.channelIndicesClip2[i]);
                const float blendedValue = blendNode->blend(value1, value2);
                finalMapping.channelIndices.push_back(blendedValues.size());
                blendedValues.push_back(blendedValue);
            }
            break;
        }
        case AnimationUtils::BlendingMappingData::NoBlending: {
            const bool useClip1 = !mapping.channelIndicesClip1.empty();
            const QVector<int> channelIndices = useClip1 ? mapping.channelIndicesClip1 : mapping.channelIndicesClip2;
            const QVector<float> values = useClip1 ? channelResultsClip1 : channelResultsClip2;
            for (int i = 0, m = channelIndices.size(); i < m; ++i) {
                const float value = values.at(channelIndices[i]);
                finalMapping.channelIndices.push_back(blendedValues.size());
                blendedValues.push_back(value);
            }
            break;
        }
        default:
            Q_UNREACHABLE();
            break;
        }

        blendedMappingData.push_back(finalMapping);
    }

    // Prepare property changes (if finalFrame it also prepares the change for the running property for the frontend)
    const QVector<Qt3DCore::QSceneChangePtr> changes = AnimationUtils::preparePropertyChanges(blendedClipAnimator->peerId(),
                                                                                              blendedMappingData,
                                                                                              blendedValues,
                                                                                              isFinalFrame);

    // Send the property changes
    blendedClipAnimator->sendPropertyChanges(changes);
}


} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
