/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_GRAPHICSCONTEXT_H
#define QT3DRENDER_RENDER_GRAPHICSCONTEXT_H

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

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QHash>
#include <QColor>
#include <QMatrix4x4>
#include <QBitArray>
#include <Qt3DRender/private/quniformvalue_p.h>
#include <Qt3DRender/qclearbuffer.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLDebugLogger;
class QOpenGLShaderProgram;
class QAbstractOpenGLFunctions;

namespace Qt3DRender {

class QGraphicsApiFilter;

namespace Render {

class Renderer;
class GraphicsHelperInterface;
class RenderStateSet;
class Material;
class Texture;
class RenderCommand;
class RenderTarget;
class AttachmentPack;
class Attribute;
class Buffer;
class GLBuffer;

enum TextureScope
{
    TextureScopeMaterial = 0,
    TextureScopeTechnique
    // per-pass for deferred rendering?
};

typedef QPair<QString, int> NamedUniformLocation;

class GraphicsContext
{
public:
    GraphicsContext();
    ~GraphicsContext();

    int id() const; // unique, small integer ID of this context

    bool beginDrawing(QSurface *surface, const QColor &color);
    void clearBackBuffer(QClearBuffer::BufferType buffers);
    void endDrawing(bool swapBuffers);

    void setViewport(const QRectF &viewport, const QSize &surfaceSize);
    QRectF viewport() const { return m_viewport; }

    /**
     * @brief releaseGL - release all OpenGL objects associated with
     * this context
     */
    void releaseOpenGL();
    void setOpenGLContext(QOpenGLContext* ctx);
    QOpenGLContext *openGLContext() { return m_gl; }
    bool makeCurrent(QSurface *surface);
    void doneCurrent();
    void activateGLHelper();

    void activateShader(Shader* shader);
    QOpenGLShaderProgram *containsProgram(const ProgramDNA &dna);

    GLuint activeFBO() const { return m_activeFBO; }
    GLuint defaultFBO() const { return m_defaultFBO; }
    void activateRenderTarget(RenderTarget *renderTarget, const AttachmentPack &attachments, GLuint defaultFboId);

    Material* activeMaterial() const { return m_material; }

    void setActiveMaterial(Material* rmat);

    void executeCommand(const RenderCommand *command);

    /**
     * @brief activeShader
     * @return
     */
    QOpenGLShaderProgram* activeShader();

    void setRenderer(Renderer *renderer);

    void specifyAttribute(const Attribute *attribute, Buffer *buffer, const QString &shaderName);
    void specifyIndices(Buffer *buffer);
    void updateBuffer(Buffer *buffer);

    void setParameters(ShaderParameterPack &parameterPack);

    /**
     * @brief glBufferFor - given a client-side (CPU) buffer, provide the
     * context-specific object. Initial call will create the buffer.
     * @param buf
     * @return
     */
    GLBuffer *glBufferForRenderBuffer(Buffer *buf);

    /**
     * @brief activateTexture - make a texture active on a hardware unit
     * @param tex - the texture to activate
     * @param onUnit - option, specify the explicit unit to activate on
     * @return - the unit the texture was activated on
     */
    int activateTexture(TextureScope scope, Texture* tex, int onUnit = -1);

    void deactivateTexture(Texture *tex);

    void setCurrentStateSet(RenderStateSet* ss);
    RenderStateSet *currentStateSet() const;
    QGraphicsApiFilter *contextInfo() const;

