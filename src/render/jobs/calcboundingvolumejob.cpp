/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "calcboundingvolumejob_p.h"

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/buffervisitor_p.h>
#include <Qt3DRender/private/entityvisitor_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>

#include <QtCore/qmath.h>
#if QT_CONFIG(concurrent)
#include <QtConcurrent/QtConcurrent>
#endif
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

class BoundingVolumeCalculator
{
public:
    BoundingVolumeCalculator(NodeManagers *manager) : m_manager(manager) { }

    const Sphere& result() { return m_volume; }
    const QVector3D min() const { return m_min; }
    const QVector3D max() const { return m_max; }

    bool apply(Qt3DRender::Render::Attribute *positionAttribute,
               Qt3DRender::Render::Attribute *indexAttribute,
               int drawVertexCount,
               bool primitiveRestartEnabled,
               int primitiveRestartIndex)
    {
        FindExtremePoints findExtremePoints(m_manager);
        if (!findExtremePoints.apply(positionAttribute, indexAttribute, drawVertexCount,
                                     primitiveRestartEnabled, primitiveRestartIndex))
            return false;

        m_min = QVector3D(findExtremePoints.xMin, findExtremePoints.yMin, findExtremePoints.zMin);
        m_max = QVector3D(findExtremePoints.xMax, findExtremePoints.yMax, findExtremePoints.zMax);

        FindMaxDistantPoint maxDistantPointY(m_manager);
        maxDistantPointY.setReferencePoint = true;
        if (!maxDistantPointY.apply(positionAttribute, indexAttribute, drawVertexCount,
                                     primitiveRestartEnabled, primitiveRestartIndex))
            return false;
        if (maxDistantPointY.hasNoPoints)
            return false;

        //const Vector3D x = maxDistantPointY.referencePt;
        const Vector3D y = maxDistantPointY.maxDistPt;

        FindMaxDistantPoint maxDistantPointZ(m_manager);
        maxDistantPointZ.setReferencePoint = false;
        maxDistantPointZ.referencePt = y;
        if (!maxDistantPointZ.apply(positionAttribute, indexAttribute, drawVertexCount,
                                     primitiveRestartEnabled, primitiveRestartIndex)) {
            return false;
        }
        const Vector3D z = maxDistantPointZ.maxDistPt;

        const Vector3D center = (y + z) * 0.5f;

        FindMaxDistantPoint maxDistantPointCenter(m_manager);
        maxDistantPointCenter.setReferencePoint = false;
        maxDistantPointCenter.referencePt = center;
        if (!maxDistantPointCenter.apply(positionAttribute, indexAttribute, drawVertexCount,
                                     primitiveRestartEnabled, primitiveRestartIndex)) {
            return false;
        }

        const float radius = (center - maxDistantPointCenter.maxDistPt).length();

        m_volume = Qt3DRender::Render::Sphere(center, radius);

        if (m_volume.isNull())
            return false;

        return true;
    }

private:
    Sphere m_volume;
    NodeManagers *m_manager;
    QVector3D m_min;
    QVector3D m_max;

    class FindExtremePoints : public Buffer3fVisitor
    {
    public:
        FindExtremePoints(NodeManagers *manager)
            : Buffer3fVisitor(manager)
            , xMin(0.0f), xMax(0.0f), yMin(0.0f), yMax(0.0f), zMin(0.0f), zMax(0.0f)
        { }

        float xMin, xMax, yMin, yMax, zMin, zMax;
        Vector3D xMinPt, xMaxPt, yMinPt, yMaxPt, zMinPt, zMaxPt;

        void visit(uint ndx, float x, float y, float z) override
        {
            if (ndx) {
                if (x < xMin) {
                    xMin = x;
                    xMinPt = Vector3D(x, y, z);
                }
                if (x > xMax) {
                    xMax = x;
                    xMaxPt = Vector3D(x, y, z);
                }
                if (y < yMin) {
                    yMin = y;
                    yMinPt = Vector3D(x, y, z);
                }
                if (y > yMax) {
                    yMax = y;
                    yMaxPt = Vector3D(x, y, z);
                }
                if (z < zMin) {
                    zMin = z;
                    zMinPt = Vector3D(x, y, z);
                }
                if (z > zMax) {
                    zMax = z;
                    zMaxPt = Vector3D(x, y, z);
                }
            } else {
                xMin = xMax = x;
                yMin = yMax = y;
                zMin = zMax = z;
                xMinPt = xMaxPt = yMinPt = yMaxPt = zMinPt = zMaxPt = Vector3D(x, y, z);
            }
        }
    };

