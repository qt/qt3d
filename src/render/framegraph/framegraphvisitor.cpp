/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 Paul Lemire
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

#include "framegraphvisitor_p.h"

#include "framegraphnode_p.h"
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/filterentitybycomponentjob_p.h>
#include <Qt3DRender/private/filterlayerentityjob_p.h>
#include <Qt3DRender/private/genericlambdajob_p.h>
#include <Qt3DRender/private/materialparametergathererjob_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/renderviewbuilderjob_p.h>
#include <Qt3DRender/private/renderview_p.h>
#include <Qt3DRender/private/frustumcullingjob_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

FrameGraphVisitor::FrameGraphVisitor(Renderer *renderer,
                                     const FrameGraphManager *manager)
    : m_renderer(renderer)
    , m_manager(manager)
    , m_jobs(nullptr)
    , m_renderviewIndex(0)

{
}

void FrameGraphVisitor::traverse(FrameGraphNode *root,
                                 QVector<Qt3DCore::QAspectJobPtr> *jobs)
{
    m_jobs = jobs;
    m_renderviewIndex = 0;

    Q_ASSERT(m_renderer);
    Q_ASSERT(m_jobs);
    Q_ASSERT_X(root, Q_FUNC_INFO, "The FrameGraphRoot is null");

    // Kick off the traversal
    Render::FrameGraphNode *node = root;
    if (node == nullptr)
        qCritical() << Q_FUNC_INFO << "FrameGraph is null";
    visit(node);
}

