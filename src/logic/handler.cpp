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

#include "handler_p.h"
#include "manager_p.h"
#include "managers_p.h"
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {
namespace Logic {

Handler::Handler()
    : m_logicManager(Q_NULLPTR)
{
}

void Handler::updateFromPeer(Qt3DCore::QNode *peer)
{
    Q_UNUSED(peer);
}

void Handler::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    Q_UNUSED(e);
}

HandlerFunctor::HandlerFunctor(Manager *manager)
    : m_manager(manager)
{
}

Qt3DCore::QBackendNode *HandlerFunctor::create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const
{
    Handler *handler = m_manager->logicHandlerManager()->getOrCreateResource(frontend->id());
    handler->setFactory(factory);
    handler->setManager(m_manager);
    handler->setPeer(frontend);
    m_manager->appendHandler(handler);
    return handler;
}

Qt3DCore::QBackendNode *HandlerFunctor::get(const Qt3DCore::QNodeId &id) const
{
    return m_manager->logicHandlerManager()->lookupResource(id);
}

void HandlerFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    m_manager->removeHandler(id);
}

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE
