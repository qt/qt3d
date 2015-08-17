/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qgraphicshelpergl3_p.h"

#ifndef QT_OPENGL_ES_2
#include <QOpenGLFunctions_3_2_Core>
#include <QtOpenGLExtensions/qopenglextensions.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <private/attachmentpack_p.h>
#include <private/qgraphicsutils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

QGraphicsHelperGL3::QGraphicsHelperGL3()
    : m_funcs(Q_NULLPTR)
    , m_tessFuncs()
{
}

void QGraphicsHelperGL3::initializeHelper(QOpenGLContext *context,
                                          QAbstractOpenGLFunctions *functions)
{
    Q_UNUSED(context);
    m_funcs = static_cast<QOpenGLFunctions_3_2_Core*>(functions);
    const bool ok = m_funcs->initializeOpenGLFunctions();
    Q_ASSERT(ok);
    Q_UNUSED(ok);

    if (context->hasExtension(QByteArrayLiteral("GL_ARB_tessellation_shader"))) {
        m_tessFuncs.reset(new QOpenGLExtension_ARB_tessellation_shader);
        m_tessFuncs->initializeOpenGLFunctions();
    }
}

void QGraphicsHelperGL3::drawElementsInstanced(GLenum primitiveType,
                                               GLsizei primitiveCount,
                                               GLint indexType,
                                               void *indices,
                                               GLsizei instances,
                                               GLint baseVertex,
                                               GLint baseInstance)
{
    if (baseInstance != 0)
        qWarning() << "glDrawElementsInstancedBaseVertexBaseInstance is not supported with OpenGL ES 2";

    // glDrawElements OpenGL 3.1 or greater
    m_funcs->glDrawElementsInstancedBaseVertex(primitiveType,
                                               primitiveCount,
                                               indexType,
                                               indices,
                                               instances,
                                               baseVertex);
}

void QGraphicsHelperGL3::drawArraysInstanced(GLenum primitiveType,
                                             GLint first,
                                             GLsizei count,
                                             GLsizei instances)
{
    // glDrawArraysInstanced OpenGL 3.1 or greater
    m_funcs->glDrawArraysInstanced(primitiveType,
                                   first,
                                   count,
                                   instances);
}

void QGraphicsHelperGL3::drawElements(GLenum primitiveType,
                                      GLsizei primitiveCount,
                                      GLint indexType,
                                      void *indices,
                                      GLint baseVertex)
{
    m_funcs->glDrawElementsBaseVertex(primitiveType,
                                      primitiveCount,
                                      indexType,
                                      indices,
                                      baseVertex);
}

void QGraphicsHelperGL3::drawArrays(GLenum primitiveType,
                                    GLint first,
                                    GLsizei count)
{
    m_funcs->glDrawArrays(primitiveType,
                          first,
                          count);
}

void QGraphicsHelperGL3::setVerticesPerPatch(GLint verticesPerPatch)
{
#if defined(QT_OPENGL_4)
    if (!m_tessFuncs) {
        qWarning() << "Tessellation not supported with OpenGL 3 without GL_ARB_tessellation_shader";
        return;
    }

    m_tessFuncs->glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);
#else
    Q_UNUSED(verticesPerPatch);
    qWarning() << "Tessellation not supported";
#endif
}

void QGraphicsHelperGL3::useProgram(GLuint programId)
{
    m_funcs->glUseProgram(programId);
}

