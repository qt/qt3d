/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "scene_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qbackendscenepropertychange.h>
#include <Qt3DRender/qabstractsceneloader.h>
#include <Qt3DRender/private/scenemanager_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Scene::Scene()
    : QBackendNode(QBackendNode::ReadWrite)
    , m_sceneManager(Q_NULLPTR)
{
}

void Scene::updateFromPeer(Qt3DCore::QNode *peer)
{
    QAbstractSceneLoader *loader = static_cast<QAbstractSceneLoader *>(peer);

    m_source = loader->source();
    m_sceneManager->addSceneData(m_source, peerUuid());
}

void Scene::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (propertyChange->propertyName() == QByteArrayLiteral("source")) {
        m_source = propertyChange->value().toUrl();
        m_sceneManager->addSceneData(m_source, peerUuid());
    }
}

QUrl Scene::source() const
{
    return m_source;
}

void Scene::setSceneSubtree(Qt3DCore::QEntity *subTree)
{
    QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
    e->setPropertyName("scene");
    // The Frontend element has to perform the clone
    // So that the objects are created in the main thread
    e->setValue(QVariant::fromValue(QNodePtr(subTree, &QNodePrivate::nodePtrDeleter)));
    e->setTargetNode(peerUuid());
    notifyObservers(e);
    QBackendScenePropertyChangePtr e2(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
    e2->setPropertyName("status");
    e2->setValue(subTree != Q_NULLPTR ? QAbstractSceneLoader::Loaded : QAbstractSceneLoader::Error);
    e2->setTargetNode(peerUuid());
    notifyObservers(e2);
}

void Scene::setSceneManager(SceneManager *manager)
{
    if (m_sceneManager != manager)
        m_sceneManager = manager;
}

RenderSceneFunctor::RenderSceneFunctor(SceneManager *sceneManager)
    : m_sceneManager(sceneManager)
{
}

Qt3DCore::QBackendNode *RenderSceneFunctor::create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const
{
    Scene *scene = m_sceneManager->getOrCreateResource(frontend->id());
    scene->setFactory(factory);
    scene->setSceneManager(m_sceneManager);
    scene->setPeer(frontend);
    return scene;
}

Qt3DCore::QBackendNode *RenderSceneFunctor::get(const Qt3DCore::QNodeId &id) const
{
    return m_sceneManager->lookupResource(id);
}

void RenderSceneFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    m_sceneManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
