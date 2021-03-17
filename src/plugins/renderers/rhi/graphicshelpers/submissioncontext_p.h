/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qblitframebuffer.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DRender/private/statemask_p.h>
#include <Qt3DRender/private/qgraphicsapifilter_p.h>
#include <shaderparameterpack_p.h>
#include <shadervariables_p.h>
#include <rhihandle_types_p.h>
#include <QSurface>
#include <QtGui/private/qrhi_p.h>
#include <QOffscreenSurface>

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

    bool beginDrawing(QSurface *surface);
    void endDrawing(bool swapBuffers);
    void releaseResources();
    void setOpenGLContext(QOpenGLContext *ctx);
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

    GLuint defaultFBO() const { return m_defaultFBO; }

    // Shaders
    bool activateShader(RHIShader *shader);
    RHIShader *activeShader() const { return m_activeShader; }

    // FBO
    GLuint activeFBO() const { return m_activeFBO; }
    void activateRenderTarget(const Qt3DCore::QNodeId id, const AttachmentPack &attachments,
                              GLuint defaultFboId);
    QSize renderTargetSize(const QSize &surfaceSize) const;
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

    // Parameters
    bool setParameters(ShaderParameterPack &parameterPack);

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
    QRhiRenderPassDescriptor *currentRenderPassDescriptor() const;
    QRhiSwapChain *currentSwapChain() const;
    QSurfaceFormat format() const noexcept;

private:
    // Material
    Material *activeMaterial() const { return m_material; }
    void setActiveMaterial(Material *rmat);

    // FBO
    void bindFrameBufferAttachmentHelper(GLuint fboId, const AttachmentPack &attachments);
    void activateDrawBuffers(const AttachmentPack &attachments);
    void resolveRenderTargetFormat();
    GLuint createRenderTarget(Qt3DCore::QNodeId renderTargetNodeId,
                              const AttachmentPack &attachments);
    GLuint updateRenderTarget(Qt3DCore::QNodeId renderTargetNodeId,
                              const AttachmentPack &attachments, bool isActiveRenderTarget);

    // Buffers
    HRHIBuffer createRHIBufferFor(Buffer *buffer);
    void uploadDataToRHIBuffer(Buffer *buffer, RHIBuffer *b, bool releaseBuffer = false);
    QByteArray downloadDataFromRHIBuffer(Buffer *buffer, RHIBuffer *b);
    bool bindRHIBuffer(RHIBuffer *buffer, RHIBuffer::Type type);

    // States
    void applyState(const StateVariant &state, QRhiGraphicsPipeline *graphicsPipeline);

    bool m_ownCurrent;
    const unsigned int m_id;
    QSurface *m_surface;
    QSize m_surfaceSize;

    RHIShader *m_activeShader;

    QHash<Qt3DCore::QNodeId, HRHIBuffer> m_renderBufferHash;
    QHash<Qt3DCore::QNodeId, GLuint> m_renderTargets;
    QHash<GLuint, QSize> m_renderTargetsSize;
    QAbstractTexture::TextureFormat m_renderTargetFormat;

    Material *m_material;
    GLuint m_activeFBO;

    Renderer *m_renderer;
    QByteArray m_uboTempArray;

    bool m_initialized;

    GLint m_maxTextureUnits;
    GLuint m_defaultFBO;
    GraphicsApiFilterData m_contextInfo;

    QRhi *m_rhi;
    QHash<QSurface *, SwapChainInfo> m_swapChains;
    QRhiSwapChain *m_currentSwapChain;
    QRhiRenderPassDescriptor *m_currentRenderPassDescriptor;

#ifndef QT_NO_OPENGL
    QOffscreenSurface *m_fallbackSurface;
#endif
};

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_SUBMISSIONCONTEXT_H