QVector<ShaderUniform> QGraphicsHelperGL3::programUniformsAndLocations(GLuint programId)
{
    QVector<ShaderUniform> uniforms;

    GLint nbrActiveUniforms = 0;
    m_funcs->glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &nbrActiveUniforms);
    uniforms.reserve(nbrActiveUniforms);
    char uniformName[256];
    for (GLint i = 0; i < nbrActiveUniforms; i++) {
        ShaderUniform uniform;
        GLsizei uniformNameLength = 0;
        // Size is 1 for scalar and more for struct or arrays
        // Type is the GL Type
        m_funcs->glGetActiveUniform(programId, i, sizeof(uniformName) - 1, &uniformNameLength,
                                    &uniform.m_size, &uniform.m_type, uniformName);
        uniformName[sizeof(uniformName) - 1] = '\0';
        uniform.m_location = m_funcs->glGetUniformLocation(programId, uniformName);
        uniform.m_name = QString::fromUtf8(uniformName, uniformNameLength);
        m_funcs->glGetActiveUniformsiv(programId, 1, (GLuint*)&i, GL_UNIFORM_BLOCK_INDEX, &uniform.m_blockIndex);
        m_funcs->glGetActiveUniformsiv(programId, 1, (GLuint*)&i, GL_UNIFORM_OFFSET, &uniform.m_offset);
        m_funcs->glGetActiveUniformsiv(programId, 1, (GLuint*)&i, GL_UNIFORM_ARRAY_STRIDE, &uniform.m_arrayStride);
        m_funcs->glGetActiveUniformsiv(programId, 1, (GLuint*)&i, GL_UNIFORM_MATRIX_STRIDE, &uniform.m_matrixStride);
        uniform.m_rawByteSize = uniformByteSize(uniform);
        uniforms.append(uniform);
        qCDebug(Render::Rendering) << uniform.m_name << "size" << uniform.m_size
                                   << " offset" << uniform.m_offset
                                   << " rawSize" << uniform.m_rawByteSize;
    }

    return uniforms;
}

QVector<ShaderAttribute> QGraphicsHelperGL3::programAttributesAndLocations(GLuint programId)
{
    QVector<ShaderAttribute> attributes;
    GLint nbrActiveAttributes = 0;
    m_funcs->glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &nbrActiveAttributes);
    attributes.reserve(nbrActiveAttributes);
    char attributeName[256];
    for (GLint i = 0; i < nbrActiveAttributes; i++) {
        ShaderAttribute attribute;
        GLsizei attributeNameLength = 0;
        // Size is 1 for scalar and more for struct or arrays
        // Type is the GL Type
        m_funcs->glGetActiveAttrib(programId, i, sizeof(attributeName) - 1, &attributeNameLength,
                                   &attribute.m_size, &attribute.m_type, attributeName);
        attributeName[sizeof(attributeName) - 1] = '\0';
        attribute.m_location = m_funcs->glGetAttribLocation(programId, attributeName);
        attribute.m_name = QString::fromUtf8(attributeName, attributeNameLength);
        attributes.append(attribute);
    }
    return attributes;
}

QVector<ShaderUniformBlock> QGraphicsHelperGL3::programUniformBlocks(GLuint programId)
{
    QVector<ShaderUniformBlock> blocks;
    GLint nbrActiveUniformsBlocks = 0;
    m_funcs->glGetProgramiv(programId, GL_ACTIVE_UNIFORM_BLOCKS, &nbrActiveUniformsBlocks);
    blocks.reserve(nbrActiveUniformsBlocks);
    for (GLint i = 0; i < nbrActiveUniformsBlocks; i++) {
        QByteArray uniformBlockName(256, '\0');
        ShaderUniformBlock uniformBlock;
        m_funcs->glGetActiveUniformBlockName(programId, i, 256, NULL, uniformBlockName.data());
        uniformBlock.m_name = QString::fromUtf8(uniformBlockName);
        uniformBlock.m_index = i;
        m_funcs->glGetActiveUniformBlockiv(programId, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformBlock.m_activeUniformsCount);
        m_funcs->glGetActiveUniformBlockiv(programId, i, GL_UNIFORM_BLOCK_BINDING, &uniformBlock.m_binding);
        m_funcs->glGetActiveUniformBlockiv(programId, i, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlock.m_size);
        blocks.append(uniformBlock);
    }
    return blocks;
}

