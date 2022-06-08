// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "channelmapper_p.h"
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/private/qchannelmapper_p.h>
#include <Qt3DAnimation/private/animationlogging_p.h>
#include <Qt3DAnimation/private/managers_p.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ChannelMapper::ChannelMapper()
    : BackendNode(ReadOnly)
    , m_mappingIds()
    , m_isDirty(true)
{
}

void ChannelMapper::cleanup()
{
    setEnabled(false);
    m_mappingIds.clear();
    m_mappings.clear();
    m_isDirty = true;
}

void ChannelMapper::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QChannelMapper *node = qobject_cast<const QChannelMapper *>(frontEnd);
    if (!node)
        return;

    auto ids = Qt3DCore::qIdsForNodes(node->mappings());
    std::sort(std::begin(ids), std::end(ids));
    m_isDirty = firstTime;
    if (m_mappingIds != ids) {
        m_mappingIds = ids;
        setDirty(Handler::ChannelMappingsDirty);
        m_isDirty = true;
    }
}

void ChannelMapper::updateMappings() const
{
    m_mappings.clear();
    m_mappings.reserve(m_mappingIds.size());
    const auto mappingManager = m_handler->channelMappingManager();
    for (const auto &mappingId : m_mappingIds) {
        const auto mapping = mappingManager->lookupResource(mappingId);
        Q_ASSERT(mapping);
        m_mappings.push_back(mapping);
    }
    m_isDirty = false;
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
