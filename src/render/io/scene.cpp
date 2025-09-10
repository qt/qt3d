// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "scene_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qdownloadhelperservice_p.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/private/qsceneloader_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Scene::Scene()
    : BackendNode(QBackendNode::ReadWrite)
    , m_sceneManager(nullptr)
{
}

void Scene::cleanup()
{
    m_source.clear();
}

void Scene::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QSceneLoader *node = qobject_cast<const QSceneLoader *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (node->source() != m_source) {
        m_source = node->source();
        if (m_source.isEmpty() || Qt3DCore::QDownloadHelperService::isLocal(m_source))
            m_sceneManager->addSceneData(m_source, peerId());
        else
            m_sceneManager->startSceneDownload(m_source, peerId());

        const auto d = static_cast<const QSceneLoaderPrivate *>(Qt3DCore::QNodePrivate::get(node));
        const_cast<QSceneLoaderPrivate *>(d)->setStatus(QSceneLoader::Loading);
    }
    markDirty(AbstractRenderer::AllDirty);
}

QUrl Scene::source() const
{
    return m_source;
}

void Scene::setSceneManager(SceneManager *manager)
{
    if (m_sceneManager != manager)
        m_sceneManager = manager;
}

RenderSceneFunctor::RenderSceneFunctor(AbstractRenderer *renderer, SceneManager *sceneManager)
    : m_sceneManager(sceneManager)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RenderSceneFunctor::create(Qt3DCore::QNodeId id) const
{
    Scene *scene = m_sceneManager->getOrCreateResource(id);
    scene->setSceneManager(m_sceneManager);
    scene->setRenderer(m_renderer);
    return scene;
}

Qt3DCore::QBackendNode *RenderSceneFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_sceneManager->lookupResource(id);
}

void RenderSceneFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_sceneManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