void QGraphicsHelperGL3::vertexAttribDivisor(GLuint index, GLuint divisor)
{
    Q_UNUSED(index);
    Q_UNUSED(divisor);
}

void QGraphicsHelperGL3::blendEquation(GLenum mode)
{
    m_funcs->glBlendEquation(mode);
}

void QGraphicsHelperGL3::alphaTest(GLenum, GLenum)
{
    qCWarning(Render::Rendering) << "AlphaTest not available with OpenGL 3.2 core";
}

void QGraphicsHelperGL3::depthTest(GLenum mode)
{
    m_funcs->glEnable(GL_DEPTH_TEST);
    m_funcs->glDepthFunc(mode);
}

void QGraphicsHelperGL3::depthMask(GLenum mode)
{
    m_funcs->glDepthMask(mode);
}

void QGraphicsHelperGL3::cullFace(GLenum mode)
{
    m_funcs->glEnable(GL_CULL_FACE);
    m_funcs->glCullFace(mode);
}

void QGraphicsHelperGL3::frontFace(GLenum mode)
{
    m_funcs->glFrontFace(mode);

}

void QGraphicsHelperGL3::enableAlphaCoverage()
{
    m_funcs->glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

void QGraphicsHelperGL3::disableAlphaCoverage()
{
    m_funcs->glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

GLuint QGraphicsHelperGL3::createFrameBufferObject()
{
    GLuint id;
    m_funcs->glGenFramebuffers(1, &id);
    return id;
}

void QGraphicsHelperGL3::releaseFrameBufferObject(GLuint frameBufferId)
{
    m_funcs->glDeleteFramebuffers(1, &frameBufferId);
}

void QGraphicsHelperGL3::bindFrameBufferObject(GLuint frameBufferId)
{
    m_funcs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
}

GLuint QGraphicsHelperGL3::boundFrameBufferObject()
{
    GLint id = 0;
    m_funcs->glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &id);
    return id;
}

bool QGraphicsHelperGL3::checkFrameBufferComplete()
{
    return (m_funcs->glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void QGraphicsHelperGL3::bindFrameBufferAttachment(QOpenGLTexture *texture, const Attachment &attachment)
{
    GLenum attr = GL_DEPTH_STENCIL_ATTACHMENT;

    if (attachment.m_type <= QRenderAttachment::ColorAttachment15)
        attr = GL_COLOR_ATTACHMENT0 + attachment.m_type;
    else if (attachment.m_type == QRenderAttachment::DepthAttachment)
        attr = GL_DEPTH_ATTACHMENT;
    else if (attachment.m_type == QRenderAttachment::StencilAttachment)
        attr = GL_STENCIL_ATTACHMENT;

    texture->bind();
    QOpenGLTexture::Target target = texture->target();
    if (target == QOpenGLTexture::Target1DArray || target == QOpenGLTexture::Target2DArray ||
            target == QOpenGLTexture::Target2DMultisampleArray || target == QOpenGLTexture::Target3D)
        m_funcs->glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, attr, texture->textureId(), attachment.m_mipLevel, attachment.m_layer);
    else if (target == QOpenGLTexture::TargetCubeMapArray)
        m_funcs->glFramebufferTexture3D(GL_DRAW_FRAMEBUFFER, attr, attachment.m_face, texture->textureId(), attachment.m_mipLevel, attachment.m_layer);
    else if (target == QOpenGLTexture::TargetCubeMap)
        m_funcs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attr, attachment.m_face, texture->textureId(), attachment.m_mipLevel);
    else
        m_funcs->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, attr, texture->textureId(), attachment.m_mipLevel);
    texture->release();
}

bool QGraphicsHelperGL3::supportsFeature(QGraphicsHelperInterface::Feature feature) const
{
    switch (feature) {
    case MRT:
    case UniformBufferObject:
    case PrimitiveRestart:
        return true;
    case Tessellation:
        return !m_tessFuncs.isNull();
    default:
        return false;
    }
}

