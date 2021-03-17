/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERVIEW_H
#define QT3DRENDER_RENDER_OPENGL_RENDERVIEW_H

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

#include <Qt3DCore/private/qframeallocator_p.h>
#include <Qt3DRender/private/aligned_malloc_p.h>

#include <renderer_p.h>
// TODO: Move out once this is all refactored
#include <renderviewjobutils_p.h>

#include <QVector>
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

namespace OpenGL {

class Renderer;
class RenderCommand;

typedef QPair<ShaderUniform, QVariant> ActivePropertyContent;
typedef QPair<QString, ActivePropertyContent > ActiveProperty;

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

// This class is kind of analogous to RenderBin but I want to avoid trampling
// on that until we get this working

class Q_AUTOTEST_EXPORT RenderView
{
public:
    RenderView();
    ~RenderView();

    QT3D_ALIGNED_MALLOC_AND_FREE()

    // TODO: Add a way to specify a sort predicate for the RenderCommands
    void sort();

    void setRenderer(Renderer *renderer);
    inline void setSurfaceSize(const QSize &size) Q_DECL_NOTHROW { m_surfaceSize = size; }
    inline Renderer *renderer() const Q_DECL_NOTHROW { return m_renderer; }
    inline NodeManagers *nodeManagers() const Q_DECL_NOTHROW { return m_manager; }
    inline const QSize &surfaceSize() const Q_DECL_NOTHROW { return m_surfaceSize; }
    inline void setDevicePixelRatio(qreal r) Q_DECL_NOTHROW { m_devicePixelRatio = r; }
    inline qreal devicePixelRatio() const Q_DECL_NOTHROW { return m_devicePixelRatio; }

    inline void setRenderCameraLens(CameraLens *renderCameraLens) Q_DECL_NOTHROW { m_renderCameraLens = renderCameraLens; }
    inline CameraLens *renderCameraLens() const Q_DECL_NOTHROW { return m_renderCameraLens; }

    inline void setRenderCameraEntity(Entity *renderCameraNode) Q_DECL_NOTHROW { m_renderCameraNode = renderCameraNode; }
    inline Entity *renderCameraEntity() const Q_DECL_NOTHROW { return m_renderCameraNode; }

    inline void setViewMatrix(const Matrix4x4 &viewMatrix) Q_DECL_NOTHROW { m_viewMatrix = viewMatrix; }
    inline Matrix4x4 viewMatrix() const Q_DECL_NOTHROW { return m_viewMatrix; }

    inline void setViewProjectionMatrix(const Matrix4x4 &viewProjectionMatrix) Q_DECL_NOTHROW { m_viewProjectionMatrix = viewProjectionMatrix; }
    inline Matrix4x4 viewProjectionMatrix() const Q_DECL_NOTHROW { return m_viewProjectionMatrix; }

    inline void setEyePosition(const Vector3D &eyePos) Q_DECL_NOTHROW { m_eyePos = eyePos; }
    inline Vector3D eyePosition() const Q_DECL_NOTHROW { return m_eyePos; }

    inline void setEyeViewDirection(const Vector3D &dir) Q_DECL_NOTHROW { m_eyeViewDir = dir; }
    inline Vector3D eyeViewDirection() const Q_DECL_NOTHROW { return m_eyeViewDir; }

    inline void appendLayerFilter(const Qt3DCore::QNodeId layerFilterId) Q_DECL_NOTHROW { m_layerFilterIds.push_back(layerFilterId); }
    inline Qt3DCore::QNodeIdVector layerFilters() const Q_DECL_NOTHROW { return m_layerFilterIds; }

    inline void appendProximityFilterId(const Qt3DCore::QNodeId proximityFilterId) { m_proximityFilterIds.push_back(proximityFilterId); }
    inline Qt3DCore::QNodeIdVector proximityFilterIds() const { return m_proximityFilterIds; }

    inline void appendInsertFenceId(const Qt3DCore::QNodeId setFenceId) { m_insertFenceIds.push_back(setFenceId); }
    // We prefix with get to avoid confusion when it is called
    inline Qt3DCore::QNodeIdVector insertFenceIds() const { return m_insertFenceIds; }

    inline void appendWaitFence(const QWaitFenceData &data) { m_waitFences.push_back(data); }
    inline QVector<QWaitFenceData> waitFences() const { return m_waitFences; }

    inline void setRenderPassFilter(const RenderPassFilter *rpFilter) Q_DECL_NOTHROW { m_passFilter = rpFilter; }
    inline const RenderPassFilter *renderPassFilter() const Q_DECL_NOTHROW { return m_passFilter; }

