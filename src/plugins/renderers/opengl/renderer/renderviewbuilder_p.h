// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERVIEWBUILDER_H
#define QT3DRENDER_RENDER_OPENGL_RENDERVIEWBUILDER_H

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

#include <functional>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/filterlayerentityjob_p.h>
#include <Qt3DRender/private/genericlambdajob_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/frustumcullingjob_p.h>
#include <Qt3DRender/private/filterproximitydistancejob_p.h>
#include <Qt3DRender/private/materialparametergathererjob_p.h>
#include <Qt3DRender/private/renderviewcommandbuilderjob_p.h>
#include <Qt3DRender/private/renderviewcommandupdaterjob_p.h>
#include <Qt3DRender/private/rendersyncjobs_p.h>
#include <rendercommand_p.h>
#include <renderview_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

class Renderer;

using SynchronizerJobPtr = GenericLambdaJobPtr<std::function<void()>>;
#define CreateSynchronizerJobPtr(lambda, type, instance) \
    SynchronizerJobPtr::create(lambda, type, #type, instance)

using RenderViewCommandBuilderJobPtr = Render::RenderViewCommandBuilderJobPtr<RenderView, RenderCommand>;
using RenderViewCommandUpdaterJobPtr = Render::RenderViewCommandUpdaterJobPtr<RenderView, RenderCommand>;
using RenderViewInitializerJobPtr = Render::RenderViewInitializerJobPtr<RenderView, Renderer>;

class Q_AUTOTEST_EXPORT RenderViewBuilder
{
public:
    explicit RenderViewBuilder(Render::FrameGraphNode *leafNode, int renderViewIndex, Renderer *renderer);

    RenderViewInitializerJobPtr renderViewJob() const;
    FilterLayerEntityJobPtr filterEntityByLayerJob() const;
    FrustumCullingJobPtr frustumCullingJob() const;
    const std::vector<RenderViewCommandBuilderJobPtr> &renderViewCommandBuilderJobs() const;
    const std::vector<RenderViewCommandUpdaterJobPtr> &renderViewCommandUpdaterJobs() const;
    const std::vector<MaterialParameterGathererJobPtr> &materialGathererJobs() const;
    SynchronizerJobPtr syncRenderViewPostInitializationJob() const;
    SynchronizerJobPtr syncPreFrustumCullingJob() const;
    SynchronizerJobPtr syncRenderViewPreCommandBuildingJob() const;
    SynchronizerJobPtr syncRenderViewPreCommandUpdateJob() const;
    SynchronizerJobPtr syncRenderViewPostCommandUpdateJob() const;
    SynchronizerJobPtr setClearDrawBufferIndexJob() const;
    SynchronizerJobPtr syncFilterEntityByLayerJob() const;
    FilterProximityDistanceJobPtr filterProximityJob() const;
    SynchronizerJobPtr syncMaterialGathererJob() const;

    void prepareJobs();
    std::vector<Qt3DCore::QAspectJobPtr> buildJobHierachy() const;

    Renderer *renderer() const;
    int renderViewIndex() const;

    void setLayerCacheNeedsToBeRebuilt(bool needsToBeRebuilt);
    bool layerCacheNeedsToBeRebuilt() const;
    void setMaterialGathererCacheNeedsToBeRebuilt(bool needsToBeRebuilt);
    bool materialGathererCacheNeedsToBeRebuilt() const;
    void setRenderCommandCacheNeedsToBeRebuilt(bool needsToBeRebuilt);
    bool renderCommandCacheNeedsToBeRebuilt() const;
    void setLightCacheNeedsToBeRebuilt(bool needsToBeRebuilt);
    bool lightCacheNeedsToBeRebuilt() const;

    int optimalJobCount() const;
    void setOptimalJobCount(int v);

private:
    Render::FrameGraphNode *m_leafNode;
    const int m_renderViewIndex;
    Renderer *m_renderer;
    RebuildFlagSet m_rebuildFlags;

    RenderViewInitializerJobPtr m_renderViewJob;
    FilterLayerEntityJobPtr m_filterEntityByLayerJob;
    FrustumCullingJobPtr m_frustumCullingJob;
    std::vector<RenderViewCommandBuilderJobPtr> m_renderViewCommandBuilderJobs;
    std::vector<RenderViewCommandUpdaterJobPtr> m_renderViewCommandUpdaterJobs;
    std::vector<MaterialParameterGathererJobPtr> m_materialGathererJobs;

    SynchronizerJobPtr m_syncRenderViewPostInitializationJob;
    SynchronizerJobPtr m_syncPreFrustumCullingJob;
    SynchronizerJobPtr m_syncRenderViewPreCommandBuildingJob;
    SynchronizerJobPtr m_syncRenderViewPreCommandUpdateJob;
    SynchronizerJobPtr m_syncRenderViewPostCommandUpdateJob;
    SynchronizerJobPtr m_setClearDrawBufferIndexJob;
    SynchronizerJobPtr m_syncFilterEntityByLayerJob;
    SynchronizerJobPtr m_syncMaterialGathererJob;
    FilterProximityDistanceJobPtr m_filterProximityJob;

    int m_optimalParallelJobCount;
};

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERVIEWBUILDER_H