void QGraphicsHelperGL3::drawBuffers(GLsizei n, const int *bufs)
{
    // Use QVarLengthArray here
    QVarLengthArray<GLenum, 16> drawBufs(n);

    for (int i = 0; i < n; i++)
        drawBufs[i] = GL_COLOR_ATTACHMENT0 + bufs[i];
    m_funcs->glDrawBuffers(n, drawBufs.constData());
}

void QGraphicsHelperGL3::bindFragDataLocation(GLuint shader, const QHash<QString, int> &outputs)
{
    Q_FOREACH (const QString &name, outputs.keys())
        m_funcs->glBindFragDataLocation(shader, outputs.value(name), name.toStdString().c_str());
}

void QGraphicsHelperGL3::bindUniform(const QVariant &v, const ShaderUniform &description)
{
    switch (description.m_type) {

    case GL_FLOAT:
        m_funcs->glUniform1fv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 1));
        break;

    case GL_FLOAT_VEC2:
        m_funcs->glUniform2fv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 2));
        break;

    case GL_FLOAT_VEC3:
        m_funcs->glUniform3fv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 3));
        break;

    case GL_FLOAT_VEC4:
        m_funcs->glUniform4fv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 4));
        break;

    case GL_FLOAT_MAT2:
        m_funcs->glUniformMatrix2fv(description.m_location, description.m_size, GL_FALSE,
                                    QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 4));
        break;

    case GL_FLOAT_MAT2x3:
        m_funcs->glUniformMatrix2x3fv(description.m_location, description.m_size, GL_FALSE,
                                      QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 6));
        break;

    case GL_FLOAT_MAT2x4:
        m_funcs->glUniformMatrix2x4fv(description.m_location, description.m_size, GL_FALSE,
                                      QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 8));
        break;

    case GL_FLOAT_MAT3:
        m_funcs->glUniformMatrix3fv(description.m_location, description.m_size, GL_FALSE,
                                    QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 9));
        break;

    case GL_FLOAT_MAT3x2:
        m_funcs->glUniformMatrix3x2fv(description.m_location, description.m_size,  GL_FALSE,
                                      QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 6));
        break;

    case GL_FLOAT_MAT3x4:
        m_funcs->glUniformMatrix3x4fv(description.m_location, description.m_size, GL_FALSE,
                                      QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 12));
        break;

    case GL_FLOAT_MAT4:
        m_funcs->glUniformMatrix4fv(description.m_location, description.m_size, GL_FALSE,
                                    QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 16));
        break;

    case GL_FLOAT_MAT4x2:
        m_funcs->glUniformMatrix4x2fv(description.m_location, description.m_size, GL_FALSE,
                                      QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 8));
        break;

    case GL_FLOAT_MAT4x3:
        m_funcs->glUniformMatrix4x3fv(description.m_location, description.m_size, GL_FALSE,
                                      QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 12));
        break;

    case GL_INT:
        m_funcs->glUniform1iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 1));
        break;

    case GL_INT_VEC2:
        m_funcs->glUniform2iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 2));
        break;

    case GL_INT_VEC3:
        m_funcs->glUniform3iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 3));
        break;

    case GL_INT_VEC4:
        m_funcs->glUniform4iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 4));
        break;

    case GL_UNSIGNED_INT:
        m_funcs->glUniform1uiv(description.m_location, description.m_size,
                               QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 1));
        break;

    case GL_UNSIGNED_INT_VEC2:
        m_funcs->glUniform2uiv(description.m_location, description.m_size,
                               QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 2));
        break;

    case GL_UNSIGNED_INT_VEC3:
        m_funcs->glUniform3uiv(description.m_location, description.m_size,
                               QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 3));
        break;

    case GL_UNSIGNED_INT_VEC4:
        m_funcs->glUniform4uiv(description.m_location, description.m_size,
                               QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 4));
        break;

    case GL_BOOL:
        m_funcs->glUniform1iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 1));
        break;

    case GL_BOOL_VEC2:
        m_funcs->glUniform2iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 2));
        break;

    case GL_BOOL_VEC3:
        m_funcs->glUniform3iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 3));
        break;

    case GL_BOOL_VEC4:
        m_funcs->glUniform4iv(description.m_location, description.m_size,
                              QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 4));
        break;

    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: {
        Q_ASSERT(description.m_size == 1);
        m_funcs->glUniform1i(description.m_location, v.toInt());
        break;
    }

    default:
        qWarning() << Q_FUNC_INFO << "unsupported uniform type:" << description.m_type << "for " << description.m_name;
        break;
    }
}

