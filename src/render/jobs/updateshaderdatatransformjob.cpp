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

#include "updateshaderdatatransformjob_p.h"
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/light_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

UpdateShaderDataTransformJob::UpdateShaderDataTransformJob()
    : m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateShaderDataTransform, 0)
}

UpdateShaderDataTransformJob::~UpdateShaderDataTransformJob()
{
}

void UpdateShaderDataTransformJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

NodeManagers *UpdateShaderDataTransformJob::managers() const
{
    return m_manager;
}

void UpdateShaderDataTransformJob::run()
{
    EntityManager *manager = m_manager->renderNodesManager();
    const std::vector<HEntity> &handles = manager->activeHandles();

    for (const HEntity &handle : handles) {
        Entity *node = manager->data(handle);
        // Update transform properties in ShaderDatas and Lights
        const QVector<ShaderData *> shaderDatas = node->renderComponents<ShaderData>();
        for (ShaderData *r : shaderDatas)
            r->updateWorldTransform(*node->worldTransform());
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
