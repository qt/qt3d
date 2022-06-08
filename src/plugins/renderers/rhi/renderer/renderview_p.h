// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RENDERVIEW_H
#define QT3DRENDER_RENDER_RHI_RENDERVIEW_H

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

#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/private/clearbuffers_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/attachmentpack_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qsortpolicy_p.h>
#include <Qt3DRender/private/lightsource_p.h>
#include <Qt3DRender/private/qmemorybarrier_p.h>
#include <Qt3DRender/private/qrendercapture_p.h>
#include <Qt3DRender/private/qblitframebuffer_p.h>
#include <Qt3DRender/private/qwaitfence_p.h>

#include <Qt3DCore/private/aligned_malloc_p.h>

#include <renderer_p.h>
// TODO: Move out once this is all refactored
#include <Qt3DRender/private/renderviewjobutils_p.h>

#include <vector>
#include <QSurface>
#include <QMutex>
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderPass;

namespace Render {

class NodeManagers;
class RenderPassFilter;
class TechniqueFilter;
class ViewportNode;
class Effect;
class RenderPass;

namespace Rhi {

class Renderer;
class RenderCommand;
class RHIShader;

typedef QPair<ShaderUniform, QVariant> ActivePropertyContent;
typedef QPair<QString, ActivePropertyContent> ActiveProperty;

using EntityRenderCommandData = Render::EntityRenderCommandData<RenderCommand>;
using EntityRenderCommandDataView = Render::EntityRenderCommandDataView<RenderCommand>;
using EntityRenderCommandDataViewPtr = Render::EntityRenderCommandDataViewPtr<RenderCommand>;
using EntityRenderCommandDataSubView = Render::EntityRenderCommandDataSubView<RenderCommand>;

struct Q_AUTOTEST_EXPORT ClearBufferInfo
{
    int drawBufferIndex = 0;
    QRenderTargetOutput::AttachmentPoint attchmentPoint = QRenderTargetOutput::Color0;
    QVector4D clearColor;
};

struct Q_AUTOTEST_EXPORT BlitFramebufferInfo
{
    Qt3DCore::QNodeId sourceRenderTargetId;
    Qt3DCore::QNodeId destinationRenderTargetId;
    QRect sourceRect;
    QRect destinationRect;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint sourceAttachmentPoint;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint destinationAttachmentPoint;
    QBlitFramebuffer::InterpolationMethod interpolationMethod;
};

struct RenderViewUBO
{
    float viewMatrix[16];
    float projectionMatrix[16];
    float uncorrectedProjectionMatrix[16];
    float clipCorrectionMatrix[16];
    float viewProjectionMatrix[16];
    float inverseViewMatrix[16];
    float inverseProjectionMatrix[16];
    float inverseViewProjectionMatrix[16];
    float viewportMatrix[16];
    float inverseViewportMatrix[16];
    float textureTransformMatrix[4];
    float eyePosition[3];
    float aspectRatio;
    float gamma;
    float exposure;
    float time;
    float yUpInNDC;
    float yUpInFBO;
};
static_assert(sizeof(RenderViewUBO) == sizeof(float) * (10 * 16 + 1 * 4 + 1 * 3 + 6 * 1),
              "UBO doesn't match std140");

class Q_AUTOTEST_EXPORT RenderView
{
public:
    RenderView();
    ~RenderView();

    QT3D_ALIGNED_MALLOC_AND_FREE()

    static void setRenderViewConfigFromFrameGraphLeafNode(RenderView *rv,
                                                          const FrameGraphNode *fgLeaf);

    // TODO: Add a way to specify a sort predicate for the RenderCommands
    void sort();

    void setRenderer(Renderer *renderer);
    inline void setSurfaceSize(const QSize &size) noexcept { m_surfaceSize = size; }
    inline Renderer *renderer() const noexcept { return m_renderer; }
    inline NodeManagers *nodeManagers() const noexcept { return m_manager; }
    inline const QSize &surfaceSize() const noexcept { return m_surfaceSize; }
    inline void setDevicePixelRatio(qreal r) noexcept { m_devicePixelRatio = r; }
    inline qreal devicePixelRatio() const noexcept { return m_devicePixelRatio; }

