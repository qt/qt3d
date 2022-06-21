// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qchangearbiter_p.h"

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <QtCore/QMutexLocker>
#include <QtCore/QReadLocker>
#include <QtCore/QThread>
#include <QtCore/QWriteLocker>

#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

#include <mutex>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QChangeArbiter::QChangeArbiter(QObject *parent)
    : QObject(parent)
    , m_scene(nullptr)
{
}

QChangeArbiter::~QChangeArbiter()
{
}

void QChangeArbiter::setScene(QScene *scene)
{
    m_scene = scene;
}

QScene *QChangeArbiter::scene() const
{
    return m_scene;
}

void QChangeArbiter::addDirtyFrontEndNode(QNode *node)
{
    if (!m_dirtyFrontEndNodes.contains(node)) {
        m_dirtyFrontEndNodes += node;
        emit receivedChange();
    }
}

void QChangeArbiter::addDirtyEntityComponentNodes(QEntity *entity, QComponent *component,
                                                  ComponentRelationshipChange::RelationShip change)
{
    addDirtyFrontEndNode(entity);
    m_dirtyEntityComponentNodeChanges.push_back({entity, component, change});
}

void QChangeArbiter::removeDirtyFrontEndNode(QNode *node)
{
    m_dirtyFrontEndNodes.removeOne(node);
    m_dirtyEntityComponentNodeChanges.erase(std::remove_if(m_dirtyEntityComponentNodeChanges.begin(), m_dirtyEntityComponentNodeChanges.end(), [node](const ComponentRelationshipChange &elt) {
                                    return elt.node == node || elt.subNode == node;
                                }), m_dirtyEntityComponentNodeChanges.end());
}

QList<QNode *> QChangeArbiter::takeDirtyFrontEndNodes()
{
    return Qt3DCore::moveAndClear(m_dirtyFrontEndNodes);
}

QList<ComponentRelationshipChange> QChangeArbiter::takeDirtyEntityComponentNodes()
{
    return Qt3DCore::moveAndClear(m_dirtyEntityComponentNodeChanges);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qchangearbiter_p.cpp"
