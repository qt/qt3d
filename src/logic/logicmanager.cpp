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

#include "logicmanager_p.h"
#include "qlogicaspect.h"
#include <Qt3DLogic/private/logicexecutor_p.h>
#include <Qt3DLogic/private/logicmanagers_p.h>
#include <QtCore/qcoreapplication.h>

#include <QDebug>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Logic {

LogicManager::LogicManager()
    : m_logicHandlerManager(new LogicHandlerManager)
    , m_semaphore(1)
{
    m_semaphore.acquire();
}

LogicManager::~LogicManager()
{
}

void LogicManager::setExecutor(LogicExecutor *executor)
{
    m_executor = executor;
    if (m_executor)
        m_executor->setSemephore(&m_semaphore);
}

void LogicManager::appendLogicHandler(LogicHandler *handler)
{
    HLogicHandler handle = m_logicHandlerManager->lookupHandle(handler->peerUuid());
    m_logicHandlers.append(handle);
    m_logicComponentIds.append(handler->peerUuid());
}

void LogicManager::removeLogicHandler(const QNodeId &id)
{
    HLogicHandler handle = m_logicHandlerManager->lookupHandle(id);
    m_logicComponentIds.removeAll(id);
    m_logicHandlers.removeAll(handle);
    m_logicHandlerManager->releaseResource(id);
}

void LogicManager::triggerLogicFrameUpdates()
{
    Q_ASSERT(m_executor);

    // Don't use blocking queued connections to main thread if it is already
    // in the process of shutting down as that will deadlock.
    if (m_logicAspect->isShuttingDown())
        return;

    // Trigger the main thread to process logic frame updates for each
    // logic component and then wait until done. The LogicExecutor will
    // release the semaphore when it has completed its work.
    m_executor->enqueueLogicFrameUpdates(m_logicComponentIds);
    qApp->postEvent(m_executor, new FrameUpdateEvent);
    m_semaphore.acquire();
}

} // Logic

} // namespace Qt3D

QT_END_NAMESPACE
