// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 Svenn-Arne Dragly.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_GRAPHICSHELPERES3_H
#define QT3DRENDER_RENDER_OPENGL_GRAPHICSHELPERES3_H

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

#include <graphicshelperes2_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

class GraphicsHelperES3 : public GraphicsHelperES2
{
public:
    GraphicsHelperES3();
    ~GraphicsHelperES3();

    // QGraphicHelperInterface interface
    void bindBufferBase(GLenum target, GLuint index, GLuint buffer) override;
    bool frameBufferNeedsRenderBuffer(const Attachment &attachment) override;
    void bindFrameBufferAttachment(QOpenGLTexture *texture, const Attachment &attachment) override;
    void bindFrameBufferObject(GLuint frameBufferId, FBOBindMode mode) override;
    void bindUniformBlock(GLuint programId, GLuint uniformBlockIndex, GLuint uniformBlockBinding) override;
    void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) override;
    void buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer) override;
    void drawBuffers(GLsizei n, const GLenum *bufs) override;
    void drawArraysInstanced(GLenum primitiveType, GLint first, GLsizei count, GLsizei instances) override;
    void drawArraysInstancedBaseInstance(GLenum primitiveType, GLint first, GLsizei count, GLsizei instances, GLsizei baseInstance) override;
    void drawElementsInstancedBaseVertexBaseInstance(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void *indices, GLsizei instances, GLint baseVertex = 0,  GLint baseInstance = 0) override;
    void readBuffer(GLenum mode) override;
    void drawBuffer(GLenum mode) override;
    void initializeHelper(QOpenGLContext *context, QAbstractOpenGLFunctions *functions) override;
    char *mapBuffer(GLenum target, GLsizeiptr size) override;
    std::vector<ShaderUniform> programUniformsAndLocations(GLuint programId) override;
    std::vector<ShaderUniformBlock> programUniformBlocks(GLuint programId) override;
    bool supportsFeature(Feature feature) const override;
    GLboolean unmapBuffer(GLenum target) override;
    void vertexAttribDivisor(GLuint index, GLuint divisor) override;
    void vertexAttributePointer(GLenum shaderDataType, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) override;

    UniformType uniformTypeFromGLType(GLenum glType) override;
    uint uniformByteSize(const ShaderUniform &description) override;

    void *fenceSync() override;
    void clientWaitSync(void *sync, GLuint64 nanoSecTimeout) override;
    void waitSync(void *sync) override;
    bool wasSyncSignaled(void *sync) override;
    void deleteSync(void *sync) override;

protected:
    QOpenGLExtraFunctions *m_extraFuncs = nullptr;
};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_GRAPHICSHELPERES3_H
