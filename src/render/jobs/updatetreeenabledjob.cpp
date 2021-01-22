/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

    const auto childrenHandles = node->childrenHandles();
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
