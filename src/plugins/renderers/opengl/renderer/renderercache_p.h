/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERERCACHE_P_H
#define QT3DRENDER_RENDER_OPENGL_RENDERERCACHE_P_H

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

#include <Qt3DRender/QFrameGraphNode>

#include <Qt3DRender/private/entity_p.h>
#include <renderviewjobutils_p.h>
#include <Qt3DRender/private/lightsource_p.h>
#include <rendercommand_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

struct RendererCache
{
    struct LeafNodeData
    {
        Matrix4x4 viewProjectionMatrix;
        // Set by the FilterLayerJob
        // Contains all Entities that satisfy the layer filtering for the RV
        QVector<Entity *> filterEntitiesByLayer;

        // Set by the MaterialParameterGatherJob
        MaterialParameterGathererData materialParameterGatherer;

        // Set by the SyncRenderViewPreCommandUpdateJob
        // Contains caches of different filtering stages that can
        // be cached across frame
        QVector<Entity *> layeredFilteredRenderables; // Changes rarely
        QVector<Entity *> filteredAndCulledRenderables; // Changes if camera is modified
        QVector<LightSource> layeredFilteredLightSources;

        // We keep two sets of filtered render commands
        // and we flip in between them every frame
        int viewIdx = 0;
        bool requestFilteringAtNextFrame = false;
        EntityRenderCommandDataViewPtr filteredRenderCommandDataViews[2];
    };

    // Variabled below are shared amongst all RV

    // Set by CachingRenderableEntityFilterJob
    QVector<Entity *> renderableEntities;

    // Set by CachingComputableEntityFilterJob
    QVector<Entity *> computeEntities;

    // Set by CachingLightGathererJob
    QVector<LightSource> gatheredLights;

    EnvironmentLight* environmentLight;

    // Per RV cache
    // Leaves inserted by SyncRenderViewPostInitialization
    QHash<FrameGraphNode *, LeafNodeData> leafNodeCache;

    QMutex *mutex() { return &m_mutex; }

private:
    QMutex m_mutex;
};

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERERCACHE_P_H