    inline void setTechniqueFilter(const TechniqueFilter *filter) Q_DECL_NOTHROW { m_techniqueFilter = filter; }
    inline const TechniqueFilter *techniqueFilter() const Q_DECL_NOTHROW { return m_techniqueFilter; }

    inline void setRenderCommandDataView(const EntityRenderCommandDataViewPtr &renderCommandDataView) Q_DECL_NOTHROW { m_renderCommandDataView = renderCommandDataView; }
    inline EntityRenderCommandDataViewPtr renderCommandDataView() const Q_DECL_NOTHROW { return m_renderCommandDataView; }

    RenderStateSet *getOrCreateStateSet();
    RenderStateSet *stateSet() const Q_DECL_NOTHROW { return m_stateSet.data(); }

    inline bool noDraw() const Q_DECL_NOTHROW { return m_noDraw; }
    void setNoDraw(bool noDraw) Q_DECL_NOTHROW { m_noDraw = noDraw; }

    inline bool isCompute() const Q_DECL_NOTHROW { return m_compute; }
    void setCompute(bool compute) Q_DECL_NOTHROW { m_compute = compute; }

    void setComputeWorkgroups(int x, int y, int z) Q_DECL_NOTHROW { m_workGroups[0] = x; m_workGroups[1] = y; m_workGroups[2] = z; }
    const int *computeWorkGroups() const Q_DECL_NOTHROW { return m_workGroups; }
    inline bool frustumCulling() const Q_DECL_NOTHROW { return m_frustumCulling; }
    void setFrustumCulling(bool frustumCulling) Q_DECL_NOTHROW { m_frustumCulling = frustumCulling; }
    bool showDebugOverlay() const Q_DECL_NOTHROW { return m_showDebugOverlay; }
    void setShowDebugOverlay(bool showDebugOverlay) Q_DECL_NOTHROW { m_showDebugOverlay = showDebugOverlay; }

    inline void setMaterialParameterTable(const MaterialParameterGathererData &parameters) Q_DECL_NOTHROW { m_parameters = parameters; }

    // TODO: Get rid of this overly complex memory management by splitting out the
    // InnerData as a RenderViewConfig struct. This can be created by setRenderViewConfigFromFrameGraphLeafNode
    // and passed along with the RenderView to the functions that populate the renderview
    inline void setViewport(const QRectF &vp) Q_DECL_NOTHROW { m_viewport = vp; }
    inline QRectF viewport() const Q_DECL_NOTHROW { return m_viewport; }

    inline float gamma() const Q_DECL_NOTHROW { return m_gamma; }
    inline void setGamma(float gamma) Q_DECL_NOTHROW { m_gamma = gamma; }

    // depth and stencil ClearBuffers are cached locally
    // color ClearBuffers are collected, as there may be multiple
    // color buffers to be cleared. we need to apply all these at rendering
    void addClearBuffers(const ClearBuffers *cb);
    inline QVector<ClearBufferInfo> specificClearColorBufferInfo() const { return m_specificClearColorBuffers; }
    inline QVector<ClearBufferInfo> &specificClearColorBufferInfo() { return m_specificClearColorBuffers; }
    inline ClearBufferInfo globalClearColorBufferInfo() const { return m_globalClearColorBuffer; }

    inline QClearBuffers::BufferTypeFlags clearTypes() const { return m_clearBuffer; }
    inline float clearDepthValue() const { return m_clearDepthValue; }
    inline int clearStencilValue() const { return m_clearStencilValue; }

    RenderPassList passesAndParameters(ParameterInfoList *parameter, Entity *node, bool useDefaultMaterials = true);

    EntityRenderCommandData buildDrawRenderCommands(const QVector<Entity *> &entities,
                                                    int offset, int count) const;
    EntityRenderCommandData buildComputeRenderCommands(const QVector<Entity *> &entities,
                                                       int offset, int count) const;

    void updateRenderCommand(const EntityRenderCommandDataSubView &subView);

    void setAttachmentPack(const AttachmentPack &pack) { m_attachmentPack = pack; }
    const AttachmentPack &attachmentPack() const { return m_attachmentPack; }

    void setRenderTargetId(Qt3DCore::QNodeId renderTargetId) Q_DECL_NOTHROW { m_renderTarget = renderTargetId; }
    Qt3DCore::QNodeId renderTargetId() const Q_DECL_NOTHROW { return m_renderTarget; }

    void addSortType(const QVector<Qt3DRender::QSortPolicy::SortType> &sortTypes) { m_sortingTypes.append(sortTypes); }

