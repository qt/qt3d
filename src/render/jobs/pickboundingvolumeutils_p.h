// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEUTILS_H
#define QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEUTILS_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/QNodeId>
#include <Qt3DRender/QAbstractRayCaster>
#include <Qt3DRender/private/qray3d_p.h>
#include <Qt3DRender/private/qraycastingservice_p.h>
#include <Qt3DRender/qpickingsettings.h>


QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3DRender {
namespace RayCasting {
class QAbstractCollisionQueryService;
}

namespace Render {

class Entity;
class Renderer;
class FrameGraphNode;
class RenderSettings;
class NodeManagers;

namespace PickingUtils {

struct Q_AUTOTEST_EXPORT ViewportCameraAreaDetails
{
    Qt3DCore::QNodeId cameraId;
    Qt3DCore::QNodeId viewportNodeId;
    QRectF viewport;
    QSize area;
    QSurface *surface = nullptr;
    Qt3DCore::QNodeIdVector layersFilters;
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, PickingUtils, ViewportCameraAreaDetails, Q_COMPLEX_TYPE)

struct PickConfiguration {
    std::vector<ViewportCameraAreaDetails> vcaDetails;
    bool trianglePickingRequested = false;
    bool edgePickingRequested = false;
    bool pointPickingRequested = false;
    bool primitivePickingRequested = false;
    bool frontFaceRequested = false;
    bool backFaceRequested = false;
    float pickWorldSpaceTolerance = -1.f;

    PickConfiguration(FrameGraphNode *frameGraphRoot, RenderSettings *renderSettings);
};

class Q_AUTOTEST_EXPORT ViewportCameraAreaGatherer
{
public:
    ViewportCameraAreaGatherer(const Qt3DCore::QNodeId &nodeId = Qt3DCore::QNodeId()) : m_targetCamera(nodeId) { }
    std::vector<ViewportCameraAreaDetails> gather(FrameGraphNode *root);

private:
    Qt3DCore::QNodeId m_targetCamera;
    std::vector<FrameGraphNode *> m_leaves;

    void visit(FrameGraphNode *node);
    ViewportCameraAreaDetails gatherUpViewportCameraAreas(Render::FrameGraphNode *node) const;
    bool isUnique(const std::vector<ViewportCameraAreaDetails> &vcaList, const ViewportCameraAreaDetails &vca) const;
};

typedef std::vector<RayCasting::QCollisionQueryResult::Hit> HitList;

class Q_AUTOTEST_EXPORT HierarchicalEntityPicker
{
public:
    explicit HierarchicalEntityPicker(const RayCasting::QRay3D &ray, bool requireObjectPicker = true);

    void setLayerFilterIds(const Qt3DCore::QNodeIdVector &layerFilterIds);
    void setLayerIds(const Qt3DCore::QNodeIdVector &layerIds, QAbstractRayCaster::FilterMode mode);

    bool collectHits(NodeManagers *manager, Entity *root);
    inline HitList hits() const { return m_hits; }
    inline const std::vector<Entity *> &entities() const { return m_entities; }
    inline QHash<Qt3DCore::QNodeId, int> entityToPriorityTable() const { return m_entityToPriorityTable; }

private:
    RayCasting::QRay3D m_ray;
    HitList m_hits;
    std::vector<Entity *> m_entities;
    bool m_objectPickersRequired;
    Qt3DCore::QNodeIdVector m_layerFilterIds;
    Qt3DCore::QNodeIdVector m_layerIds;
    QAbstractRayCaster::FilterMode m_layerFilterMode = QAbstractRayCaster::AcceptAnyMatchingLayers;
    QHash<Qt3DCore::QNodeId, int> m_entityToPriorityTable;
};

struct Q_AUTOTEST_EXPORT AbstractCollisionGathererFunctor
{
    AbstractCollisionGathererFunctor();
    virtual ~AbstractCollisionGathererFunctor();

    bool m_objectPickersRequired = true;
    NodeManagers *m_manager = nullptr;
    RayCasting::QRay3D m_ray;
    QHash<Qt3DCore::QNodeId, int> m_entityToPriorityTable;

    virtual HitList computeHits(const std::vector<Entity *> &entities, Qt3DRender::QPickingSettings::PickResultMode mode) = 0;

    // This define is required to work with QtConcurrent
    typedef HitList result_type;
    HitList operator ()(const Entity *entity) const;
    virtual HitList pick(const Entity *entity) const = 0;

    bool rayHitsEntity(const Entity *entity) const;
    static void sortHits(HitList &results);
};

struct Q_AUTOTEST_EXPORT EntityCollisionGathererFunctor : public AbstractCollisionGathererFunctor
{
    HitList computeHits(const std::vector<Entity *> &entities, Qt3DRender::QPickingSettings::PickResultMode mode) override;
    HitList pick(const Entity *entity) const override;
};

struct Q_AUTOTEST_EXPORT TriangleCollisionGathererFunctor : public AbstractCollisionGathererFunctor
{
    bool m_frontFaceRequested;
    bool m_backFaceRequested;

    HitList computeHits(const std::vector<Entity *> &entities, Qt3DRender::QPickingSettings::PickResultMode mode) override;
    HitList pick(const Entity *entity) const override;
};

struct Q_AUTOTEST_EXPORT LineCollisionGathererFunctor : public AbstractCollisionGathererFunctor
{
    float m_pickWorldSpaceTolerance;

    HitList computeHits(const std::vector<Entity *> &entities, Qt3DRender::QPickingSettings::PickResultMode mode) override;
    HitList pick(const Entity *entity) const override;
};

struct Q_AUTOTEST_EXPORT PointCollisionGathererFunctor : public AbstractCollisionGathererFunctor
{
    float m_pickWorldSpaceTolerance;

    HitList computeHits(const std::vector<Entity *> &entities, Qt3DRender::QPickingSettings::PickResultMode mode) override;
    HitList pick(const Entity *entity) const override;
};

} // PickingUtils

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PICKBOUNDINGVOLUMEUTILS_H
