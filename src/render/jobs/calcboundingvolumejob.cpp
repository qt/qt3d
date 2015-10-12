/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "calcboundingvolumejob_p.h"

#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/entity_p.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/geometryrenderer_p.h>
#include <Qt3DRenderer/private/buffermanager_p.h>
#include <Qt3DRenderer/private/buffer_p.h>
#include <Qt3DRenderer/sphere.h>
#include <Qt3DCore/qaxisalignedboundingbox.h>

#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

void calculateLocalBoundingVolume(Renderer *renderer, Entity *node)
{
    Qt3DRender::Render::GeometryRenderer *mesh = node->renderComponent<GeometryRenderer>();
    if (mesh) {
        Geometry *geom = renderer->geometryManager()->lookupResource(mesh->geometryId());
        if (geom) {
            Q_FOREACH (const Qt3DCore::QNodeId &attrId, geom->attributes()) {
                Attribute *attr = renderer->attributeManager()->lookupResource(attrId);
                if (!attr
                        || attr->attributeType() != Qt3DCore::QAbstractAttribute::VertexAttribute
                        || !attr->isDirty()
                        || attr->name() != QAttribute::defaultPositionAttributeName()
                        || attr->dataType() != Qt3DCore::QAbstractAttribute::Float
                        || attr->dataSize() < 3)
                    continue;

                Buffer *buf = renderer->bufferManager()->lookupResource(attr->bufferId());
                // No point in continuing if the positionAttribute doesn't have a suitable buffer
                if (!buf)
                    break;

                Qt3DCore::QAxisAlignedBoundingBox bbox;
                const QByteArray buffer = buf->data();
                const char *rawBuffer = buffer.constData();
                rawBuffer += attr->byteOffset();
                const int stride = attr->byteStride() ? attr->byteStride() : sizeof(float) * attr->dataSize();
                QVector<QVector3D> vertices(attr->count());

                // TO DO: We don't need to create a vector of QVector3D
                // to build bbox used to then build a sphere, we could build the sphere
                // by just looking at the vertices using more efficient algorithms (EPOS, Ritters)
                for (int c = 0, vC = vertices.size(); c < vC; ++c) {
                    QVector3D v;
                    const float *fptr = reinterpret_cast<const float*>(rawBuffer);
                    for (uint i = 0, m = qMin(attr->dataSize(), 3U); i < m; ++i)
                        v[i] = fptr[i];
                    vertices[c] = v;
                    rawBuffer += stride;
                }
                //Phase 1
                bbox.update(vertices);

                //Phase 2
                node->localBoundingVolume()->setCenter(bbox.center());
                node->localBoundingVolume()->setRadius(bbox.maxExtent() * 0.5f);
                break;
            }
        }
    }

    Q_FOREACH (Entity *child, node->children())
        calculateLocalBoundingVolume(renderer, child);
}

} // anonymous

CalculateBoundingVolumeJob::CalculateBoundingVolumeJob(Renderer *renderer, Entity *node)
    : m_renderer(renderer),
      m_node(node)
{
}

void CalculateBoundingVolumeJob::run()
{
    calculateLocalBoundingVolume(m_renderer, m_node);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

