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

#include "animationutils_p.h"
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/clipblendnode_p.h>
#include <Qt3DAnimation/private/clipblendnodevisitor_p.h>
#include <Qt3DAnimation/private/clipblendvalue_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qpropertyupdatedchangebase_p.h>
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qquaternion.h>
#include <QtCore/qvariant.h>
#include <Qt3DAnimation/private/animationlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ClipEvaluationData evaluationDataForClip(AnimationClipLoader *clip,
                                         const AnimatorEvaluationData &animatorData)
{
    // global time values expected in seconds
    ClipEvaluationData result;
    result.localTime = localTimeFromGlobalTime(animatorData.globalTime, animatorData.startTime,
                                               animatorData.playbackRate, clip->duration(),
                                               animatorData.loopCount, result.currentLoop);
    result.isFinalFrame = isFinalFrame(result.localTime, clip->duration(),
                                       result.currentLoop, animatorData.loopCount);
    return result;
}

double localTimeFromGlobalTime(double t_global,
                               double t_start_global,
                               double playbackRate,
                               double duration,
                               int loopCount,
                               int &currentLoop)
{
    double t_local = playbackRate * (t_global - t_start_global);
    double loopNumber = 0;
    if (loopCount == 1) {
        t_local = qBound(0.0, t_local, duration);
    } else if (loopCount == 0) {
        // Loops forever
        (void) std::modf(t_local / duration, &loopNumber);
        t_local = std::fmod(t_local, duration);
    } else {
        // N loops
        t_local = qBound(0.0, t_local, double(loopCount) * duration);
        (void) std::modf(t_local / duration, &loopNumber);
        t_local = std::fmod(t_local, duration);

        // Ensure we clamp to end of final loop
        if (loopNumber == loopCount) {
            loopNumber = loopCount - 1;
            t_local = duration;
        }
    }

    qCDebug(Jobs) << "t_global - t_start =" << t_global - t_start_global
                  << "current loop =" << loopNumber
                  << "t =" << t_local
                  << "duration =" << duration;

    currentLoop = loopNumber;

    return t_local;
}

double phaseFromGlobalTime(double t_global, double t_start_global,
                           double playbackRate, double duration,
                           int loopCount, int &currentLoop)
{
    const double t_local = localTimeFromGlobalTime(t_global, t_start_global, playbackRate,
                                                   duration, loopCount, currentLoop);
    return t_local / duration;
}

ComponentIndices channelComponentsToIndices(const Channel &channel, int dataType, int offset)
{
#if defined Q_COMPILER_UNIFORM_INIT
    static const QVector<char> standardSuffixes = { 'X', 'Y', 'Z', 'W' };
    static const QVector<char> quaternionSuffixes = { 'W', 'X', 'Y', 'Z' };
#else
    static const QVector<char> standardSuffixes = (QVector<char>() << 'X' << 'Y' << 'Z' << 'W');
    static const QVector<char> quaternionSuffixes = (QVector<char>() << 'W' << 'X' << 'Y' << 'Z');
#endif

    if (dataType != QVariant::Quaternion)
        return channelComponentsToIndicesHelper(channel, dataType, offset, standardSuffixes);
    else
        return channelComponentsToIndicesHelper(channel, dataType, offset, quaternionSuffixes);
}

ComponentIndices channelComponentsToIndicesHelper(const Channel &channel,
                                              int dataType,
                                              int offset,
                                              const QVector<char> &suffixes)
{
    int expectedComponentCount = 1;
    switch (dataType) {
    case QVariant::Double:
        expectedComponentCount = 1;
        break;

    case QVariant::Vector2D:
        expectedComponentCount = 2;
        break;

    case QVariant::Vector3D:
        expectedComponentCount = 3;
        break;

    case QVariant::Vector4D:
    case QVariant::Quaternion:
        expectedComponentCount = 4;
        break;

    default:
        qWarning() << "Unhandled animation type";
    }

    const int actualComponentCount = channel.channelComponents.size();
    if (actualComponentCount != expectedComponentCount) {
        qWarning() << "Data type expects" << expectedComponentCount
                   << "but found" << actualComponentCount << "components in the animation clip";
    }

    ComponentIndices indices(expectedComponentCount);
    for (int i = 0; i < expectedComponentCount; ++i) {
        const QString &componentName = channel.channelComponents[i].name;
        char suffix = componentName.at(componentName.length() - 1).toLatin1();
        int index = suffixes.indexOf(suffix);
        if (index != -1)
            indices[i] = index + offset;
        else
            indices[i] = -1;
    }
    return indices;
}

ClipResults evaluateClipAtLocalTime(AnimationClipLoader *clip, float localTime)
{
    QVector<float> channelResults;
    Q_ASSERT(clip);

    // Ensure we have enough storage to hold the evaluations
    channelResults.resize(clip->channelCount());

    // Iterate over channels and evaluate the fcurves
    const QVector<Channel> &channels = clip->channels();
    int i = 0;
    for (const Channel &channel : channels) {
        for (const auto channelComponent : qAsConst(channel.channelComponents))
            channelResults[i++] = channelComponent.fcurve.evaluateAtTime(localTime);
    }
    return channelResults;
}

ClipResults evaluateClipAtPhase(AnimationClipLoader *clip, float phase)
{
    // Calculate the clip local time from the phase and clip duration
    const double localTime = phase * clip->duration();
    return evaluateClipAtLocalTime(clip, localTime);
}

QVector<Qt3DCore::QSceneChangePtr> preparePropertyChanges(Qt3DCore::QNodeId animatorId,
                                                          const QVector<MappingData> &mappingDataVec,
                                                          const QVector<float> &channelResults,
                                                          bool finalFrame)
{
    QVector<Qt3DCore::QSceneChangePtr> changes;
    // Iterate over the mappings
    for (const MappingData &mappingData : mappingDataVec) {
        // Construct a property update change, set target, property and delivery options
        auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(mappingData.targetId);
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName(mappingData.propertyName);

        // Handle intermediate updates vs final flag properly
        Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(e.data())->m_isIntermediate = !finalFrame;

        // Build the new value from the channel/fcurve evaluation results
        QVariant v;
        switch (mappingData.type) {
        case QVariant::Double: {
            v = QVariant::fromValue(channelResults[mappingData.channelIndices[0]]);
            break;
        }

        case QVariant::Vector2D: {
            const QVector2D vector(channelResults[mappingData.channelIndices[0]],
                    channelResults[mappingData.channelIndices[1]]);
            v = QVariant::fromValue(vector);
            break;
        }

        case QVariant::Vector3D: {
            const QVector3D vector(channelResults[mappingData.channelIndices[0]],
                    channelResults[mappingData.channelIndices[1]],
                    channelResults[mappingData.channelIndices[2]]);
            v = QVariant::fromValue(vector);
            break;
        }

        case QVariant::Vector4D: {
            const QVector4D vector(channelResults[mappingData.channelIndices[0]],
                    channelResults[mappingData.channelIndices[1]],
                    channelResults[mappingData.channelIndices[2]],
                    channelResults[mappingData.channelIndices[3]]);
            v = QVariant::fromValue(vector);
            break;
        }

        case QVariant::Quaternion: {
            QQuaternion q(channelResults[mappingData.channelIndices[0]],
                    channelResults[mappingData.channelIndices[1]],
                    channelResults[mappingData.channelIndices[2]],
                    channelResults[mappingData.channelIndices[3]]);
            q.normalize();
            v = QVariant::fromValue(q);
            break;
        }

        default:
            qWarning() << "Unhandled animation type";
            continue;
        }

        // Assign new value and send
        e->setValue(v);
        changes.push_back(e);
    }


    // If it's the final frame, notify the frontend that we've stopped
    if (finalFrame) {
        auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(animatorId);
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("running");
        e->setValue(false);
        changes.push_back(e);
    }
    return changes;
}

QVector<MappingData> buildPropertyMappings(Handler *handler,
                                           const AnimationClipLoader *clip,
                                           const ChannelMapper *mapper)
{
    QVector<MappingData> mappingDataVec;
    ChannelMappingManager *mappingManager = handler->channelMappingManager();
    const QVector<Channel> &channels = clip->channels();

    // Iterate over the mappings in the mapper object
    for (const Qt3DCore::QNodeId mappingId : mapper->mappingIds()) {
        // Get the mapping object
        ChannelMapping *mapping = mappingManager->lookupResource(mappingId);
        Q_ASSERT(mapping);

        // Populate the data we need, easy stuff first
        MappingData mappingData;
        mappingData.targetId = mapping->targetId();
        mappingData.propertyName = mapping->propertyName();
        mappingData.type = mapping->type();

        if (mappingData.type == static_cast<int>(QVariant::Invalid)) {
            qWarning() << "Unknown type for node id =" << mappingData.targetId
                       << "and property =" << mapping->property();
            continue;
        }

        // Now the tricky part. Mapping the channel indices onto the property type.
        // Try to find a ChannelGroup with matching name
        const QString channelName = mapping->channelName();
        int channelGroupIndex = 0;
        bool foundMatch = false;
        for (const Channel &channel : channels) {
            if (channel.name == channelName) {
                foundMatch = true;
                const int channelBaseIndex = clip->channelComponentBaseIndex(channelGroupIndex);

                // Within this group, match channel names with index ordering
                mappingData.channelIndices = channelComponentsToIndices(channel, mappingData.type, channelBaseIndex);

                // Store the mapping data
                mappingDataVec.push_back(mappingData);

                if (foundMatch)
                    break;
            }

            ++channelGroupIndex;
        }
    }

    return mappingDataVec;
}

QVector<Qt3DCore::QNodeId> gatherValueNodesToEvaluate(Handler *handler,
                                                      Qt3DCore::QNodeId blendTreeRootId)
{
    Q_ASSERT(handler);
    Q_ASSERT(blendTreeRootId.isNull() == false);

    // We need the ClipBlendNodeManager to be able to lookup nodes from their Ids
    ClipBlendNodeManager *nodeManager = handler->clipBlendNodeManager();

    // Visit the tree in a pre-order manner and collect the dependencies
    QVector<Qt3DCore::QNodeId> clipIds;
    ClipBlendNodeVisitor visitor(nodeManager,
                                 ClipBlendNodeVisitor::PreOrder,
                                 ClipBlendNodeVisitor::VisitOnlyDependencies);

    auto func = [&clipIds, nodeManager] (ClipBlendNode *blendNode) {
        const auto dependencyIds = blendNode->dependencyIds();
        for (const auto dependencyId : dependencyIds) {
            // Look up the blend node and if it's a value type (clip),
            // add it to the set of value node ids that need to be evaluated
            ClipBlendNode *node = nodeManager->lookupNode(dependencyId);
            if (node && node->blendType() == ClipBlendNode::ValueType)
                clipIds.append(dependencyId);
        }
    };
    visitor.traverse(blendTreeRootId, func);

    // Sort and remove duplicates
    std::sort(clipIds.begin(), clipIds.end());
    auto last = std::unique(clipIds.begin(), clipIds.end());
    clipIds.erase(last, clipIds.end());
    return clipIds;
}

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