    // Wrapper methods
    void    alphaTest(GLenum mode1, GLenum mode2);
    void    bindBufferBase(GLenum target, GLuint bindingIndex, GLuint buffer);
    void    bindFragOutputs(GLuint shader, const QHash<QString, int> &outputs);
    void    bindUniformBlock(GLuint programId, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
    void    bindUniform(const QVariant &v, const ShaderUniform &description);
    void    bindShaderStorageBlock(GLuint programId, GLuint shaderStorageBlockIndex, GLuint shaderStorageBlockBinding);
    void    blendEquation(GLenum mode);
    void    blendFunci(GLuint buf, GLenum sfactor, GLenum dfactor);
    void    blendFuncSeparatei(GLuint buf, GLenum sRGB, GLenum dRGB, GLenum sAlpha, GLenum dAlpha);
    GLuint  boundFrameBufferObject();
    void    buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer);
    void    clearColor(const QColor &color);
    void    cullFace(GLenum mode);
    void    depthMask(GLenum mode);
    void    depthTest(GLenum mode);
    void    disableAlphaCoverage();
    void    disableClipPlane(int clipPlane);
    void    disablei(GLenum cap, GLuint index);
    void    disablePrimitiveRestart();
    void    dispatchCompute(int x, int y, int z);
    void    drawArrays(GLenum primitiveType, GLint first, GLsizei count);
    void    drawArraysInstanced(GLenum primitiveType, GLint first, GLsizei count, GLsizei instances);
    void    drawElements(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void * indices, GLint baseVertex = 0);
    void    drawElementsInstanced(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void * indices, GLsizei instances, GLint baseVertex = 0, GLint baseInstance = 0);
    void    enableAlphaCoverage();
    void    enableClipPlane(int clipPlane);
    void    enablei(GLenum cap, GLuint index);
    void    enablePrimitiveRestart(int restartIndex);
    void    frontFace(GLenum mode);
    GLint   maxClipPlaneCount();
    void    pointSize(bool programmable, GLfloat value);
    void    setVerticesPerPatch(GLint verticesPerPatch);

    // Helper methods
    static GLint elementType(GLint type);
    static GLint tupleSizeFromType(GLint type);
    static GLuint byteSizeFromType(GLint type);
    static GLint glDataTypeFromAttributeDataType(QAttribute::DataType dataType);

    bool supportsDrawBuffersBlend() const;
    bool supportsVAO() const { return m_supportsVAO; }

private:
    void initialize();

    void decayTextureScores();

    GLint assignUnitForTexture(Texture* tex);
    void deactivateTexturesWithScope(TextureScope ts);

    GraphicsHelperInterface *resolveHighestOpenGLFunctions();

    void bindFrameBufferAttachmentHelper(GLuint fboId, const AttachmentPack &attachments);
    void activateDrawBuffers(const AttachmentPack &attachments);
    HGLBuffer createGLBufferFor(Buffer *buffer);
    void uploadDataToGLBuffer(Buffer *buffer, GLBuffer *b, bool releaseBuffer = false);

    bool m_initialized;
    const unsigned int m_id;
    QOpenGLContext *m_gl;
    QSurface *m_surface;
    GraphicsHelperInterface *m_glHelper;
    bool m_ownCurrent;

    Shader *m_activeShader;
    QHash<ProgramDNA, Shader *> m_renderShaderHash;
    QHash<Qt3DCore::QNodeId, HGLBuffer> m_renderBufferHash;
    QHash<Qt3DCore::QNodeId, GLuint> m_renderTargets;
    QHash<GLuint, QSize> m_renderTargetsSize;

    QHash<QSurface *, GraphicsHelperInterface*> m_glHelpers;

    // active textures, indexed by texture unit
    QVector<uint> m_activeTextures;
    QBitArray m_pinnedTextureUnits;
    QVector<TextureScope> m_textureScopes;

    // recency score for all render-textures we've seen. Higher scores
    // mean more recently used.
    QHash<uint, int> m_textureScores;

    Material* m_material;
    QRectF m_viewport;
    GLuint m_activeFBO;
    GLuint m_defaultFBO;

    RenderStateSet* m_stateSet;

    Renderer *m_renderer;
    QGraphicsApiFilter *m_contextInfo;

    QByteArray m_uboTempArray;

    bool m_supportsVAO;
    QScopedPointer<QOpenGLDebugLogger> m_debugLogger;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GRAPHICSCONTEXT_H
