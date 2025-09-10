// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "channelmapping_p.h"
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/qskeletonmapping.h>
#include <Qt3DAnimation/qcallbackmapping.h>
#include <Qt3DAnimation/private/qcallbackmapping_p.h>
#include <Qt3DAnimation/private/qchannelmapping_p.h>
#include <Qt3DAnimation/private/qskeletonmapping_p.h>
#include <Qt3DAnimation/private/animationlogging_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DCore/qabstractskeleton.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ChannelMapping::ChannelMapping()
    : BackendNode(ReadOnly)
    , m_channelName()
    , m_targetId()
    , m_type(static_cast<int>(QMetaType::UnknownType))
    , m_componentCount(0)
    , m_propertyName(nullptr)
    , m_callback(nullptr)
    , m_skeletonId()
    , m_mappingType(MappingType::ChannelMappingType)
{
}

void ChannelMapping::cleanup()
{
    setEnabled(false);
    m_channelName.clear();
    m_targetId = Qt3DCore::QNodeId();
    m_type = static_cast<int>(QMetaType::UnknownType);
    m_propertyName = nullptr;
    m_componentCount = 0;
    m_callback = nullptr;
    m_callbackFlags = {};
    m_skeletonId = Qt3DCore::QNodeId();
}

void ChannelMapping::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAbstractChannelMapping *node = qobject_cast<const QAbstractChannelMapping *>(frontEnd);
    if (!node)
        return;

    const QChannelMapping *channelMapping = qobject_cast<const QChannelMapping *>(frontEnd);
    if (channelMapping) {
        m_mappingType = ChannelMappingType;
        m_channelName = channelMapping->channelName();
        m_targetId = Qt3DCore::qIdForNode(channelMapping->target());

        QChannelMappingPrivate *d = static_cast<QChannelMappingPrivate *>(Qt3DCore::QNodePrivate::get(const_cast<QChannelMapping *>(channelMapping)));
        m_type = d->m_type;
        m_propertyName = d->m_propertyName;
        m_componentCount = d->m_componentCount;
    }

    const QSkeletonMapping *skeletonMapping = qobject_cast<const QSkeletonMapping *>(frontEnd);
    if (skeletonMapping) {
        m_mappingType = SkeletonMappingType;
        m_skeletonId = Qt3DCore::qIdForNode(skeletonMapping->skeleton());
    }

    const QCallbackMapping *callbackMapping = qobject_cast<const QCallbackMapping *>(frontEnd);
    if (callbackMapping) {
        m_mappingType = ChannelMappingType;
        m_channelName = callbackMapping->channelName();

        const QCallbackMappingPrivate *d = static_cast<const QCallbackMappingPrivate *>(Qt3DCore::QNodePrivate::get(callbackMapping));
        m_type = d->m_type;
        m_callback = d->m_callback;
        m_callbackFlags = d->m_callbackFlags;
    }
}

Skeleton *ChannelMapping::skeleton() const
{
    return m_handler->skeletonManager()->lookupResource(m_skeletonId);
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
