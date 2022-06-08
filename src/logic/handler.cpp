// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "handler_p.h"

#include <Qt3DCore/qnode.h>

#include <Qt3DLogic/private/manager_p.h>
#include <Qt3DLogic/private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {
namespace Logic {

Handler::Handler()
    : m_logicManager(nullptr)
{
}

void Handler::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    Q_UNUSED(frontEnd);
    if (firstTime)
        m_logicManager->appendHandler(this);
}

HandlerFunctor::HandlerFunctor(Manager *manager)
    : m_manager(manager)
{
}

Qt3DCore::QBackendNode *HandlerFunctor::create(Qt3DCore::QNodeId id) const
{
    Handler *handler = m_manager->logicHandlerManager()->getOrCreateResource(id);
    handler->setManager(m_manager);
    return handler;
}

Qt3DCore::QBackendNode *HandlerFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_manager->logicHandlerManager()->lookupResource(id);
}

void HandlerFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_manager->removeHandler(id);
}

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE
