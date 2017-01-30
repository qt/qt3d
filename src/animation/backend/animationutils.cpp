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

AnimationUtils::ClipPreEvaluationData AnimationUtils::evaluationDataForClip(AnimationClip *clip,
                                                                            const AnimationUtils::AnimatorEvaluationData &animatorData)
{
    // global time values expected in seconds
    ClipPreEvaluationData result;
    result.localTime = localTimeFromGlobalTime(animatorData.globalTime, animatorData.startTime,
                                               animatorData.playbackRate, clip->duration(),
                                               animatorData.loopCount, result.currentLoop);
    result.isFinalFrame = (result.localTime >= clip->duration() &&
                           animatorData.loopCount != 0 &&
                           result.currentLoop >= animatorData.loopCount - 1);
    return result;
}

double AnimationUtils::localTimeFromGlobalTime(double t_global,
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

QVector<int> AnimationUtils::channelsToIndices(const ChannelGroup &channelGroup, int dataType, int offset)
{
    static const QStringList standardSuffixes = (QStringList()
                                                 << QLatin1String("x")
                                                 << QLatin1String("y")
                                                 << QLatin1String("z")
                                                 << QLatin1String("w"));
    static const QStringList quaternionSuffixes = (QStringList()
                                                   << QLatin1String("w")
                                                   << QLatin1String("x")
                                                   << QLatin1String("y")
                                                   << QLatin1String("z"));

    if (dataType != QVariant::Quaternion)
        return channelsToIndicesHelper(channelGroup, dataType, offset, standardSuffixes);
    else
        return channelsToIndicesHelper(channelGroup, dataType, offset, quaternionSuffixes);

}

QVector<int> AnimationUtils::channelsToIndicesHelper(const ChannelGroup &channelGroup, int dataType, int offset, const QStringList &suffixes)
{
    int expectedChannelCount = 1;
    switch (dataType) {
    case QVariant::Double:
        expectedChannelCount = 1;
        break;

    case QVariant::Vector2D:
        expectedChannelCount = 2;
        break;

    case QVariant::Vector3D:
        expectedChannelCount = 3;
        break;

    case QVariant::Vector4D:
    case QVariant::Quaternion:
        expectedChannelCount = 4;
        break;

    default:
        qWarning() << "Unhandled animation type";
    }

    const int foundChannelCount = channelGroup.channels.size();
    if (foundChannelCount != expectedChannelCount) {
        qWarning() << "Data type expects" << expectedChannelCount
                   << "but found" << foundChannelCount << "channels in the animation clip";
    }

    QVector<int> indices(expectedChannelCount);
    for (int i = 0; i < expectedChannelCount; ++i) {
        int index = suffixes.indexOf(channelGroup.channels[i].name);
        if (index != -1)
            indices[i] = index + offset;
        else
            indices[i] = -1;
    }
    return indices;
}

QVector<float> AnimationUtils::evaluateClipAtLocalTime(AnimationClip *clip, float localTime)
{
    QVector<float> channelResults;
    Q_ASSERT(clip);

    // Ensure we have enough storage to hold the evaluations
    channelResults.resize(clip->channelCount());

    // Iterate over channels and evaluate the fcurves
    const QVector<ChannelGroup> &channelGroups = clip->channelGroups();
    int i = 0;
    for (const ChannelGroup &channelGroup : channelGroups) {
        for (const auto channel : qAsConst(channelGroup.channels))
            channelResults[i++] = channel.fcurve.evaluateAtTime(localTime);
    }
    return channelResults;
}

QVector<Qt3DCore::QSceneChangePtr> AnimationUtils::preparePropertyChanges(Qt3DCore::QNodeId peerId,
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
        Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(e.data())->m_isFinal = finalFrame;

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
        auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId);
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("running");
        e->setValue(false);
        Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(e.data())->m_isFinal = true;
        changes.push_back(e);
    }
    return changes;
}

QVector<AnimationUtils::MappingData> AnimationUtils::buildPropertyMappings(Handler *handler, const AnimationClip *clip, const ChannelMapper *mapper)
{
    QVector<MappingData> mappingDataVec;
    ChannelMappingManager *mappingManager = handler->channelMappingManager();
    const QVector<ChannelGroup> &channelGroups = clip->channelGroups();

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
        for (const ChannelGroup &channelGroup : channelGroups) {
            if (channelGroup.name == channelName) {
                foundMatch = true;
                const int channelBaseIndex = clip->channelBaseIndex(channelGroupIndex);

                // Within this group, match channel names with index ordering
                mappingData.channelIndices = channelsToIndices(channelGroup, mappingData.type, channelBaseIndex);

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


} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
