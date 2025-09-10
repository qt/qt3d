// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "manager_p.h"

#include <Qt3DLogic/qlogicaspect.h>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/qcoreapplication.h>

#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DLogic/private/executor_p.h>
#include <Qt3DLogic/private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {
namespace Logic {

Manager::Manager()
    : m_logicHandlerManager(new HandlerManager)
    , m_logicAspect(nullptr)
    , m_executor(nullptr)
    , m_dt(0.0f)
{
}

Manager::~Manager()
{
}

void Manager::setExecutor(Executor *executor)
{
    m_executor = executor;
}

void Manager::appendHandler(Handler *handler)
{
    HHandler handle = m_logicHandlerManager->lookupHandle(handler->peerId());
    if (!m_logicHandlers.contains(handle)) {
        m_logicHandlers.append(handle);
        m_logicComponentIds.append(handler->peerId());
    }
}

void Manager::removeHandler(Qt3DCore::QNodeId id)
{
    HHandler handle = m_logicHandlerManager->lookupHandle(id);
    m_logicComponentIds.removeAll(id);
    m_logicHandlers.removeAll(handle);
    m_logicHandlerManager->releaseResource(id);
}

bool Manager::hasFrameActions() const
{
    return m_logicHandlers.size() > 0;
}

// Called from Job postFrame (main thread)
void Manager::triggerLogicFrameUpdates()
{
    Q_ASSERT(m_executor);

    // Don't use blocking queued connections to main thread if it is already
    // in the process of shutting down as that will deadlock.
    if (Qt3DCore::QAbstractAspectPrivate::get(m_logicAspect)->m_aspectManager->isShuttingDown())
        return;

    m_executor->processLogicFrameUpdates(m_logicComponentIds, m_dt);
}

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE
