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