void QGraphicsHelperGL3::bindUniformBlock(GLuint programId, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    m_funcs->glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
}

void QGraphicsHelperGL3::bindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
    m_funcs->glBindBufferBase(target, index, buffer);
}

void QGraphicsHelperGL3::buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer)
{
    char *bufferData = buffer.data();

    switch (description.m_type) {

    case GL_FLOAT: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 1);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 1);
        break;
    }

    case GL_FLOAT_VEC2: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 2);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 2);
        break;
    }

    case GL_FLOAT_VEC3: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 3);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 3);
        break;
    }

    case GL_FLOAT_VEC4: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 4);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 4);
        break;
    }

    case GL_FLOAT_MAT2: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 4);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 2, 2);
        break;
    }

    case GL_FLOAT_MAT2x3: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 6);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 2, 3);
        break;
    }

    case GL_FLOAT_MAT2x4: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 8);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 2, 4);
        break;
    }

    case GL_FLOAT_MAT3: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 9);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 3, 3);
        break;
    }

    case GL_FLOAT_MAT3x2: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 6);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 3, 2);
        break;
    }

    case GL_FLOAT_MAT3x4: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 12);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 3, 4);
        break;
    }

    case GL_FLOAT_MAT4: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 16);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 4, 4);
        break;
    }

    case GL_FLOAT_MAT4x2: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 8);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 4, 2);
        break;
    }

    case GL_FLOAT_MAT4x3: {
        const GLfloat *data = QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 12);
        QGraphicsUtils::fillDataMatrixArray(bufferData, data, description, 4, 3);
        break;
    }

    case GL_INT: {
        const GLint *data = QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 1);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 1);
        break;
    }

    case GL_INT_VEC2: {
        const GLint *data = QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 2);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 2);
        break;
    }

    case GL_INT_VEC3: {
        const GLint *data = QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 3);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 3);
        break;
    }

    case GL_INT_VEC4: {
        const GLint *data = QGraphicsUtils::valueArrayFromVariant<GLint>(v, description.m_size, 4);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 4);
        break;
    }

    case GL_UNSIGNED_INT: {
        const GLuint *data = QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 1);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 1);
        break;
    }

    case GL_UNSIGNED_INT_VEC2: {
        const GLuint *data = QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 2);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 2);
        break;
    }

    case GL_UNSIGNED_INT_VEC3: {
        const GLuint *data = QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 3);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 3);
        break;
    }

    case GL_UNSIGNED_INT_VEC4: {
        const GLuint *data = QGraphicsUtils::valueArrayFromVariant<GLuint>(v, description.m_size, 4);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 4);
        break;
    }

    case GL_BOOL: {
        const GLboolean *data = QGraphicsUtils::valueArrayFromVariant<GLboolean>(v, description.m_size, 1);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 1);
        break;
    }

    case GL_BOOL_VEC2: {
        const GLboolean *data = QGraphicsUtils::valueArrayFromVariant<GLboolean>(v, description.m_size, 2);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 2);
        break;
    }

    case GL_BOOL_VEC3: {
        const GLboolean *data = QGraphicsUtils::valueArrayFromVariant<GLboolean>(v, description.m_size, 3);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 3);
        break;
    }

    case GL_BOOL_VEC4: {
        const GLboolean *data = QGraphicsUtils::valueArrayFromVariant<GLboolean>(v, description.m_size, 4);
        QGraphicsUtils::fillDataArray(bufferData, data, description, 4);
        break;
    }

    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: {
        Q_ASSERT(description.m_size == 1);
        int value = v.toInt();
        QGraphicsUtils::fillDataArray<GLint>(bufferData, &value, description, 1);
        break;
    }

    default:
        qWarning() << Q_FUNC_INFO << "unsupported uniform type:" << description.m_type << "for " << description.m_name;
        break;
    }
}

