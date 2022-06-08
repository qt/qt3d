// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_FILTERLAYERENTITYJOB_H
#define QT3DRENDER_RENDER_FILTERLAYERENTITYJOB_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/qlayerfilter.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Entity;
class NodeManagers;

class Q_3DRENDERSHARED_PRIVATE_EXPORT FilterLayerEntityJob : public Qt3DCore::QAspectJob
{
public:
    FilterLayerEntityJob();

    inline void setManager(NodeManagers *manager) noexcept { m_manager = manager; }
    inline void setLayerFilters(const Qt3DCore::QNodeIdVector &layerIds) noexcept { m_layerFilterIds = layerIds; }
    inline std::vector<Entity *> &filteredEntities() noexcept { return m_filteredEntities; }

    inline bool hasLayerFilter() const noexcept { return !m_layerFilterIds.isEmpty(); }
    inline Qt3DCore::QNodeIdVector layerFilters() const { return m_layerFilterIds; }

    // QAspectJob interface
    void run() final;

    void filterEntityAgainstLayers(Entity *entity, const Qt3DCore::QNodeIdVector &layerIds, const QLayerFilter::FilterMode filterMode);
    void filterAcceptAnyMatchingLayers(Entity *entity, const Qt3DCore::QNodeIdVector &layerIds);
    void filterAcceptAllMatchingLayers(Entity *entity, const Qt3DCore::QNodeIdVector &layerIds);
    void filterDiscardAnyMatchingLayers(Entity *entity, const Qt3DCore::QNodeIdVector &layerIds);
    void filterDiscardAllMatchingLayers(Entity *entity, const Qt3DCore::QNodeIdVector &layerIds);

private:
    void filterLayerAndEntity();
    void selectAllEntities();

    NodeManagers *m_manager;
    Qt3DCore::QNodeIdVector m_layerFilterIds;
    std::vector<Entity *> m_filteredEntities;
};

typedef QSharedPointer<FilterLayerEntityJob> FilterLayerEntityJobPtr;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FILTERLAYERENTITYJOB_H
