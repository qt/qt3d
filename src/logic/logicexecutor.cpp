/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "logicexecutor_p.h"
#include <Qt3DLogic/qlogiccomponent.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qscene_p.h>
#include <QtCore/qsemaphore.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Logic {

LogicExecutor::LogicExecutor(QObject *parent)
    : QObject(parent)
    , m_scene(Q_NULLPTR)
{
}

void LogicExecutor::clearQueueAndProceed()
{
    // Clear the queue of nodes to process frame updates for (throw the work away).
    // If the semaphore is acquired, release it to allow the logic job and hence the
    // manager and frame to complete and shutdown to continue.
    m_nodeIds.clear();
    if (m_semaphore->available() == 0)
        m_semaphore->release();
}

void LogicExecutor::enqueueLogicFrameUpdates(const QVector<QNodeId> &nodeIds)
{
    m_nodeIds = nodeIds;
}

bool LogicExecutor::event(QEvent *e)
{
    if (e->type() == QEvent::User) {
        processLogicFrameUpdates();
        e->setAccepted(true);
        return true;
    }
    return false;
}

/*!
   \internal

   Called from context of main thread
*/
void LogicExecutor::processLogicFrameUpdates()
{
    Q_ASSERT(m_scene);
    Q_ASSERT(m_semaphore);
    QVector<QNode *> nodes = m_scene->lookupNodes(m_nodeIds);
    foreach (QNode *node, nodes) {
        QLogicComponent *logicComponent = qobject_cast<QLogicComponent *>(node);
        if (logicComponent)
            logicComponent->onFrameUpdate();
    }

    // Release the semaphore so the calling LogicManager can continue
    m_semaphore->release();
}

} // Logic

} // Qt3D

QT_END_NAMESPACE