    class FindMaxDistantPoint : public Buffer3fVisitor
    {
    public:
        FindMaxDistantPoint(NodeManagers *manager)
            : Buffer3fVisitor(manager)
        { }

        float maxLengthSquared = 0.0f;
        Vector3D maxDistPt;
        Vector3D referencePt;
        bool setReferencePoint = false;
        bool hasNoPoints = true;

        void visit(uint ndx, float x, float y, float z) override
        {
            Q_UNUSED(ndx);
            const Vector3D p = Vector3D(x, y, z);

            if (hasNoPoints && setReferencePoint) {
                maxLengthSquared = 0.0f;
                referencePt = p;
            }
            const float lengthSquared = (p - referencePt).lengthSquared();
            if ( lengthSquared >= maxLengthSquared ) {
                maxDistPt = p;
                maxLengthSquared = lengthSquared;
            }
            hasNoPoints = false;
        }
    };
};

struct BoundingVolumeComputeData {
    Entity *entity = nullptr;
    Geometry *geometry = nullptr;
    Attribute *positionAttribute = nullptr;
    Attribute *indexAttribute = nullptr;
    bool primitiveRestartEnabled = false;
    int primitiveRestartIndex = -1;
    int vertexCount = 0;

    bool valid() const { return entity != nullptr; }
};

BoundingVolumeComputeData findBoundingVolumeComputeData(NodeManagers *manager, Entity *node)
{
    GeometryRenderer *gRenderer = node->renderComponent<GeometryRenderer>();
    GeometryManager *geometryManager = manager->geometryManager();
    if (!gRenderer || gRenderer->primitiveType() == QGeometryRenderer::Patches)
        return {};

    Geometry *geom = geometryManager->lookupResource(gRenderer->geometryId());
    if (!geom)
        return {};

    int drawVertexCount = gRenderer->vertexCount(); // may be 0, gets changed below if so

    Qt3DRender::Render::Attribute *positionAttribute = manager->lookupResource<Attribute, AttributeManager>(geom->boundingPositionAttribute());
    bool hasBoundingVolumePositionAttribute = positionAttribute != nullptr;

    // Use the default position attribute if attribute is null
    if (!hasBoundingVolumePositionAttribute) {
        const auto attrIds = geom->attributes();
        for (const Qt3DCore::QNodeId &attrId : attrIds) {
            positionAttribute = manager->lookupResource<Attribute, AttributeManager>(attrId);
            if (positionAttribute &&
                positionAttribute->name() == QAttribute::defaultPositionAttributeName())
                break;
        }
    }

    if (!positionAttribute
        || positionAttribute->attributeType() != QAttribute::VertexAttribute
        || positionAttribute->vertexBaseType() != QAttribute::Float
        || positionAttribute->vertexSize() < 3) {
        qWarning("findBoundingVolumeComputeData: Position attribute not suited for bounding volume computation");
        return {};
    }

    Buffer *buf = manager->lookupResource<Buffer, BufferManager>(positionAttribute->bufferId());
    // No point in continuing if the positionAttribute doesn't have a suitable buffer
    if (!buf) {
        qWarning("findBoundingVolumeComputeData: Position attribute not referencing a valid buffer");
        return {};
    }

    // Check if there is an index attribute.
    Qt3DRender::Render::Attribute *indexAttribute = nullptr;
    Buffer *indexBuf = nullptr;

    if (!hasBoundingVolumePositionAttribute) {
        const QVector<Qt3DCore::QNodeId> attributes = geom->attributes();

        for (Qt3DCore::QNodeId attrNodeId : attributes) {
            Qt3DRender::Render::Attribute *attr = manager->lookupResource<Attribute, AttributeManager>(attrNodeId);
            if (attr && attr->attributeType() == QAttribute::IndexAttribute) {
                indexBuf = manager->lookupResource<Buffer, BufferManager>(attr->bufferId());
                if (indexBuf) {
                    indexAttribute = attr;

                    if (!drawVertexCount)
                        drawVertexCount = indexAttribute->count();

                    const QAttribute::VertexBaseType validIndexTypes[] = {
                        QAttribute::UnsignedShort,
                        QAttribute::UnsignedInt,
                        QAttribute::UnsignedByte
                    };

                    if (std::find(std::begin(validIndexTypes),
                                  std::end(validIndexTypes),
                                  indexAttribute->vertexBaseType()) == std::end(validIndexTypes)) {
                        qWarning() << "findBoundingVolumeComputeData: Unsupported index attribute type" << indexAttribute->name() << indexAttribute->vertexBaseType();
                        return {};
                    }

                    break;
                }
            }
        }
    }

    if (hasBoundingVolumePositionAttribute || (!indexAttribute && !drawVertexCount))
        drawVertexCount = positionAttribute->count();

    // Buf will be set to not dirty once it's loaded
    // in a job executed after this one
    // We need to recompute the bounding volume
    // If anything in the GeometryRenderer has changed
    if (buf->isDirty()
        || node->isBoundingVolumeDirty()
        || positionAttribute->isDirty()
        || geom->isDirty()
        || gRenderer->isDirty()
        || (indexAttribute && indexAttribute->isDirty())
        || (indexBuf && indexBuf->isDirty())) {
        BoundingVolumeComputeData res;
        res.entity = node;
        res.geometry = geom;
        res.positionAttribute = positionAttribute;
        res.indexAttribute = indexAttribute;
        res.primitiveRestartEnabled = gRenderer->primitiveRestartEnabled();
        res.primitiveRestartIndex = gRenderer->restartIndexValue();
        res.vertexCount = drawVertexCount;
        return res;
    }

    return {};
}

