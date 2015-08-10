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

#include "qgraphicshelperes2_p.h"
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <private/attachmentpack_p.h>
#include <private/qgraphicsutils_p.h>

QT_BEGIN_NAMESPACE

// ES 3.0+
#ifndef GL_SAMPLER_3D
#define GL_SAMPLER_3D                     0x8B5F
#endif
#ifndef GL_SAMPLER_2D_SHADOW
#define GL_SAMPLER_2D_SHADOW              0x8B62
#endif
#ifndef GL_SAMPLER_CUBE_SHADOW
#define GL_SAMPLER_CUBE_SHADOW            0x8DC5
#endif
#ifndef GL_SAMPLER_2D_ARRAY
#define GL_SAMPLER_2D_ARRAY               0x8DC1
#endif
#ifndef GL_SAMPLER_2D_ARRAY_SHADOW
#define GL_SAMPLER_2D_ARRAY_SHADOW        0x8DC4
#endif

namespace Qt3D {
namespace Render {

QGraphicsHelperES2::QGraphicsHelperES2() :
    m_funcs(0)
{
}

QGraphicsHelperES2::~QGraphicsHelperES2()
{
}

void QGraphicsHelperES2::initializeHelper(QOpenGLContext *context,
                                          QAbstractOpenGLFunctions *)
{
    Q_ASSERT(context);
    m_funcs = context->functions();
    Q_ASSERT(m_funcs);
    m_isES3 = context->format().majorVersion() >= 3;
}

void QGraphicsHelperES2::drawElementsInstanced(GLenum primitiveType,
                                               GLsizei primitiveCount,
                                               GLint indexType,
                                               void *indices,
                                               GLsizei instances,
                                               GLint baseVertex,
                                               GLint baseInstance)
{
    if (baseInstance != 0)
        qWarning() << "glDrawElementsInstancedBaseVertexBaseInstance is not supported with OpenGL ES 2";

    if (baseVertex != 0)
        qWarning() << "glDrawElementsInstancedBaseVertex is not supported with OpenGL ES 2";

    for (GLint i = 0; i < instances; i++)
        drawElements(primitiveType,
                     primitiveCount,
                     indexType,
                     indices);
}

void QGraphicsHelperES2::drawArraysInstanced(GLenum primitiveType,
                                             GLint first,
                                             GLsizei count,
                                             GLsizei instances)
{
    for (GLint i = 0; i < instances; i++)
        drawArrays(primitiveType,
                   first,
                   count);
}

void QGraphicsHelperES2::drawElements(GLenum primitiveType,
                                      GLsizei primitiveCount,
                                      GLint indexType,
                                      void *indices,
                                      GLint baseVertex)
{
    if (baseVertex != 0)
        qWarning() << "glDrawElementsBaseVertex is not supported with OpenGL ES 2";
    m_funcs->glDrawElements(primitiveType,
                            primitiveCount,
                            indexType,
                            indices);
}

void QGraphicsHelperES2::drawArrays(GLenum primitiveType,
                                    GLint first,
                                    GLsizei count)
{
    m_funcs->glDrawArrays(primitiveType,
                          first,
                          count);
}

void QGraphicsHelperES2::setVerticesPerPatch(GLint verticesPerPatch)
{
    Q_UNUSED(verticesPerPatch);
    qWarning() << "Tessellation not supported with OpenGL ES 2";
}

void QGraphicsHelperES2::useProgram(GLuint programId)
{
    m_funcs->glUseProgram(programId);
}

QVector<ShaderUniform> QGraphicsHelperES2::programUniformsAndLocations(GLuint programId)
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
        uniforms.append(uniform);
    }
    return uniforms;
}

QVector<ShaderAttribute> QGraphicsHelperES2::programAttributesAndLocations(GLuint programId)
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

QVector<ShaderUniformBlock> QGraphicsHelperES2::programUniformBlocks(GLuint programId)
{
    Q_UNUSED(programId);
    QVector<ShaderUniformBlock> blocks;
    qWarning() << "UBO are not supported by OpenGL ES 2.0 (since OpenGL ES 3.0)";
    return blocks;
}

void QGraphicsHelperES2::vertexAttribDivisor(GLuint index, GLuint divisor)
{
    Q_UNUSED(index);
    Q_UNUSED(divisor);
}

void QGraphicsHelperES2::blendEquation(GLenum mode)
{
    m_funcs->glBlendEquation(mode);
}

void QGraphicsHelperES2::alphaTest(GLenum, GLenum)
{
    qCWarning(Render::Rendering) << Q_FUNC_INFO << "AlphaTest not available with OpenGL ES 2.0";
}

void QGraphicsHelperES2::depthTest(GLenum mode)
{
    m_funcs->glEnable(GL_DEPTH_TEST);
    m_funcs->glDepthFunc(mode);
}

void QGraphicsHelperES2::depthMask(GLenum mode)
{
    m_funcs->glDepthMask(mode);
}

void QGraphicsHelperES2::cullFace(GLenum mode)
{
    m_funcs->glEnable(GL_CULL_FACE);
    m_funcs->glCullFace(mode);
}

void QGraphicsHelperES2::frontFace(GLenum mode)
{
    m_funcs->glFrontFace(mode);
}

