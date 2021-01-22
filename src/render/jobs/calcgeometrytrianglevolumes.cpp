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

#include "calcgeometrytrianglevolumes_p.h"
#include <Qt3DRender/private/trianglesextractor_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

CalcGeometryTriangleVolumes::CalcGeometryTriangleVolumes(const Qt3DCore::QNodeId geometryRendererId, NodeManagers *manager)
    : Qt3DCore::QAspectJob()
    , m_geometryRendererId(geometryRendererId)
    , m_manager(manager)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::CalcTriangleVolume, 0)
}

void CalcGeometryTriangleVolumes::run()
{
    GeometryRenderer *renderer = m_manager->geometryRendererManager()->lookupResource(m_geometryRendererId);
    if (renderer != nullptr) {
        TrianglesExtractor extractor(renderer, m_manager);
        renderer->setTriangleVolumes(extractor.extract(m_geometryRendererId));
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
