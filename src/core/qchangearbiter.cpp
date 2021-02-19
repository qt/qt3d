/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