    inline void setRenderCameraLens(CameraLens *renderCameraLens) noexcept { m_renderCameraLens = renderCameraLens; }
    inline CameraLens *renderCameraLens() const noexcept { return m_renderCameraLens; }

    inline void setRenderCameraEntity(Entity *renderCameraNode) noexcept { m_renderCameraNode = renderCameraNode; }
    inline Entity *renderCameraEntity() const noexcept { return m_renderCameraNode; }

    inline void setViewMatrix(const Matrix4x4 &viewMatrix) noexcept { m_viewMatrix = viewMatrix; }
    inline Matrix4x4 viewMatrix() const noexcept { return m_viewMatrix; }

    inline void setViewProjectionMatrix(const Matrix4x4 &viewProjectionMatrix) noexcept { m_viewProjectionMatrix = viewProjectionMatrix; }
    inline Matrix4x4 viewProjectionMatrix() const noexcept { return m_viewProjectionMatrix; }

    inline void setEyePosition(const Vector3D &eyePos) noexcept { m_eyePos = eyePos; }
    inline Vector3D eyePosition() const noexcept { return m_eyePos; }

    inline void setEyeViewDirection(const Vector3D &dir) noexcept { m_eyeViewDir = dir; }
    inline Vector3D eyeViewDirection() const noexcept { return m_eyeViewDir; }

    inline void appendLayerFilter(const Qt3DCore::QNodeId layerFilterId) noexcept { m_layerFilterIds.push_back(layerFilterId); }
    inline Qt3DCore::QNodeIdVector layerFilters() const noexcept { return m_layerFilterIds; }

    inline void appendProximityFilterId(const Qt3DCore::QNodeId proximityFilterId) { m_proximityFilterIds.push_back(proximityFilterId); }
    inline Qt3DCore::QNodeIdVector proximityFilterIds() const { return m_proximityFilterIds; }

    inline void setRenderPassFilter(const RenderPassFilter *rpFilter) noexcept { m_passFilter = rpFilter; }
    inline const RenderPassFilter *renderPassFilter() const noexcept { return m_passFilter; }

    inline void setTechniqueFilter(const TechniqueFilter *filter) noexcept { m_techniqueFilter = filter; }
    inline const TechniqueFilter *techniqueFilter() const noexcept { return m_techniqueFilter; }

    inline void setRenderCommandDataView(const EntityRenderCommandDataViewPtr &renderCommandDataView) noexcept { m_renderCommandDataView = renderCommandDataView; }
    inline EntityRenderCommandDataViewPtr renderCommandDataView() const noexcept { return m_renderCommandDataView; }

    RenderStateSet *getOrCreateStateSet();
    RenderStateSet *stateSet() const noexcept { return m_stateSet.data(); }

    inline bool noDraw() const noexcept { return m_noDraw; }
    void setNoDraw(bool noDraw) noexcept { m_noDraw = noDraw; }

    inline bool isCompute() const noexcept { return m_compute; }
    void setCompute(bool compute) noexcept { m_compute = compute; }

    void setComputeWorkgroups(int x, int y, int z) noexcept { m_workGroups[0] = x; m_workGroups[1] = y; m_workGroups[2] = z; }
    const int *computeWorkGroups() const noexcept { return m_workGroups; }
    inline bool frustumCulling() const noexcept { return m_frustumCulling; }
    void setFrustumCulling(bool frustumCulling) noexcept { m_frustumCulling = frustumCulling; }
    bool showDebugOverlay() const noexcept { return m_showDebugOverlay; }
    void setShowDebugOverlay(bool showDebugOverlay) noexcept { m_showDebugOverlay = showDebugOverlay; }

