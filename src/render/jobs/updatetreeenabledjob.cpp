// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "updatetreeenabledjob_p.h"

#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

void updateTreeEnabled(NodeManagers *manager, Entity *node, bool parentEnabled)
{
    const bool treeEnabled = node->isEnabled() && parentEnabled;
    node->setTreeEnabled(treeEnabled);

    const auto &childrenHandles = node->childrenHandles();
    for (const HEntity &handle : childrenHandles) {
        Entity *child = manager->renderNodesManager()->data(handle);
        if (child)
            updateTreeEnabled(manager, child, treeEnabled);
    }
}

}

UpdateTreeEnabledJob::UpdateTreeEnabledJob()
    : Qt3DCore::QAspectJob()
    , m_node(nullptr)
    , m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateTreeEnabled, 0)
}

void UpdateTreeEnabledJob::setRoot(Entity *root)
{
    m_node = root;
}

void UpdateTreeEnabledJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void UpdateTreeEnabledJob::run()
{
    if (m_node && m_manager)
        updateTreeEnabled(m_manager, m_node, true);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
