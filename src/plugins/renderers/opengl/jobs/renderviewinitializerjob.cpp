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

#include "renderviewinitializerjob_p.h"

#include <renderview_p.h>
#include <renderer_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/job_common_p.h>

#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/clearbuffers_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/rendertargetselectornode_p.h>
#include <Qt3DRender/private/sortpolicy_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/statesetnode_p.h>
#include <Qt3DRender/private/dispatchcompute_p.h>
#include <Qt3DRender/private/rendersurfaceselector_p.h>
#include <Qt3DRender/private/rendercapture_p.h>
#include <Qt3DRender/private/buffercapture_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/private/techniquemanager_p.h>
#include <Qt3DRender/private/memorybarrier_p.h>
#include <Qt3DRender/private/blitframebuffer_p.h>
#include <Qt3DRender/private/waitfence_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/private/rendercapture_p.h>

#include <QElapsedTimer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

namespace {
// only accessed in ctor and dtor of RenderViewJob
// which are always being called in a non concurrent manner
int renderViewInstanceCounter = 0;
} // anonymous


/*!
    \internal
    Walks up the framegraph tree from \a fgLeaf and builds up as much state
    as possible and populates \a rv. For cases where we can't get the specific state
    (e.g. because it depends upon more than just the framegraph) we store the data from
    the framegraph that will be needed to later when the rest of the data becomes available
*/
void RenderViewInitializerJob::setRenderViewConfigFromFrameGraphLeafNode(RenderView *rv, const FrameGraphNode *fgLeaf)
{
    // The specific RenderPass to be used is also dependent upon the Effect and TechniqueFilter
    // which is referenced by the Material which is referenced by the RenderMesh. So we can
    // only store the filter info in the RenderView structure and use it to do the resolving
    // when we build the RenderCommand list.
    const NodeManagers *manager = rv->nodeManagers();
    const FrameGraphNode *node = fgLeaf;

    while (node) {
        FrameGraphNode::FrameGraphNodeType type = node->nodeType();
        if (node->isEnabled())
            switch (type) {
            case FrameGraphNode::InvalidNodeType:
                // A base FrameGraphNode, can be used for grouping purposes
                break;
            case FrameGraphNode::CameraSelector:
                // Can be set only once and we take camera nearest to the leaf node
                if (!rv->renderCameraLens()) {
                    const CameraSelector *cameraSelector = static_cast<const CameraSelector *>(node);
                    Entity *camNode = manager->renderNodesManager()->lookupResource(cameraSelector->cameraUuid());
                    if (camNode) {
                        CameraLens *lens = camNode->renderComponent<CameraLens>();
                        rv->setRenderCameraEntity(camNode);
                        if (lens && lens->isEnabled()) {
                            rv->setRenderCameraLens(lens);
                            // ViewMatrix and ProjectionMatrix are computed
                            // later in updateMatrices()
                            // since at this point the transformation matrices
                            // may not yet have been updated
                        }
                    }
                }
                break;

            case FrameGraphNode::LayerFilter: // Can be set multiple times in the tree
                rv->appendLayerFilter(static_cast<const LayerFilterNode *>(node)->peerId());
                break;

            case FrameGraphNode::ProximityFilter: // Can be set multiple times in the tree
                rv->appendProximityFilterId(node->peerId());
                break;

            case FrameGraphNode::RenderPassFilter:
                // Can be set once
                // TODO: Amalgamate all render pass filters from leaf to root
                if (!rv->renderPassFilter())
                    rv->setRenderPassFilter(static_cast<const RenderPassFilter *>(node));
                break;

            case FrameGraphNode::RenderTarget: {
                // Can be set once and we take render target nearest to the leaf node
                const RenderTargetSelector *targetSelector = static_cast<const RenderTargetSelector *>(node);
                Qt3DCore::QNodeId renderTargetUid = targetSelector->renderTargetUuid();
                HTarget renderTargetHandle = manager->renderTargetManager()->lookupHandle(renderTargetUid);

                // Add renderTarget Handle and build renderCommand AttachmentPack
                if (!rv->renderTargetId()) {
                    rv->setRenderTargetId(renderTargetUid);

                    RenderTarget *renderTarget = manager->renderTargetManager()->data(renderTargetHandle);
                    if (renderTarget)
                        rv->setAttachmentPack(AttachmentPack(renderTarget, manager->attachmentManager(), targetSelector->outputs()));
                }
                break;
            }

            case FrameGraphNode::ClearBuffers: {
                const ClearBuffers *cbNode = static_cast<const ClearBuffers *>(node);
                rv->addClearBuffers(cbNode);
                break;
            }

            case FrameGraphNode::TechniqueFilter:
                // Can be set once
                // TODO Amalgamate all technique filters from leaf to root
                if (!rv->techniqueFilter())
                    rv->setTechniqueFilter(static_cast<const TechniqueFilter *>(node));
                break;

            case FrameGraphNode::Viewport: {
                // If the Viewport has already been set in a lower node
                // Make it so that the new viewport is actually
                // a subregion relative to that of the parent viewport
                const ViewportNode *vpNode = static_cast<const ViewportNode *>(node);
                rv->setViewport(ViewportNode::computeViewport(rv->viewport(), vpNode));
                rv->setGamma(vpNode->gamma());
                break;
            }

            case FrameGraphNode::SortMethod: {
                const Render::SortPolicy *sortPolicy = static_cast<const Render::SortPolicy *>(node);
                rv->addSortType(sortPolicy->sortTypes());
                break;
            }

            case FrameGraphNode::SubtreeEnabler:
                // Has no meaning here. SubtreeEnabler was used
                // in a prior step to filter the list of RenderViewJobs
                break;

            case FrameGraphNode::StateSet: {
                const Render::StateSetNode *rStateSet = static_cast<const Render::StateSetNode *>(node);
                // Add states from new stateSet we might be missing
                // but don' t override existing states (lower StateSetNode always has priority)
                if (rStateSet->hasRenderStates()) {
                    // Create global RenderStateSet for renderView if no stateSet was set before
                    RenderStateSet *stateSet = rv->getOrCreateStateSet();
                    addStatesToRenderStateSet(stateSet, rStateSet->renderStates(), manager->renderStateManager());
                }
                break;
            }

            case FrameGraphNode::NoDraw: {
                rv->setNoDraw(true);
                break;
            }

            case FrameGraphNode::FrustumCulling: {
                rv->setFrustumCulling(true);
                break;
            }

            case FrameGraphNode::ComputeDispatch: {
                const Render::DispatchCompute *dispatchCompute = static_cast<const Render::DispatchCompute *>(node);
                rv->setCompute(true);
                rv->setComputeWorkgroups(dispatchCompute->x(),
                                         dispatchCompute->y(),
                                         dispatchCompute->z());
                break;
            }

            case FrameGraphNode::Lighting: {
                // TODO
                break;
            }

            case FrameGraphNode::Surface: {
                // Use the surface closest to leaf node
                if (rv->surface() == nullptr) {
                    const Render::RenderSurfaceSelector *surfaceSelector
                            = static_cast<const Render::RenderSurfaceSelector *>(node);
                    rv->setSurface(surfaceSelector->surface());
                    rv->setSurfaceSize(surfaceSelector->renderTargetSize() * surfaceSelector->devicePixelRatio());
                    rv->setDevicePixelRatio(surfaceSelector->devicePixelRatio());
                }
                break;
            }

            case FrameGraphNode::DebugOverlay:
                rv->setShowDebugOverlay(true);
                break;

            case FrameGraphNode::RenderCapture: {
                auto *renderCapture = const_cast<Render::RenderCapture *>(
                                            static_cast<const Render::RenderCapture *>(node));
                if (rv->renderCaptureNodeId().isNull() && renderCapture->wasCaptureRequested()) {
                    rv->setRenderCaptureNodeId(renderCapture->peerId());
                    rv->setRenderCaptureRequest(renderCapture->takeCaptureRequest());
                }
                break;
            }

            case FrameGraphNode::MemoryBarrier: {
                const Render::MemoryBarrier *barrier = static_cast<const Render::MemoryBarrier *>(node);
                rv->setMemoryBarrier(barrier->waitOperations()|rv->memoryBarrier());
                break;
            }

            case FrameGraphNode::BufferCapture: {
                auto *bufferCapture = const_cast<Render::BufferCapture *>(
                                            static_cast<const Render::BufferCapture *>(node));
                if (bufferCapture != nullptr)
                     rv->setIsDownloadBuffersEnable(bufferCapture->isEnabled());
                break;
            }

            case FrameGraphNode::BlitFramebuffer: {
                const Render::BlitFramebuffer *blitFramebufferNode =
                        static_cast<const Render::BlitFramebuffer *>(node);
                rv->setHasBlitFramebufferInfo(true);
                BlitFramebufferInfo bfbInfo;
                bfbInfo.sourceRenderTargetId = blitFramebufferNode->sourceRenderTargetId();
                bfbInfo.destinationRenderTargetId = blitFramebufferNode->destinationRenderTargetId();
                bfbInfo.sourceRect = blitFramebufferNode->sourceRect();
                bfbInfo.destinationRect = blitFramebufferNode->destinationRect();
                bfbInfo.sourceAttachmentPoint = blitFramebufferNode->sourceAttachmentPoint();
                bfbInfo.destinationAttachmentPoint = blitFramebufferNode->destinationAttachmentPoint();
                bfbInfo.interpolationMethod = blitFramebufferNode->interpolationMethod();
                rv->setBlitFrameBufferInfo(bfbInfo);
                break;
            }

            case FrameGraphNode::WaitFence: {
                const Render::WaitFence *waitFence = static_cast<const Render::WaitFence *>(node);
                rv->appendWaitFence(waitFence->data());
                break;
            }

            case FrameGraphNode::SetFence: {
                rv->appendInsertFenceId(node->peerId());
                break;
            }

            case FrameGraphNode::NoPicking:
                // Nothing to do RenderView wise for NoPicking
                break;

            default:
                // Should never get here
                qCWarning(Backend) << "Unhandled FrameGraphNode type";
            }

        node = node->parent();
    }
}

RenderViewInitializerJob::RenderViewInitializerJob()
    : m_renderer(nullptr)
    , m_fgLeaf(nullptr)
    , m_index(0)
    , m_renderView(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::RenderView, renderViewInstanceCounter++)
}

RenderViewInitializerJob::~RenderViewInitializerJob()
{
    renderViewInstanceCounter--;
}

void RenderViewInitializerJob::run()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_index;
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    QElapsedTimer timer;
    timer.start();
    qint64 gatherLightsTime;
    qint64 buildCommandsTime;
#endif

    // Create a RenderView object
    m_renderView = new RenderView;

    // RenderView should allocate heap resources using only the currentFrameAllocator
    m_renderView->setRenderer(m_renderer);

    // Populate the renderview's configuration from the framegraph
    setRenderViewConfigFromFrameGraphLeafNode(m_renderView, m_fgLeaf);
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    qint64 gatherStateTime = timer.nsecsElapsed();
    timer.restart();
#endif
}

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