    inline void setMaterialParameterTable(const MaterialParameterGathererData &parameters) noexcept { m_parameters = parameters; }

    // TODO: Get rid of this overly complex memory management by splitting out the
    // InnerData as a RenderViewConfig struct. This can be created by setRenderViewConfigFromFrameGraphLeafNode
    // and passed along with the RenderView to the functions that populate the renderview
    inline void setViewport(const QRectF &vp) noexcept { m_viewport = vp; }
    inline QRectF viewport() const noexcept { return m_viewport; }

    inline float gamma() const noexcept { return m_gamma; }
    inline void setGamma(float gamma) noexcept { m_gamma = gamma; }

    // depth and stencil ClearBuffers are cached locally
    // color ClearBuffers are collected, as there may be multiple
    // color buffers to be cleared. we need to apply all these at rendering
    void addClearBuffers(const ClearBuffers *cb);
    inline const std::vector<ClearBufferInfo> &specificClearColorBufferInfo() const { return m_specificClearColorBuffers; }
    inline std::vector<ClearBufferInfo> &specificClearColorBufferInfo() { return m_specificClearColorBuffers; }
    inline ClearBufferInfo globalClearColorBufferInfo() const { return m_globalClearColorBuffer; }

    inline QClearBuffers::BufferTypeFlags clearTypes() const { return m_clearBuffer; }
    inline float clearDepthValue() const { return m_clearDepthValue; }
    inline int clearStencilValue() const { return m_clearStencilValue; }

    RenderPassList passesAndParameters(ParameterInfoList *parameter, Entity *node, bool useDefaultMaterials = true);

    EntityRenderCommandData buildDrawRenderCommands(const Entity **entities,
                                                    int offset, int count) const;
    EntityRenderCommandData buildComputeRenderCommands(const Entity **entities,
                                                       int offset, int count) const;

    void updateRenderCommand(const EntityRenderCommandDataSubView &subView);

    void setRenderTargetId(Qt3DCore::QNodeId renderTargetId) noexcept { m_renderTarget = renderTargetId; }
    Qt3DCore::QNodeId renderTargetId() const noexcept { return m_renderTarget; }

    void addSortType(const QList<Qt3DRender::QSortPolicy::SortType> &sortTypes) { Qt3DCore::append(m_sortingTypes, sortTypes); }

    void setSurface(QSurface *surface) { m_surface = surface; }
    QSurface *surface() const { return m_surface; }

    void setLightSources(const std::vector<LightSource> &lightSources) noexcept { m_lightSources = lightSources; }
    void setEnvironmentLight(EnvironmentLight *environmentLight) noexcept { m_environmentLight = environmentLight; }

    void updateMatrices();

    inline void setRenderCaptureNodeId(const Qt3DCore::QNodeId nodeId) noexcept { m_renderCaptureNodeId = nodeId; }
    inline const Qt3DCore::QNodeId renderCaptureNodeId() const noexcept { return m_renderCaptureNodeId; }
    inline void setRenderCaptureRequest(const QRenderCaptureRequest& request) noexcept { m_renderCaptureRequest = request; }
    inline const QRenderCaptureRequest renderCaptureRequest() const noexcept { return m_renderCaptureRequest; }


    bool isDownloadBuffersEnable() const;
    void setIsDownloadBuffersEnable(bool isDownloadBuffersEnable);

    BlitFramebufferInfo blitFrameBufferInfo() const;
    void setBlitFrameBufferInfo(const BlitFramebufferInfo &blitFrameBufferInfo);

    bool hasBlitFramebufferInfo() const;
    void setHasBlitFramebufferInfo(bool hasBlitFramebufferInfo);

    bool shouldSkipSubmission() const;

    template<typename F>
    inline void forEachCommand(F func) const
    {
        if (!m_renderCommandDataView)
            return;
        m_renderCommandDataView->forEachCommand(func);
    }