void FrameGraphVisitor::visit(Render::FrameGraphNode *node)
{
    // TO DO: check if node is a subtree selector
    // in which case, we only visit the subtrees returned
    // by the selector functor and not all the children
    // as we would otherwise do

    // Recurse to children (if we have any), otherwise if this is a leaf node,
    // initiate a rendering from the current camera
    const QVector<Qt3DCore::QNodeId> fgChildIds = node->childrenIds();

    for (const Qt3DCore::QNodeId fgChildId : fgChildIds)
        visit(m_manager->lookupNode(fgChildId));

    // Leaf node - create a RenderView ready to be populated
    // TODO: Pass in only framegraph config that has changed from previous
    // index RenderViewJob.
    if (fgChildIds.empty()) {
        // 1) Prepare parameter pack for all possible Pass Filter / Technique Filter
        // At most 1 of each per RenderView -> not very frequent -> doesn't need to be in a job

        // 1) Update all the ShaderData

        // 1) RenderView initialization from
        //     2) One job to filter out the Entities in a layers (no dependency)
        //     2) -> One job to filter out only the Entities with a QDispatchCompute
        //     2) -> One job to filter out only the Entities with a GeometryRenderer
        //     2) -> One job to create all ParameterInfoList for each Material
        //     2) -> One job to perform frustrum culling of layered filtered entities

        //          -> 3) Merge Parameter pack 1 / 2 + Material
        //          ->      4) One job to prepare the commands
        //                  -> One job to sort the RenderCommands
        //                  -> One job to set the active uniforms / build the ParameterPack

        // GenericLambdaJob will be used to sync data between jobs and their dependencies
        // Prefer linear iteration over tree traversal

        const int currentRenderViewIndex = m_renderviewIndex++;
        RenderViewInitializerJobPtr renderViewJob = m_renderer->createRenderViewInitializerJob(node, currentRenderViewIndex);
        auto renderViewCommandBuilder = Render::RenderViewBuilderJobPtr::create();
        auto filterEntityByLayer = Render::FilterLayerEntityJobPtr::create();
        auto renderableEntityFilterer = Render::FilterEntityByComponentJobPtr<Render::GeometryRenderer, Render::Material>::create();

        // Note: do it only if OpenGL 4.3+ available
        auto computeEntityFilterer = Render::FilterEntityByComponentJobPtr<Render::ComputeCommand, Render::Material>::create();

        auto frustumCulling = Render::FrustumCullingJobPtr::create();

        // Init what we can here
        EntityManager *entityManager = m_renderer->nodeManagers()->renderNodesManager();
        filterEntityByLayer->setManager(entityManager);
        renderableEntityFilterer->setManager(entityManager);
        computeEntityFilterer->setManager(entityManager);
        frustumCulling->setRoot(m_renderer->sceneRoot());
        renderViewCommandBuilder->setIndex(currentRenderViewIndex);
        renderViewCommandBuilder->setRenderer(m_renderer);

        // Since Material gathering is an heavy task, we split it
        QVector<Render::MaterialParameterGathererJobPtr> materialGatherers;
        { // Scoped to avoid copy in lambdas
            const QVector<HMaterial> materialHandles = m_renderer->nodeManagers()->materialManager()->activeHandles();
            const int materialGathererCount = std::max(materialHandles.size() / 8, 1);
            materialGatherers.reserve(materialGathererCount);
            for (auto i = 0; i < materialGathererCount; ++i) {
                auto materialGatherer = Render::MaterialParameterGathererJobPtr::create();
                materialGatherer->setNodeManagers(m_renderer->nodeManagers());
                materialGatherer->setRenderer(m_renderer);
                materialGatherer->setHandles(materialHandles.mid(i * 8, 8));
                materialGatherers.push_back(materialGatherer);
            }
        }

        // Copy shared ptr -> this is called once the FrameGraphBranch was used to fill initial data in the RenderView
        auto syncRenderViewInitialization = [=] () {
            RenderView *rv = renderViewJob->renderView();

            // Layer filtering
            filterEntityByLayer->setHasLayerFilter(rv->hasLayerFilter());
            filterEntityByLayer->setLayers(rv->layerFilter());

            // Material Parameter building
            for (const auto materialGatherer : qAsConst(materialGatherers)) {
                materialGatherer->setRenderPassFilter(const_cast<RenderPassFilter *>(rv->renderPassFilter()));
                materialGatherer->setTechniqueFilter(const_cast<TechniqueFilter *>(rv->techniqueFilter()));
            }

            // Frustum culling
            frustumCulling->setViewProjection(rv->viewProjectionMatrix());
        };

        // Copy shared ptr -> this is called after filtering / culling / parameter setting has been performed
        auto syncForRenderCommandBuilding = [=] () {
            // Set the result of previous job computations
            // for final RenderCommand building
            RenderView *rv = renderViewJob->renderView();

            if (!rv->noDraw()) {
                // Remove all entities from the compute and renderable vectors that aren't in the filtered layer vector
                const QVector<Entity *> filteredEntities = filterEntityByLayer->filteredEntities();

                // Note: this could further be improved if needed
                // Set the renderable and computable entities
                if (!rv->isCompute()) {
                    QVector<Entity *> renderableEntities = renderableEntityFilterer->filteredEntities();

                    for (auto i = renderableEntities.size() - 1; i >= 0; --i) {
                        if (!filteredEntities.contains(renderableEntities.at(i)))
                            renderableEntities.removeAt(i);
                    }

                    if (rv->frustumCulling()) {
                        const QVector<Entity *> visibleEntities = frustumCulling->visibleEntities();
                        for (auto i = renderableEntities.size() - 1; i >= 0; --i) {
                            if (!visibleEntities.contains(renderableEntities.at(i)))
                                renderableEntities.removeAt(i);
                        }
                    }

                    rv->setRenderables(std::move(renderableEntities));
                } else {
                    QVector<Entity *> computableEntities = computeEntityFilterer->filteredEntities();
                    for (auto i = computableEntities.size() - 1; i >= 0; --i) {
                        if (!filteredEntities.contains(computableEntities.at(i)))
                            computableEntities.removeAt(i);
                    }
                    rv->setComputables(std::move(computableEntities));
                }

                // Reduction
                QHash<Qt3DCore::QNodeId, QVector<RenderPassParameterData>> params;
                for (const auto materialGatherer : qAsConst(materialGatherers))
                    params.unite(materialGatherer->materialToPassAndParameter());
                // Set all required data on the RenderView for final processing
                rv->setMaterialParameterTable(std::move(params));
            }
            renderViewCommandBuilder->setRenderView(renderViewJob->renderView());
        };

        auto syncRenderViewCommandBuildingJob = GenericLambdaJobPtr<decltype(syncForRenderCommandBuilding)>::create(syncForRenderCommandBuilding);
        auto syncRenderViewInitializationJob = GenericLambdaJobPtr<decltype(syncRenderViewInitialization)>::create(syncRenderViewInitialization);

        // Set dependencies
        syncRenderViewInitializationJob->addDependency(renderViewJob);

        filterEntityByLayer->addDependency(syncRenderViewInitializationJob);
        frustumCulling->addDependency(syncRenderViewInitializationJob);

        syncRenderViewCommandBuildingJob->addDependency(syncRenderViewInitializationJob);
        for (const auto materialGatherer : qAsConst(materialGatherers)) {
            materialGatherer->addDependency(syncRenderViewInitializationJob);
            syncRenderViewCommandBuildingJob->addDependency(materialGatherer);
        }
        syncRenderViewCommandBuildingJob->addDependency(renderableEntityFilterer);
        syncRenderViewCommandBuildingJob->addDependency(computeEntityFilterer);
        syncRenderViewCommandBuildingJob->addDependency(filterEntityByLayer);

        renderViewCommandBuilder->addDependency(syncRenderViewCommandBuildingJob);

        // Add jobs
        m_jobs->push_back(renderViewJob);
        m_jobs->push_back(filterEntityByLayer);
        m_jobs->push_back(renderableEntityFilterer);
        m_jobs->push_back(computeEntityFilterer);
        m_jobs->push_back(syncRenderViewInitializationJob);
        m_jobs->push_back(syncRenderViewCommandBuildingJob);
        m_jobs->push_back(renderViewCommandBuilder);
        m_jobs->push_back(frustumCulling);
        for (const auto materialGatherer : qAsConst(materialGatherers))
            m_jobs->push_back(materialGatherer);
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
