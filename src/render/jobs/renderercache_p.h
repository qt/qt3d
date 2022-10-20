/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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
