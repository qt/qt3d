// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "loadgeometryjob_p.h"
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/vector_helper_p.h>
#include <Qt3DRender/private/qmesh_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class LoadGeometryJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    LoadGeometryJobPrivate() {}
    ~LoadGeometryJobPrivate() {}

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QList<std::pair<Qt3DCore::QNodeId, GeometryFunctorResult>> m_updates;
};

LoadGeometryJob::LoadGeometryJob(const HGeometryRenderer &handle)
    : QAspectJob(*new LoadGeometryJobPrivate)
    , m_handle(handle)
    , m_nodeManagers(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LoadGeometry, 0)
}

LoadGeometryJob::~LoadGeometryJob()
{
}

void LoadGeometryJob::run()
{
    Q_D(LoadGeometryJob);
    GeometryRenderer *geometryRenderer = m_nodeManagers->geometryRendererManager()->data(m_handle);
    if (geometryRenderer != nullptr)
        d->m_updates.push_back({ geometryRenderer->peerId(), geometryRenderer->executeFunctor() });
}

void LoadGeometryJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    const auto updates = Qt3DCore::moveAndClear(m_updates);
    for (const auto &update : updates) {
        QGeometryRenderer *gR = static_cast<decltype(gR)>(manager->lookupNode(update.first));
        const GeometryFunctorResult &result = update.second;
        gR->setGeometry(result.geometry);

        // Set status if gR is a QMesh instance
        QMesh *mesh = qobject_cast<QMesh *>(gR);
        if (mesh) {
            QMeshPrivate *dMesh = static_cast<decltype(dMesh)>(Qt3DCore::QNodePrivate::get(mesh));
            dMesh->setStatus(result.status);
        }
    }
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
