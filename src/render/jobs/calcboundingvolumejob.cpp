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

#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DCore/qaxisalignedboundingbox.h>

#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

void calculateLocalBoundingVolume(Renderer *renderer, Entity *node)
{
    // TO DO: How do we set the object picker to dirty when the buffer
    // referenced by the pickVolumeAttribute changes or has its internal buffer
    // data changed

    GeometryRenderer *gRenderer = node->renderComponent<GeometryRenderer>();
    if (gRenderer) {
        Geometry *geom = renderer->geometryManager()->lookupResource(gRenderer->geometryId());

        // TO DO: We must not recompute this every frame
        // Find a way to detect that the bounding volume attribute or its buffer have changed

        if (geom) {
            Qt3DRender::Render::Attribute *pickVolumeAttribute = renderer->attributeManager()->lookupResource(geom->boundingPositionAttribute());

            // Use the default position attribute if attribute is null
            if (!pickVolumeAttribute) {
                Q_FOREACH (const Qt3DCore::QNodeId attrId, geom->attributes()) {
                    pickVolumeAttribute = renderer->attributeManager()->lookupResource(attrId);
                    if (pickVolumeAttribute &&
                            pickVolumeAttribute->name() == QAttribute::defaultPositionAttributeName())
                        break;
                }
            }

            if (pickVolumeAttribute) {
                if (!pickVolumeAttribute
                        || pickVolumeAttribute->attributeType() != Qt3DCore::QAbstractAttribute::VertexAttribute
                        || pickVolumeAttribute->dataType() != Qt3DCore::QAbstractAttribute::Float
                        || pickVolumeAttribute->dataSize() < 3) {
                    qWarning() << "QBoundingVolumeSpecifier pickVolume Attribute not suited for bounding volume computation";
                    return;
                }

                Buffer *buf = renderer->bufferManager()->lookupResource(pickVolumeAttribute->bufferId());
                // No point in continuing if the positionAttribute doesn't have a suitable buffer
                if (!buf) {
                    qWarning() << "ObjectPicker pickVolume Attribute not referencing a valid buffer";
                    return;
                }

                const QByteArray buffer = buf->data();
                const char *rawBuffer = buffer.constData();
                rawBuffer += pickVolumeAttribute->byteOffset();
                const int stride = pickVolumeAttribute->byteStride() ? pickVolumeAttribute->byteStride() : sizeof(float) * pickVolumeAttribute->dataSize();
                QVector<QVector3D> vertices(pickVolumeAttribute->count());

                for (int c = 0, vC = vertices.size(); c < vC; ++c) {
                    QVector3D v;
                    const float *fptr = reinterpret_cast<const float*>(rawBuffer);
                    for (uint i = 0, m = qMin(pickVolumeAttribute->dataSize(), 3U); i < m; ++i)
                        v[i] = fptr[i];
                    vertices[c] = v;
                    rawBuffer += stride;
                }

                node->localBoundingVolume()->initializeFromPoints(vertices);
            }
        }
    }

    Q_FOREACH (Entity *child, node->children())
        calculateLocalBoundingVolume(renderer, child);
}

} // anonymous

CalculateBoundingVolumeJob::CalculateBoundingVolumeJob(Renderer *renderer)
    : m_renderer(renderer),
      m_node(Q_NULLPTR)
{
}

void CalculateBoundingVolumeJob::run()
{
    calculateLocalBoundingVolume(m_renderer, m_node);
}

void CalculateBoundingVolumeJob::setRoot(Entity *node)
{
    m_node = node;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

