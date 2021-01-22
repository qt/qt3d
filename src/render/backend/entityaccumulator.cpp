/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

    QVector<Entity *> m_entities;

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
QVector<Entity *> EntityAccumulator::apply(Entity *root) const
{
    Accumulator a(m_predicate, m_manager);
    a.apply(root);
    return a.m_entities;
}
