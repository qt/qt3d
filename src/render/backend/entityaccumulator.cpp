// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "entityaccumulator_p.h"
#include "entityvisitor_p.h"

QT_USE_NAMESPACE
using namespace Qt3DRender::Render;

namespace {

class Accumulator : public EntityVisitor
{
public:
    Accumulator(std::function<bool(Entity *)> predicate, NodeManagers *manager)
        : EntityVisitor(manager)
        , m_predicate(predicate)
    {
    }

    EntityVisitor::Operation visit(Entity *entity) override {
        if (m_predicate(entity))
            m_entities << entity;
        return Continue;
    }

    QList<Entity *> m_entities;

private:
    std::function<bool(Entity *)> m_predicate;
};

}

EntityAccumulator::EntityAccumulator(NodeManagers *manager)
    : m_manager(manager)
    , m_predicate([](Entity*) { return true; })
{

}

EntityAccumulator::EntityAccumulator(std::function<bool (Entity *)> predicate, NodeManagers *manager)
    : m_manager(manager)
    , m_predicate(predicate)
{

}

/*!
 * \internal
 *
 * Call this to traverse the scene graph and return all entities for
 * which the predicate returns true.
 *
 * Can be useful to get all the entities that contain a specific type
 * of component.
 */
QList<Entity *> EntityAccumulator::apply(Entity *root) const
{
    Accumulator a(m_predicate, m_manager);
    a.apply(root);
    return a.m_entities;
}
