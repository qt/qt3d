/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

void Handler::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    Q_UNUSED(change);
    m_logicManager->appendHandler(this);
}

void Handler::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QBackendNode::sceneChangeEvent(e);
}

HandlerFunctor::HandlerFunctor(Manager *manager)
    : m_manager(manager)
{
}

Qt3DCore::QBackendNode *HandlerFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    Handler *handler = m_manager->logicHandlerManager()->getOrCreateResource(change->subjectId());
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
