/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "framepreparationjob_p.h"
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

FramePreparationJob::FramePreparationJob(NodeManagers *manager)
    : m_root(nullptr)
    , m_manager(manager)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::FramePreparation, 0);
}

FramePreparationJob::~FramePreparationJob()
{
}

void FramePreparationJob::setRoot(Entity *root)
{
    m_root = root;
}

void FramePreparationJob::run()
{
    parseNodeTree(m_root);
}

void FramePreparationJob::parseNodeTree(Entity *node)
{
    // Update transform properties in ShaderDatas and Lights
    const QList<ShaderData *> shaderDatas = node->renderComponents<ShaderData>();
    for (ShaderData *r : shaderDatas)
        r->updateWorldTransform(*node->worldTransform());

    // Look if for the GeometryRender/Geometry the attributes and or buffers are dirty
    // in which case we need to recompute the triangle list
    GeometryRenderer *geomRenderer = node->renderComponent<GeometryRenderer>();
    const Qt3DCore::QNodeId geomRendererId = node->componentUuid<GeometryRenderer>();
    Geometry *geom = nullptr;
    if (geomRenderer &&
            (geom = m_manager->lookupResource<Geometry, GeometryManager>(geomRenderer->geometryId())) != nullptr) {
        if (!m_manager->geometryRendererManager()->isGeometryRendererScheduledForTriangleDataRefresh(geomRendererId)) {
            // Check if the attributes or buffers are dirty
            bool dirty = geomRenderer->isDirty();
            Attribute *attr = nullptr;
            const auto attrIds = geom->attributes();
            for (const Qt3DCore::QNodeId attrId : attrIds) {
                if ((attr = m_manager->attributeManager()->lookupResource(attrId)) != nullptr) {
                    dirty |= attr->isDirty();
                    if (!dirty) {
                        Buffer *buffer = nullptr;
                        if ((buffer = m_manager->bufferManager()->lookupResource(attr->bufferId())) != nullptr)
                            dirty = buffer->isDirty();
                    }
                    if (dirty)
                        break;
                }
            }
            if (dirty)
                m_manager->geometryRendererManager()->requestTriangleDataRefreshForGeometryRenderer(geomRendererId);
        }
    }

    const QVector<Entity *> children = node->children();
    for (Entity *c : children)
        parseNodeTree(c);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
