// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_SUBMISSIONCONTEXT_H
#define QT3DRENDER_RENDER_RHI_SUBMISSIONCONTEXT_H

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

#include <rhibuffer_p.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qblitframebuffer.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DRender/private/statemask_p.h>
#include <Qt3DRender/private/qgraphicsapifilter_p.h>
#include <shaderparameterpack_p.h>
#include <shadervariables_p.h>
#include <rhihandle_types_p.h>
#include <QSurface>
#include <QOffscreenSurface>
#include <rhi/qrhi.h>

QT_BEGIN_NAMESPACE

class QAbstractOpenGLFunctions;

namespace Qt3DRender {

namespace Render {

class Material;
class AttachmentPack;
class Attribute;
class Buffer;
class ShaderManager;
struct StateVariant;
class RenderTarget;
class RenderStateSet;

namespace Rhi {

class Renderer;
class GraphicsHelperInterface;
class RHITexture;
class RHIShader;
class RHIShaderManager;
class RenderCommand;

typedef QPair<QString, int> NamedUniformLocation;

class Q_AUTOTEST_EXPORT SubmissionContext
{
public:
    enum Feature {
        MRT = 0,
        Tessellation,
        UniformBufferObject,
        BindableFragmentOutputs,
        PrimitiveRestart,
        RenderBufferDimensionRetrieval,
        TextureDimensionRetrieval,
        ShaderStorageObject,
        Compute,
        DrawBuffersBlend,
        BlitFramebuffer,
        IndirectDrawing,
        MapBuffer,
        Fences,
        ShaderImage
    };

    enum FBOBindMode { FBODraw, FBORead, FBOReadAndDraw };

    SubmissionContext();
    ~SubmissionContext();

    void initialize();
    int id() const; // unique, small integer ID of this context
    void setRenderer(Renderer *renderer) { m_renderer = renderer; }

    void setDrivenExternally(bool drivenExternally);
    bool drivenExternally() const;

    bool beginDrawing(QSurface *surface);
    void endDrawing(bool swapBuffers);
    void releaseResources();
    void setOpenGLContext(QOpenGLContext *ctx);
    void setRHIContext(QRhi *ctx);
    void setDefaultRenderTarget(QRhiRenderTarget *target);
    void setCommandBuffer(QRhiCommandBuffer *commandBuffer);
    bool isInitialized() const { return m_initialized; }
    const GraphicsApiFilterData *contextInfo() const;

    // Shaders
    struct ShaderCreationInfo
    {
        bool linkSucceeded = false;
        QString logs;
    };

    ShaderCreationInfo createShaderProgram(RHIShader *shaderNode);
    void loadShader(Shader *shader, ShaderManager *shaderManager,
                    RHIShaderManager *rhiShaderManager);


    // FBO
    QImage readFramebuffer(const QRect &rect);
    void blitFramebuffer(Qt3DCore::QNodeId outputRenderTargetId,
                         Qt3DCore::QNodeId inputRenderTargetId, QRect inputRect, QRect outputRect,
                         uint defaultFboId,
                         QRenderTargetOutput::AttachmentPoint inputAttachmentPoint,
                         QRenderTargetOutput::AttachmentPoint outputAttachmentPoint,
                         QBlitFramebuffer::InterpolationMethod interpolationMethod);

    // Attributes
    void specifyAttribute(const Attribute *attribute, Buffer *buffer,
                          const ShaderAttribute *attributeDescription);
    void specifyIndices(Buffer *buffer);

    // Buffer
    void updateBuffer(Buffer *buffer);
    QByteArray downloadBufferContent(Buffer *buffer);
    void releaseBuffer(Qt3DCore::QNodeId bufferId);
    bool hasRHIBufferForBuffer(Buffer *buffer);
    RHIBuffer *rhiBufferForRenderBuffer(Buffer *buf);

    // RenderState
    void applyStateSet(const RenderStateSet *ss, QRhiGraphicsPipeline *graphicsPipeline);
    StateVariant *getState(RenderStateSet *ss, StateMask type) const;

    // Swap chain

    struct SwapChainInfo
    {
        QRhiSwapChain *swapChain = nullptr;
        QRhiRenderBuffer *renderBuffer = nullptr;
        QRhiRenderPassDescriptor *renderPassDescriptor = nullptr;
    };
    SwapChainInfo *swapChainForSurface(QSurface *surface) noexcept;

    QRhiResourceUpdateBatch *m_currentUpdates {};

    QRhi *rhi() const { return m_rhi; }
    QRhiCommandBuffer *currentFrameCommandBuffer() const;
    QRhiRenderTarget *currentFrameRenderTarget() const;
    QRhiRenderTarget *defaultRenderTarget() const;
    QRhiRenderPassDescriptor *currentRenderPassDescriptor() const;
    QRhiSwapChain *currentSwapChain() const;
    QSurfaceFormat format() const noexcept;

private:
    // FBO
    void bindFrameBufferAttachmentHelper(GLuint fboId, const AttachmentPack &attachments);
    void activateDrawBuffers(const AttachmentPack &attachments);
    // Buffers
    HRHIBuffer createRHIBufferFor(Buffer *buffer);
    void uploadDataToRHIBuffer(Buffer *buffer, RHIBuffer *b);
    QByteArray downloadDataFromRHIBuffer(Buffer *buffer, RHIBuffer *b);
    bool bindRHIBuffer(RHIBuffer *buffer, RHIBuffer::Type type);

    // States
    void applyState(const StateVariant &state, QRhiGraphicsPipeline *graphicsPipeline);

    bool m_initialized;
    bool m_ownsRhiCtx;
    bool m_drivenExternally;

    QHash<Qt3DCore::QNodeId, HRHIBuffer> m_renderBufferHash;

    Material *m_material;

    Renderer *m_renderer;

    GraphicsApiFilterData m_contextInfo;

    QRhi *m_rhi;
    QHash<QSurface *, SwapChainInfo> m_swapChains;
    QRhiSwapChain *m_currentSwapChain;
    QRhiRenderPassDescriptor *m_currentRenderPassDescriptor;
    QRhiRenderTarget *m_defaultRenderTarget;
    QRhiCommandBuffer *m_defaultCommandBuffer;

#ifndef QT_NO_OPENGL
    QOffscreenSurface *m_fallbackSurface;
#endif
};

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_SUBMISSIONCONTEXT_H
