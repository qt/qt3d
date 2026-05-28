// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "calcboundingvolumejob_p.h"

#include <Qt3DCore/qboundingvolume.h>
#include <Qt3DCore/private/qabstractfrontendnodemanager_p.h>
#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/qgeometryrenderer_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/buffervisitor_p.h>
#include <Qt3DRender/private/entityvisitor_p.h>

#include <QtCore/qmath.h>
#if QT_CONFIG(concurrent)
#include <QtConcurrent/QtConcurrent>
#endif
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

namespace {

class BoundingVolumeCalculator
{
public:
    explicit BoundingVolumeCalculator(NodeManagers *manager) : m_manager(manager) { }

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
        explicit FindExtremePoints(NodeManagers *manager)
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
        explicit FindMaxDistantPoint(NodeManagers *manager)
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
    GeometryRenderer *renderer = nullptr;
    Geometry *geometry = nullptr;
    Attribute *positionAttribute = nullptr;
    Attribute *indexAttribute = nullptr;
    int vertexCount = -1;

    bool valid() const { return vertexCount >= 0; }
};

BoundingVolumeComputeData findBoundingVolumeComputeData(NodeManagers *manager, Entity *node)
{
    BoundingVolumeComputeData res;
    res.entity = node;

    res.renderer = node->renderComponent<GeometryRenderer>();
    if (!res.renderer || res.renderer->primitiveType() == QGeometryRenderer::Patches)
        return res;

    GeometryManager *geometryManager = manager->geometryManager();
    res.geometry = geometryManager->lookupResource(res.renderer->geometryId());
    if (!res.geometry)
        return res;

    // if it has a view, the bounding volume will have been computed by the core aspect
    if (res.renderer->hasView())
        return res;

    int drawVertexCount = res.renderer->vertexCount(); // may be 0, gets changed below if so

    Qt3DRender::Render::Attribute *positionAttribute = manager->lookupResource<Attribute, AttributeManager>(res.geometry->boundingPositionAttribute());
    bool hasBoundingVolumePositionAttribute = positionAttribute != nullptr;

    // Use the default position attribute if attribute is null
    if (!hasBoundingVolumePositionAttribute) {
        const auto attrIds = res.geometry->attributes();
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
        return res;
    }

    Buffer *buf = manager->lookupResource<Buffer, BufferManager>(positionAttribute->bufferId());
    // No point in continuing if the positionAttribute doesn't have a suitable buffer
    if (!buf) {
        qWarning("findBoundingVolumeComputeData: Position attribute not referencing a valid buffer");
        return res;
    }

    // Check if there is an index attribute.
    Qt3DRender::Render::Attribute *indexAttribute = nullptr;
    Buffer *indexBuf = nullptr;

    if (!hasBoundingVolumePositionAttribute) {
        const QList<Qt3DCore::QNodeId> attributes = res.geometry->attributes();

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
                        return res;
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
        || res.geometry->isDirty()
        || res.renderer->isDirty()
        || (indexAttribute && indexAttribute->isDirty())
        || (indexBuf && indexBuf->isDirty())) {
        res.vertexCount = drawVertexCount;
        res.positionAttribute = positionAttribute;
        res.indexAttribute = indexAttribute;
    }

    return res;
}

std::vector<Geometry *> calculateLocalBoundingVolume(NodeManagers *manager, const BoundingVolumeComputeData &data)
{
    // The Bounding volume will only be computed if the position Buffer
    // isDirty

    std::vector<Geometry *> updatedGeometries;

    BoundingVolumeCalculator reader(manager);
    if (reader.apply(data.positionAttribute, data.indexAttribute, data.vertexCount,
                     data.renderer->primitiveRestartEnabled(), data.renderer->restartIndexValue())) {
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
    typedef std::vector<Geometry *> result_type;
    std::vector<Geometry *> operator ()(const BoundingVolumeComputeData &data)
    {
        return calculateLocalBoundingVolume(manager, data);
    }
};

struct ReduceUpdateBoundFunctor
{
    void operator ()(std::vector<Geometry *> &result, const std::vector<Geometry *> &values)
    {
        result.insert(result.end(),
                      values.begin(),
                      values.end());
    }
};

class DirtyEntityAccumulator : public EntityVisitor
{
public:
    explicit DirtyEntityAccumulator(NodeManagers *manager)
        : EntityVisitor(manager)
    {
    }

    EntityVisitor::Operation visit(Entity *entity) override
    {
        if (!entity->isTreeEnabled())
            return Prune;
        auto data = findBoundingVolumeComputeData(m_manager, entity);

        if (data.valid()) {
            // only valid if front end is a QGeometryRenderer without a view. All other cases handled by core aspect
            m_entities.push_back(data);
        }

        return Continue;
    }

    Qt3DCore::QAbstractFrontEndNodeManager *m_frontEndNodeManager = nullptr;
    std::vector<BoundingVolumeComputeData> m_entities;
};


} // anonymous


CalculateBoundingVolumeJob::CalculateBoundingVolumeJob()
    : Qt3DCore::QAspectJob()
    , m_manager(nullptr)
    , m_node(nullptr)
    , m_frontEndNodeManager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::CalcBoundingVolume, 0)
}

