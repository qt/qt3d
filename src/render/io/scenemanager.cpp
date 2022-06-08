// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "scenemanager_p.h"
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

SceneManager::SceneManager()
    : Qt3DCore::QResourceManager<Scene,
                                 Qt3DCore::QNodeId,
                                 Qt3DCore::ObjectLevelLockingPolicy>()
    , m_service(nullptr)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::setDownloadService(Qt3DCore::QDownloadHelperService *service)
{
    m_service = service;
}

void SceneManager::addSceneData(const QUrl &source,
                                Qt3DCore::QNodeId sceneUuid,
                                const QByteArray &data)
{
    LoadSceneJobPtr newJob(new LoadSceneJob(source, sceneUuid));

    if (!data.isEmpty())
        newJob->setData(data);

    // We cannot run two jobs that use the same scene loader plugin
    // in two different threads at the same time
    if (!m_pendingJobs.empty())
        newJob->addDependency(m_pendingJobs.back());

    m_pendingJobs.push_back(newJob);
}

std::vector<LoadSceneJobPtr> SceneManager::takePendingSceneLoaderJobs()
{
    return Qt3DCore::moveAndClear(m_pendingJobs);
}

void SceneManager::startSceneDownload(const QUrl &source, Qt3DCore::QNodeId sceneUuid)
{
    if (!m_service)
        return;
    SceneDownloaderPtr request = SceneDownloaderPtr::create(source, sceneUuid, this);
    m_pendingDownloads.push_back(request);
    m_service->submitRequest(request);
}

void SceneManager::clearSceneDownload(SceneDownloader *downloader)
{
    for (auto it = m_pendingDownloads.begin(); it != m_pendingDownloads.end(); ++it) {
        if ((*it).data() == downloader) {
            m_pendingDownloads.erase(it);
            return;
        }
    }
}


SceneDownloader::SceneDownloader(const QUrl &source, Qt3DCore::QNodeId sceneComponent, SceneManager *manager)
    : Qt3DCore::QDownloadRequest(source)
    , m_sceneComponent(sceneComponent)
    , m_manager(manager)
{

}

void SceneDownloader::onCompleted()
{
    if (!m_manager)
        return;
    if (succeeded())
        m_manager->addSceneData(url(), m_sceneComponent, m_data);
    else
        qWarning() << "Failed to download scene at" << url();
    m_manager->clearSceneDownload(this);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
