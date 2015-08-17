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

#include "framepreparationjob_p.h"
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/renderentity_p.h>
#include <Qt3DRenderer/private/rendershaderdata_p.h>
#include <Qt3DRenderer/sphere.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

FramePreparationJob::FramePreparationJob(RenderEntity *root)
    : m_root(root)
{
}

FramePreparationJob::~FramePreparationJob()
{

}

void FramePreparationJob::run()
{
    parseNodeTree(m_root);
}

void FramePreparationJob::parseNodeTree(RenderEntity *node)
{
    // Initialize worldBoundingVolume if Mesh associated
    Qt3D::Render::RenderGeometryRenderer *mesh = Q_NULLPTR;
    if ((node->localBoundingVolume()->isNull())
            && (mesh = node->renderComponent<RenderGeometryRenderer>()) != Q_NULLPTR) {
        //        if (!mesh->meshDataHandle().isNull()) {
        //            Qt3D::QMeshData *meshData = mesh->meshData();
        //            if (meshData != Q_NULLPTR) {
        //                const QAxisAlignedBoundingBox box = meshData->boundingBox();
        //                node->localBoundingVolume()->setCenter(box.center());
        //                const QVector3D &radii = box.radii();
        //                node->localBoundingVolume()->setRadius(qMax(radii.x(), qMax(radii.y(), radii.z())));
        //            }
        //        }
        // TO DO: Make that work with the GeometryRenderer
    }

    // Update transform properties in RenderShaderData
    QList<RenderShaderData *> shadersData = node->renderComponents<RenderShaderData>();
    Q_FOREACH (RenderShaderData *r, shadersData) {
        r->updateTransformedProperties(*node->worldTransform());
    }

    // Traverse children
    Q_FOREACH (RenderEntity *child, node->children())
        parseNodeTree(child);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
