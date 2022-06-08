// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "entityvisitor_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

QT_USE_NAMESPACE
using namespace Qt3DRender::Render;

EntityVisitor::EntityVisitor(NodeManagers *manager)
    : m_manager(manager)
    , m_pruneDisabled(false)
{

}

EntityVisitor::~EntityVisitor() = default;

/*!
 * \internal
 *
 * Override in derived class to do work on the current entity
 *
 * Return value (Continue, Prune, Stop) will affect traversal
 */
EntityVisitor::Operation EntityVisitor::visit(Entity *entity) {
    // return false to stop traversal
    if (!entity)
        return Stop;
    return Continue;
}

/*!
 * \internal
 *
 * If true, disabled entities and all their children will be ignored
 * during traversal
 *
 */
bool EntityVisitor::pruneDisabled() const
{
    return m_pruneDisabled;
}

void EntityVisitor::setPruneDisabled(bool pruneDisabled)
{
    m_pruneDisabled = pruneDisabled;
}

/*!
 * \internal
 *
 * Call on the root of the tree that should be traversed.
 * Returns false if any visit resulted in Stop
 */
bool EntityVisitor::apply(Entity *root) {
    if (!root)
        return false;
    if (m_pruneDisabled && !root->isEnabled())
        return true;

    const auto op = visit(root);
    if (op == Stop)
        return false;
    if (op == Prune)
        return true;

    const auto &childrenHandles = root->childrenHandles();
    for (const HEntity &handle : childrenHandles) {
        Entity *child = m_manager->renderNodesManager()->data(handle);
        if (child != nullptr && !apply(child))
            return false;
    }

    return true;
}