void CalculateBoundingVolumeJob::run()
{
    // There's 2 bounding volume jobs, one in Core, the other here in Render.
    // This one is setup to run after the other.
    // (see more details in Qt3DCore::CalculateBoundingVolumeJob::run)
    //
    // TODO:
    // - remove the one frame delay for propagating results of first job
    // - avoid copying the computed BV at every frame

    Q_ASSERT(m_frontEndNodeManager);

    DirtyEntityAccumulator accumulator(m_manager);
    accumulator.m_frontEndNodeManager = m_frontEndNodeManager;
    accumulator.apply(m_node);

    const std::vector<BoundingVolumeComputeData> entities = std::move(accumulator.m_entities);

    std::vector<Geometry *> updatedGeometries;
    updatedGeometries.reserve(entities.size());

#if QT_CONFIG(concurrent)
    if (entities.size() > 1 && QAspectJobManager::idealThreadCount() > 1) {
        UpdateBoundFunctor functor;
        functor.manager = m_manager;
        ReduceUpdateBoundFunctor reduceFunctor;
        const std::vector<Geometry *> &newGeometries = QtConcurrent::blockingMappedReduced<decltype(updatedGeometries)>(entities, functor, reduceFunctor);
        updatedGeometries.insert(updatedGeometries.end(),
                                 newGeometries.begin(),
                                 newGeometries.end());
    } else
#endif
    {
        for (const auto &data: entities) {
            const std::vector<Geometry *> &newGeometries = calculateLocalBoundingVolume(m_manager, data);
            updatedGeometries.insert(updatedGeometries.end(),
                                     newGeometries.begin(),
                                     newGeometries.end());
        }
    }

    m_updatedGeometries = std::move(updatedGeometries);
}

void CalculateBoundingVolumeJob::postFrame(QAspectEngine *aspectEngine)
{
    Q_UNUSED(aspectEngine);
    for (Geometry *backend : m_updatedGeometries) {
        Qt3DCore::QGeometry *node = qobject_cast<Qt3DCore::QGeometry *>(m_frontEndNodeManager->lookupNode(backend->peerId()));
        if (!node)
            continue;
        Qt3DCore::QGeometryPrivate *dNode = static_cast<Qt3DCore::QGeometryPrivate *>(Qt3DCore::QNodePrivate::get(node));
        dNode->setExtent(backend->min(), backend->max());
    }

    m_updatedGeometries.clear();
}

void CalculateBoundingVolumeJob::process(const Qt3DCore::BoundingVolumeComputeResult &result, bool computedResult)
{
    // This gets called from the thread of the CalculateBoundingVolumeJob in the core aspect.
    // We receive the data calculated there and update our backend nodes

    auto entity = m_manager->renderNodesManager()->lookupResource(result.entity->id());
    if (!entity)
        return;

    // copy data to the entity
    entity->localBoundingVolume()->setCenter(Vector3D(result.m_center));
    entity->localBoundingVolume()->setRadius(std::max(result.m_radius, 0.0f));
    entity->unsetBoundingVolumeDirty();
    // copy the data to the geometry
    if (computedResult) {
        auto renderer = entity->renderComponent<GeometryRenderer>();
        if (renderer) {
            auto geometry = m_manager->geometryManager()->lookupResource(renderer->geometryId());

            if (geometry)
                geometry->updateExtent(result.m_min, result.m_max);
        }
    }
}

void CalculateBoundingVolumeJob::setRoot(Entity *node)
{
    m_node = node;
}

void CalculateBoundingVolumeJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void CalculateBoundingVolumeJob::setFrontEndNodeManager(Qt3DCore::QAbstractFrontEndNodeManager *manager)
{
    m_frontEndNodeManager = manager;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