void QGraphicsHelperES2::enableAlphaCoverage()
{
    m_funcs->glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

void QGraphicsHelperES2::disableAlphaCoverage()
{
    m_funcs->glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

GLuint QGraphicsHelperES2::createFrameBufferObject()
{
    GLuint id;
    m_funcs->glGenFramebuffers(1, &id);
    return id;
}

void QGraphicsHelperES2::releaseFrameBufferObject(GLuint frameBufferId)
{
    m_funcs->glDeleteFramebuffers(1, &frameBufferId);
}

void QGraphicsHelperES2::bindFrameBufferObject(GLuint frameBufferId)
{
    m_funcs->glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
}

GLuint QGraphicsHelperES2::boundFrameBufferObject()
{
    GLint id = 0;
    m_funcs->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &id);
    return id;
}

bool QGraphicsHelperES2::checkFrameBufferComplete()
{
    return (m_funcs->glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void QGraphicsHelperES2::bindFrameBufferAttachment(QOpenGLTexture *texture, const Attachment &attachment)
{
    GLenum attr = GL_COLOR_ATTACHMENT0;

    if (attachment.m_type == QRenderAttachment::ColorAttachment0)
        attr = GL_COLOR_ATTACHMENT0;
    else if (attachment.m_type == QRenderAttachment::DepthAttachment)
        attr = GL_DEPTH_ATTACHMENT;
    else if (attachment.m_type == QRenderAttachment::StencilAttachment)
        attr = GL_STENCIL_ATTACHMENT;
    else
        qCritical() << "Unsupported FBO attachment OpenGL ES 2.0";

    texture->bind();
    QOpenGLTexture::Target target = texture->target();
    if (target == QOpenGLTexture::Target2D)
        m_funcs->glFramebufferTexture2D(GL_FRAMEBUFFER, attr, target, texture->textureId(), attachment.m_mipLevel);
    else if (target == QOpenGLTexture::TargetCubeMap)
        m_funcs->glFramebufferTexture2D(GL_FRAMEBUFFER, attr, attachment.m_face, texture->textureId(), attachment.m_mipLevel);
    else
        qCritical() << "Unsupported Texture FBO attachment format";
    texture->release();
}

bool QGraphicsHelperES2::supportsFeature(QGraphicsHelperInterface::Feature feature) const
{
    switch (feature) {
    default:
        return false;
    }
}
void QGraphicsHelperES2::drawBuffers(GLsizei , const int *)
{
    qCritical() << "drawBuffers is not supported by ES 2.0";
}

void QGraphicsHelperES2::bindUniform(const QVariant &v, const ShaderUniform &description)
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

    case GL_FLOAT_MAT3:
        m_funcs->glUniformMatrix3fv(description.m_location, description.m_size, GL_FALSE,
                                    QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 9));
        break;

    case GL_FLOAT_MAT4:
        m_funcs->glUniformMatrix4fv(description.m_location, description.m_size, GL_FALSE,
                                    QGraphicsUtils::valueArrayFromVariant<GLfloat>(v, description.m_size, 16));
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

    case GL_SAMPLER_2D:
    case GL_SAMPLER_CUBE: {
        Q_ASSERT(description.m_size == 1);
        m_funcs->glUniform1i(description.m_location, v.toInt());
        break;
    }

        // ES 3.0+
    case GL_SAMPLER_3D:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
        if (m_isES3) {
            Q_ASSERT(description.m_size == 1);
            m_funcs->glUniform1i(description.m_location, v.toInt());
        } else {
            qWarning() << Q_FUNC_INFO << "ES 3.0 uniform type" << description.m_type << "for"
                       << description.m_name << "is not supported in ES 2.0";
        }
        break;

    default:
        qWarning() << Q_FUNC_INFO << "unsupported uniform type:" << description.m_type << "for " << description.m_name;
        break;
    }
}

void QGraphicsHelperES2::bindFragDataLocation(GLuint , const QHash<QString, int> &)
{
    qCritical() << "bindFragDataLocation is not supported by ES 2.0";
}

void QGraphicsHelperES2::bindUniformBlock(GLuint programId, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    Q_UNUSED(programId);
    Q_UNUSED(uniformBlockIndex);
    Q_UNUSED(uniformBlockBinding);
    qWarning() << "UBO are not supported by ES 2.0 (since ES 3.0)";
}

void QGraphicsHelperES2::bindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
    Q_UNUSED(target);
    Q_UNUSED(index);
    Q_UNUSED(buffer);
    qWarning() << "bindBufferBase is not supported by ES 2.0 (since ES 3.0)";
}

void QGraphicsHelperES2::buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer)
{
    Q_UNUSED(v);
    Q_UNUSED(description);
    Q_UNUSED(buffer);
    qWarning() << "UBO are not supported by ES 2.0 (since ES 3.0)";
}

uint QGraphicsHelperES2::uniformByteSize(const ShaderUniform &description)
{
    uint rawByteSize = 0;
    int arrayStride = qMax(description.m_arrayStride, 0);
    int matrixStride = qMax(description.m_matrixStride, 0);

    switch (description.m_type) {

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
        rawByteSize = 8;
        break;

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
        rawByteSize = 12;
        break;

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
        rawByteSize = 16;
        break;

    case GL_FLOAT_MAT2:
        rawByteSize = matrixStride ? 2 * matrixStride : 16;
        break;

    case GL_FLOAT_MAT3:
        rawByteSize = matrixStride ? 3 * matrixStride : 36;
        break;

    case GL_FLOAT_MAT4:
        rawByteSize = matrixStride ? 4 * matrixStride : 64;
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
    case GL_SAMPLER_2D:
    case GL_SAMPLER_CUBE:
        rawByteSize = 4;
        break;
    }

    return arrayStride ? rawByteSize * arrayStride : rawByteSize;
}

void QGraphicsHelperES2::enableClipPlane(int)
{
}

void QGraphicsHelperES2::disableClipPlane(int)
{
}

GLint QGraphicsHelperES2::maxClipPlaneCount()
{
    return 0;
}

void QGraphicsHelperES2::enablePrimitiveRestart(int)
{
}

void QGraphicsHelperES2::disablePrimitiveRestart()
{
}

} // Render
} //Qt3D

QT_END_NAMESPACE
