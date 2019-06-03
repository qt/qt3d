/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

    const auto childrenHandles = root->childrenHandles();
    for (const HEntity &handle : childrenHandles) {
        Entity *child = m_manager->renderNodesManager()->data(handle);
        if (child != nullptr && !apply(child))
            return false;
    }

    return true;
}