uint QGraphicsHelperGL3::uniformByteSize(const ShaderUniform &description)
{
    uint rawByteSize = 0;
    int arrayStride = qMax(description.m_arrayStride, 0);
    int matrixStride = qMax(description.m_matrixStride, 0);

    switch (description.m_type) {

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
        rawByteSize = 8;
        break;

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
        rawByteSize = 12;
        break;

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
        rawByteSize = 16;
        break;

    case GL_FLOAT_MAT2:
        rawByteSize = matrixStride ? 2 * matrixStride : 16;
        break;

    case GL_FLOAT_MAT2x4:
        rawByteSize = matrixStride ? 2 * matrixStride : 32;
        break;

    case GL_FLOAT_MAT4x2:
        rawByteSize = matrixStride ? 4 * matrixStride : 32;
        break;

    case GL_FLOAT_MAT3:
        rawByteSize = matrixStride ? 3 * matrixStride : 36;
        break;

    case GL_FLOAT_MAT2x3:
        rawByteSize = matrixStride ? 2 * matrixStride : 24;
        break;

    case GL_FLOAT_MAT3x2:
        rawByteSize = matrixStride ? 3 * matrixStride : 24;
        break;

    case GL_FLOAT_MAT4:
        rawByteSize = matrixStride ? 4 * matrixStride : 64;
        break;

    case GL_FLOAT_MAT4x3:
        rawByteSize = matrixStride ? 4 * matrixStride : 48;
        break;

    case GL_FLOAT_MAT3x4:
        rawByteSize = matrixStride ? 3 * matrixStride : 48;
        break;

    case GL_BOOL:
        rawByteSize = 1;
        break;

    case GL_BOOL_VEC2:
        rawByteSize = 2;
        break;

    case GL_BOOL_VEC3:
        rawByteSize = 3;
        break;

    case GL_BOOL_VEC4:
        rawByteSize = 4;
        break;

    case GL_INT:
    case GL_FLOAT:
    case GL_UNSIGNED_INT:
    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        rawByteSize = 4;
        break;
    }

    return arrayStride ? rawByteSize * arrayStride : rawByteSize;
}

void QGraphicsHelperGL3::enableClipPlane(int clipPlane)
{
    m_funcs->glEnable(GL_CLIP_DISTANCE0 + clipPlane);
}

void QGraphicsHelperGL3::disableClipPlane(int clipPlane)
{
    m_funcs->glDisable(GL_CLIP_DISTANCE0 + clipPlane);
}

GLint QGraphicsHelperGL3::maxClipPlaneCount()
{
    GLint max = 0;
    m_funcs->glGetIntegerv(GL_MAX_CLIP_DISTANCES, &max);
    return max;
}

void QGraphicsHelperGL3::enablePrimitiveRestart(int primitiveRestartIndex)
{
    m_funcs->glPrimitiveRestartIndex(primitiveRestartIndex);
    m_funcs->glEnable(GL_PRIMITIVE_RESTART);
}

void QGraphicsHelperGL3::disablePrimitiveRestart()
{
    m_funcs->glDisable(GL_PRIMITIVE_RESTART);
}

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // !QT_OPENGL_ES_2