    template<typename F>
    inline void forEachCommand(F func)
    {
        if (!m_renderCommandDataView)
            return;
        m_renderCommandDataView->forEachCommand(func);
    }

    inline int commandCount() const { return m_renderCommandDataView ? int(m_renderCommandDataView->size()) : 0; }

    inline const RenderViewUBO *renderViewUBO() const { return &m_renderViewUBO; }

private:
    void setShaderAndUniforms(RenderCommand *command, ParameterInfoList &parameters, const Entity *entity,
                              const std::vector<LightSource> &activeLightSources,
                              EnvironmentLight *environmentLight) const;

    Renderer *m_renderer = nullptr;
    NodeManagers *m_manager = nullptr;
    EntityRenderCommandDataViewPtr m_renderCommandDataView;

    QSize m_surfaceSize;
    float m_devicePixelRatio = 1.0f;
    QRectF m_viewport = QRectF(0.0f, 0.0f, 1.0f, 1.0f);
    float m_gamma = 2.2f;

    Qt3DCore::QNodeId m_renderCaptureNodeId;
    QRenderCaptureRequest m_renderCaptureRequest;
    bool m_isDownloadBuffersEnable = false;

    bool m_hasBlitFramebufferInfo = false;
    BlitFramebufferInfo m_blitFrameBufferInfo;

    QSurface *m_surface = nullptr;
    Qt3DCore::QNodeId m_renderTarget;
    AttachmentPack m_attachmentPack;
    QClearBuffers::BufferTypeFlags m_clearBuffer = QClearBuffers::None;
    float m_clearDepthValue = 1.0f;
    int m_clearStencilValue = 0;
    ClearBufferInfo m_globalClearColorBuffer;               // global ClearColor
    std::vector<ClearBufferInfo> m_specificClearColorBuffers;   // different draw buffers with distinct colors

    QScopedPointer<RenderStateSet> m_stateSet;
    CameraLens *m_renderCameraLens = nullptr;
    Entity *m_renderCameraNode = nullptr;
    const TechniqueFilter *m_techniqueFilter = nullptr;
    const RenderPassFilter *m_passFilter = nullptr;
    bool m_noDraw = false;
    bool m_compute = false;
    bool m_frustumCulling = false;
    bool m_showDebugOverlay = false;
    int m_workGroups[3] = { 1, 1, 1};
    std::vector<Qt3DRender::QSortPolicy::SortType> m_sortingTypes;
    Qt3DCore::QNodeIdVector m_proximityFilterIds;
    Qt3DCore::QNodeIdVector m_layerFilterIds;
    Matrix4x4 m_viewMatrix;
    Matrix4x4 m_viewProjectionMatrix;
    Matrix4x4 m_clipCorrectionMatrix;
    Vector3D m_eyePos;
    Vector3D m_eyeViewDir;

    MaterialParameterGathererData m_parameters;
    mutable std::vector<LightSource> m_lightSources;
    EnvironmentLight *m_environmentLight = nullptr;

    RenderViewUBO m_renderViewUBO;

    void setUniformValue(ShaderParameterPack &uniformPack, int nameId,
                         const UniformValue &value) const;
    void setUniformBlockValue(ShaderParameterPack &uniformPack, const RHIShader *shader,
                              const ShaderUniformBlock &block, const UniformValue &value) const;
    void setShaderStorageValue(ShaderParameterPack &uniformPack, const RHIShader *shader,
                               const ShaderStorageBlock &block, const UniformValue &value) const;
    void setShaderDataValue(ShaderParameterPack &uniformPack,
                            const ShaderUniformBlock &block,
                            const Qt3DCore::QNodeId &shaderDataId) const;
    void setDefaultUniformBlockShaderDataValue(ShaderParameterPack &uniformPack,
                                               const RHIShader *shader,
                                               ShaderData *shaderData,
                                               const QString &structName) const;
    void applyParameter(const Parameter *param, RenderCommand *command,
                        const RHIShader *shader) const noexcept;
};

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_ENDERVIEW_H
