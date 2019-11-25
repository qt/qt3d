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

#include "channelmapping_p.h"
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/qskeletonmapping.h>
#include <Qt3DAnimation/qcallbackmapping.h>
#include <Qt3DAnimation/private/qcallbackmapping_p.h>
#include <Qt3DAnimation/private/qchannelmapping_p.h>
#include <Qt3DAnimation/private/qskeletonmapping_p.h>
#include <Qt3DAnimation/private/animationlogging_p.h>
#include <Qt3DAnimation/private/qchannelmappingcreatedchange_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DCore/qabstractskeleton.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ChannelMapping::ChannelMapping()
    : BackendNode(ReadOnly)
    , m_channelName()
    , m_targetId()
    , m_type(static_cast<int>(QVariant::Invalid))
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
    m_type = static_cast<int>(QVariant::Invalid);
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
