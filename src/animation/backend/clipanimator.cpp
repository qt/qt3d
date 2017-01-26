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
#include <Qt3DAnimation/private/qclipanimator_p.h>
#include <Qt3DAnimation/private/animationclip_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qpropertyupdatedchangebase_p.h>

#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qquaternion.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ClipAnimator::ClipAnimator()
    : BackendNode(ReadWrite)
    , m_clipId()
    , m_mapperId()
    , m_running(false)
    , m_startGlobalTime(0)
    , m_channelResults()
    , m_mappingData()
    , m_currentLoop(0)
    , m_finalFrame(false)
{
}

void ClipAnimator::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QClipAnimatorData>>(change);
    const auto &data = typedChange->data;
    m_clipId = data.clipId;
    m_mapperId = data.mapperId;
    m_running = data.running;
    setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::setClipId(Qt3DCore::QNodeId clipId)
{
    m_clipId = clipId;
    setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::setMapperId(Qt3DCore::QNodeId mapperId)
{
    m_mapperId = mapperId;
    setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::setRunning(bool running)
{
    m_running = running;
    if (!running) {
        m_finalFrame = false;
        m_currentLoop = 0;
    }
    setDirty(Handler::ClipAnimatorDirty);
}

void ClipAnimator::cleanup()
{
    setEnabled(false);
    m_handler = nullptr;
    m_clipId = Qt3DCore::QNodeId();
    m_mapperId = Qt3DCore::QNodeId();
    m_running = false;
}

void ClipAnimator::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    switch (e->type()) {
    case Qt3DCore::PropertyUpdated: {
        const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("clip"))
            setClipId(change->value().value<Qt3DCore::QNodeId>());
        else if (change->propertyName() == QByteArrayLiteral("channelMapper"))
            setMapperId(change->value().value<Qt3DCore::QNodeId>());
        else if (change->propertyName() == QByteArrayLiteral("running"))
            setRunning(change->value().toBool());
        break;
    }

    default:
        break;
    }
    QBackendNode::sceneChangeEvent(e);
}

void ClipAnimator::buildPropertyMappings()
{
    m_mappingData.clear();

    ChannelMappingManager *mappingManager = m_handler->channelMappingManager();
    AnimationClip *clip = m_handler->animationClipManager()->getOrCreateResource(m_clipId);
    Q_ASSERT(clip);
    const QVector<ChannelGroup> &channelGroups = clip->channelGroups();

    // Iterate over the mappings in the mapper object
    const ChannelMapper *mapper = m_handler->channelMapperManager()->getOrCreateResource(m_mapperId);
    Q_ASSERT(mapper);
    for (const auto mappingId : mapper->mappingIds()) {
        // Get the mapping object
        ChannelMapping *mapping = mappingManager->getOrCreateResource(mappingId);
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
                m_mappingData.push_back(mappingData);

                if (foundMatch)
                    break;
            }

            ++channelGroupIndex;
        }
    }
}

void ClipAnimator::evaluateAtGlobalTime(qint64 globalTime)
{
    AnimationClip *clip = m_handler->animationClipManager()->getOrCreateResource(m_clipId);
    Q_ASSERT(clip);

    // Calculate local time from global time
    const double t_global = double(globalTime) / 1.0e9;
    const double t_start_global = double(m_startGlobalTime) / 1.0e9;
    const double playbackRate = 1.0; // Assume standard playback rate for now
    const double duration = clip->duration();
    const int loopCount = 1; // TODO: Make property on QClipAnimator

    const double localTime = localTimeFromGlobalTime(t_global, t_start_global,
                                                     playbackRate, duration,
                                                     loopCount);
    evaluateAtLocalTime(localTime);
}

void ClipAnimator::evaluateAtLocalTime(float localTime)
{
    AnimationClip *clip = m_handler->animationClipManager()->getOrCreateResource(m_clipId);
    Q_ASSERT(clip);

    // TODO: Uncomment when we add loopCount property
    if (localTime >= clip->duration() && /*loopCount != 0 &&*/ m_currentLoop == 0 /*loopCount - 1*/)
        m_finalFrame = true;

    // Ensure we have enough storage to hold the evaluations
    m_channelResults.resize(clip->channelCount());

    // Iterate over channels and evaluate the fcurves
    const QVector<ChannelGroup> &channelGroups = clip->channelGroups();
    int i = 0;
    for (const ChannelGroup &channelGroup : channelGroups) {
        for (const auto channel : qAsConst(channelGroup.channels))
            m_channelResults[i++] = channel.fcurve.evaluateAtTime(localTime);
    }
}

void ClipAnimator::sendPropertyChanges()
{
    // Iterate over the mappings
    for (const auto mappingData : m_mappingData) {
        // Construct a property update change, set target, property and delivery options
        auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(mappingData.targetId);
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName(mappingData.propertyName);

        // Handle intermediate updates vs final flag properly
        Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(e.data())->m_isFinal = m_finalFrame;

        // Build the new value from the channel/fcurve evaluation results
        QVariant v;
        switch (mappingData.type) {
        case QVariant::Double: {
            v = QVariant::fromValue(m_channelResults[mappingData.channelIndices[0]]);
            break;
        }

        case QVariant::Vector2D: {
            const QVector2D vector(m_channelResults[mappingData.channelIndices[0]],
                                   m_channelResults[mappingData.channelIndices[1]]);
            v = QVariant::fromValue(vector);
            break;
        }

        case QVariant::Vector3D: {
            const QVector3D vector(m_channelResults[mappingData.channelIndices[0]],
                                   m_channelResults[mappingData.channelIndices[1]],
                                   m_channelResults[mappingData.channelIndices[2]]);
            v = QVariant::fromValue(vector);
            break;
        }

        case QVariant::Vector4D: {
            const QVector4D vector(m_channelResults[mappingData.channelIndices[0]],
                                   m_channelResults[mappingData.channelIndices[1]],
                                   m_channelResults[mappingData.channelIndices[2]],
                                   m_channelResults[mappingData.channelIndices[3]]);
            v = QVariant::fromValue(vector);
            break;
        }

        case QVariant::Quaternion: {
            QQuaternion q(m_channelResults[mappingData.channelIndices[0]],
                          m_channelResults[mappingData.channelIndices[1]],
                          m_channelResults[mappingData.channelIndices[2]],
                          m_channelResults[mappingData.channelIndices[3]]);
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
        notifyObservers(e);
    }

    // If it's the final frame, notify the frontend that we've stopped
    if (m_finalFrame) {
        setRunning(false);

        auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("running");
        e->setValue(false);
        Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(e.data())->m_isFinal = true;
        notifyObservers(e);
    }
}

double ClipAnimator::localTimeFromGlobalTime(double t_global, double t_start_global,
                                             double playbackRate, double duration,
                                             int loopCount)
{
    double t_local = playbackRate * (t_global - t_start_global);
    if (loopCount == 1) {
        t_local = qBound(0.0, t_local, duration);
    } else if (loopCount == 0) {
        // Loops forever
        t_local = std::fmod(t_local, duration);
    } else {
        // N loops
        t_local = qBound(0.0, t_local, double(loopCount) * duration);
        t_local = std::fmod(t_local, duration);
    }
    return t_local;
}

QVector<int> ClipAnimator::channelsToIndices(const ChannelGroup &channelGroup,
                                             int dataType,
                                             int offset)
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

QVector<int> ClipAnimator::channelsToIndicesHelper(const ChannelGroup &channelGroup,
                                                   int dataType,
                                                   int offset,
                                                   const QStringList &suffixes)
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

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