QVector<Geometry *> calculateLocalBoundingVolume(NodeManagers *manager, const BoundingVolumeComputeData &data)
{
    // The Bounding volume will only be computed if the position Buffer
    // isDirty

    QVector<Geometry *> updatedGeometries;

    BoundingVolumeCalculator reader(manager);
    if (reader.apply(data.positionAttribute, data.indexAttribute, data.vertexCount,
                     data.primitiveRestartEnabled, data.primitiveRestartIndex)) {
        data.entity->localBoundingVolume()->setCenter(reader.result().center());
        data.entity->localBoundingVolume()->setRadius(reader.result().radius());
        data.entity->unsetBoundingVolumeDirty();

        // Record min/max vertex in Geometry
        data.geometry->updateExtent(reader.min(), reader.max());
        // Mark geometry as requiring a call to update its frontend
        updatedGeometries.push_back(data.geometry);
    }

    return updatedGeometries;
}

struct UpdateBoundFunctor
{
    NodeManagers *manager;

    // This define is required to work with QtConcurrent
    typedef QVector<Geometry *> result_type;
    QVector<Geometry *> operator ()(const BoundingVolumeComputeData &data)
    {
        return calculateLocalBoundingVolume(manager, data);
    }
};

struct ReduceUpdateBoundFunctor
{
    void operator ()(QVector<Geometry *> &result, const QVector<Geometry *> &values)
    {
        result += values;
    }
};

class DirtyEntityAccumulator : public EntityVisitor
{
public:
    DirtyEntityAccumulator(NodeManagers *manager)
        : EntityVisitor(manager)
    {
    }

    EntityVisitor::Operation visit(Entity *entity) override
    {
        if (!entity->isTreeEnabled())
            return Prune;
        auto data = findBoundingVolumeComputeData(m_manager, entity);
        if (data.valid())
            m_entities.push_back(data);
        return Continue;
    }

    std::vector<BoundingVolumeComputeData> m_entities;
};

} // anonymous

CalculateBoundingVolumeJob::CalculateBoundingVolumeJob()
    : m_manager(nullptr)
    , m_node(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::CalcBoundingVolume, 0)
}

void CalculateBoundingVolumeJob::run()
{
    DirtyEntityAccumulator accumulator(m_manager);
    accumulator.apply(m_node);

    std::vector<BoundingVolumeComputeData> entities = std::move(accumulator.m_entities);

    QVector<Geometry *> updatedGeometries;
    updatedGeometries.reserve(entities.size());

#if QT_CONFIG(concurrent)
    if (entities.size() > 1) {
        UpdateBoundFunctor functor;
        functor.manager = m_manager;
        ReduceUpdateBoundFunctor reduceFunctor;
        updatedGeometries += QtConcurrent::blockingMappedReduced<decltype(updatedGeometries)>(entities, functor, reduceFunctor);
    } else
#endif
    {
        for (const auto &data: entities)
            updatedGeometries += calculateLocalBoundingVolume(m_manager, data);
    }

    // Send extent updates to frontend
    for (Geometry *geometry : updatedGeometries)
        geometry->notifyExtentChanged();
}

void CalculateBoundingVolumeJob::setRoot(Entity *node)
{
    m_node = node;
}

void CalculateBoundingVolumeJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