    void setSurface(QSurface *surface) { m_surface = surface; }
    QSurface *surface() const { return m_surface; }

    void setLightSources(const QVector<LightSource> &lightSources) Q_DECL_NOTHROW { m_lightSources = lightSources; }
    void setEnvironmentLight(EnvironmentLight *environmentLight) Q_DECL_NOTHROW { m_environmentLight = environmentLight; }

    void updateMatrices();

    inline void setRenderCaptureNodeId(const Qt3DCore::QNodeId nodeId) Q_DECL_NOTHROW { m_renderCaptureNodeId = nodeId; }
    inline const Qt3DCore::QNodeId renderCaptureNodeId() const Q_DECL_NOTHROW { return m_renderCaptureNodeId; }
    inline void setRenderCaptureRequest(const QRenderCaptureRequest& request) Q_DECL_NOTHROW { m_renderCaptureRequest = request; }
    inline const QRenderCaptureRequest renderCaptureRequest() const Q_DECL_NOTHROW { return m_renderCaptureRequest; }

    void setMemoryBarrier(QMemoryBarrier::Operations barrier) Q_DECL_NOTHROW { m_memoryBarrier = barrier; }
    QMemoryBarrier::Operations memoryBarrier() const Q_DECL_NOTHROW { return m_memoryBarrier; }

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

    inline int commandCount() const { return m_renderCommandDataView ? m_renderCommandDataView->size() : 0; }

private:
    void setShaderAndUniforms(RenderCommand *command,
                              const ParameterInfoList &parameters,
                              const Entity *entity) const;

    void updateLightUniforms(RenderCommand *command,
                             const Entity *entity) const;

    mutable QThreadStorage<UniformBlockValueBuilder*> m_localData;

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
    QVector<ClearBufferInfo> m_specificClearColorBuffers;   // different draw buffers with distinct colors

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
    QMemoryBarrier::Operations m_memoryBarrier = QMemoryBarrier::None;
    QVector<Qt3DCore::QNodeId> m_insertFenceIds;
    QVector<QWaitFenceData> m_waitFences;
    QVector<Qt3DRender::QSortPolicy::SortType> m_sortingTypes;
    Qt3DCore::QNodeIdVector m_proximityFilterIds;
    Qt3DCore::QNodeIdVector m_layerFilterIds;
    Matrix4x4 m_viewMatrix;
    Matrix4x4 m_viewProjectionMatrix;
    Vector3D m_eyePos;
    Vector3D m_eyeViewDir;

    MaterialParameterGathererData m_parameters;
    mutable QVector<LightSource> m_lightSources;
    EnvironmentLight *m_environmentLight = nullptr;

    enum StandardUniform
    {
        ModelMatrix,
        ViewMatrix,
        ProjectionMatrix,
        ModelViewMatrix,
        ViewProjectionMatrix,
        ModelViewProjectionMatrix,
        InverseModelMatrix,
        InverseViewMatrix,
        InverseProjectionMatrix,
        InverseModelViewMatrix,
        InverseViewProjectionMatrix,
        InverseModelViewProjectionMatrix,
        ModelNormalMatrix,
        ModelViewNormalMatrix,
        ViewportMatrix,
        InverseViewportMatrix,
        AspectRatio,
        Time,
        Exposure,
        Gamma,
        EyePosition,
        SkinningPalette
    };

    typedef QHash<int, StandardUniform> StandardUniformsNameToTypeHash;
    static StandardUniformsNameToTypeHash ms_standardUniformSetters;
    static StandardUniformsNameToTypeHash initializeStandardUniformSetters();

    UniformValue standardUniformValue(StandardUniform standardUniformType,
                                      const Entity *entity) const;

    void setUniformValue(ShaderParameterPack &uniformPack, int nameId, const UniformValue &value) const;
    void setStandardUniformValue(ShaderParameterPack &uniformPack,
                                 int nameId,
                                 const Entity *entity) const;
    void setUniformBlockValue(ShaderParameterPack &uniformPack,
                              const ShaderUniformBlock &block,
                              const UniformValue &value) const;
    void setShaderStorageValue(ShaderParameterPack &uniformPack,
                               const ShaderStorageBlock &block,
                               const UniformValue &value) const;
    void setDefaultUniformBlockShaderDataValue(ShaderParameterPack &uniformPack,
                                               const GLShader *shader,
                                               const ShaderData *shaderData,
                                               const QString &structName) const;
    void applyParameter(const Parameter *param,
                        RenderCommand *command,
                        const GLShader *shader) const noexcept;
};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_ENDERVIEW_H
