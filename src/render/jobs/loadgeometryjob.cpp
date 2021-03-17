/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "loadgeometryjob_p.h"
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
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

    QVector<std::pair<Qt3DCore::QNodeId, GeometryFunctorResult>> m_updates;
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
    const auto updates = std::move(m_updates);
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
