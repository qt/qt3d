// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERERCACHE_P_H
#define QT3DRENDER_RENDER_RENDERERCACHE_P_H

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

#include <Qt3DCore/private/vector_helper_p.h>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>
#include <Qt3DRender/private/lightsource_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

template<class RenderCommand>
struct EntityRenderCommandData
{
    std::vector<const Entity *> entities;
    std::vector<RenderCommand> commands;
    std::vector<RenderPassParameterData> passesData;

    void reserve(size_t size)
    {
        entities.reserve(size);
        commands.reserve(size);
        passesData.reserve(size);
    }

    inline size_t size() const { return entities.size(); }

    inline void push_back(const Entity *e, const RenderCommand &c, const RenderPassParameterData &p)
    {
        entities.push_back(e);
        commands.push_back(c);
        passesData.push_back(p);
    }

    inline void push_back(const Entity *e, RenderCommand &&c, RenderPassParameterData &&p)
    {
        entities.push_back(e);
        commands.push_back(std::move(c));
        passesData.push_back(std::move(p));
    }

    EntityRenderCommandData &operator+=(EntityRenderCommandData &&t)
    {
        Qt3DCore::moveAtEnd(entities, std::move(t.entities));
        Qt3DCore::moveAtEnd(commands, std::move(t.commands));
        Qt3DCore::moveAtEnd(passesData, std::move(t.passesData));
        return *this;
    }

};

template<class RenderCommand>
struct EntityRenderCommandDataView
{
    EntityRenderCommandData<RenderCommand> data;
    std::vector<size_t> indices;

    size_t size() const noexcept { return indices.size(); }

    template<typename F>
    void forEachCommand(F func)
    {
        for (size_t idx : indices)
            func(data.commands[idx]);
    }
};

template<class RenderCommand>
using EntityRenderCommandDataViewPtr = QSharedPointer<EntityRenderCommandDataView<RenderCommand>>;

template<class RenderCommand>
struct EntityRenderCommandDataSubView
{
    EntityRenderCommandDataViewPtr<RenderCommand> view;
    size_t offset = 0;
    size_t count = 0;

    template<typename F>
    void forEach(F func)
    {
        for (size_t i = 0, m = size_t(count); i < m; ++i) {
            const size_t idx = view->indices[offset + i];
            func(view->data.entities[idx],
                 view->data.passesData[idx],
                 view->data.commands[idx]);
        }
    }

    template<typename F>
    void forEach(F func) const
    {
        for (size_t i = 0, m = size_t(count); i < m; ++i) {
            const size_t idx = view->indices[offset + i];
            func(view->data.entities[idx],
                 view->data.passesData[idx],
                 view->data.commands[idx]);
        }
    }
};

template<class RenderCommand>
struct RendererCache
{
    struct LeafNodeData
    {
        Matrix4x4 viewProjectionMatrix;
        // Set by the FilterLayerJob
        // Contains all Entities that satisfy the layer filtering for the RV
        std::vector<Entity *> filterEntitiesByLayer;

        // Set by the MaterialParameterGatherJob
        MaterialParameterGathererData materialParameterGatherer;

        // Set by the SyncRenderViewPreCommandUpdateJob
        // Contains caches of different filtering stages that can
        // be cached across frame
        std::vector<Entity *> layeredFilteredRenderables; // Changes rarely
        std::vector<Entity *> filteredAndCulledRenderables; // Changes if camera is modified
        std::vector<LightSource> layeredFilteredLightSources;

        // Cache of RenderCommands
        EntityRenderCommandDataViewPtr<RenderCommand> filteredRenderCommandDataViews;
    };

    // Variabled below are shared amongst all RV

    // Set by CachingRenderableEntityFilterJob
    std::vector<Entity *> renderableEntities;

    // Set by CachingComputableEntityFilterJob
    std::vector<Entity *> computeEntities;

    // Set by CachingLightGathererJob
    std::vector<LightSource> gatheredLights;

    EnvironmentLight* environmentLight;

    // Per RV cache
    // Leaves inserted by SyncRenderViewPostInitialization
    QHash<FrameGraphNode *, LeafNodeData> leafNodeCache;

    QMutex *mutex() { return &m_mutex; }

private:
    QMutex m_mutex;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERERCACHE_P_H
