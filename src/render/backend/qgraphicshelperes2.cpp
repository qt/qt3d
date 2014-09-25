/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgraphicshelperes2_p.h"
#include "qopenglfunctions_es2.h"
#include "renderlogging.h"
#include <private/attachmentpack_p.h>

#ifdef QT_OPENGL_ES_2

QT_BEGIN_NAMESPACE

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
                                          QAbstractOpenGLFunctions *functions)
{
    Q_UNUSED(context)
    m_funcs = static_cast<QOpenGLFunctions_ES2*>(functions);
    const bool ok = m_funcs->initializeOpenGLFunctions();
    Q_ASSERT(ok);
    Q_UNUSED(ok);
    // Check Vertex Array Object extension is present
    Q_ASSERT(context->hasExtension(QByteArrayLiteral("GL_OES_vertex_array_object")));
}

void QGraphicsHelperES2::drawElementsInstanced(GLenum primitiveType,
                                               GLsizei primitiveCount,
                                               GLint indexType,
                                               void *indices,
                                               GLsizei instances)
{
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
                                      void *indices)
{
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

QVector<QPair<QString, int> > QGraphicsHelperES2::programUniformsAndLocations(GLuint programId)
{
    QVector<QPair<QString, int> > uniforms;

    GLint nbrActiveUniforms = 0;
    m_funcs->glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &nbrActiveUniforms);
    uniforms.resize(nbrActiveUniforms);
    for (GLint i = 0; i < nbrActiveUniforms; i++) {
        QByteArray uniformName(256, '\0');
        GLenum type;
        GLint size;
        // Size is 1 for scalar and more for struct or arrays
        // Type is the GL Type
        m_funcs->glGetActiveUniform(programId, i, 256, NULL, &size, &type , uniformName.data());
        GLint location = m_funcs->glGetUniformLocation(programId, uniformName.constData());
        uniforms.append(QPair<QString, int>(QString::fromUtf8(uniformName), location));
    }
    return uniforms;
}

QVector<QPair<QString, int> > QGraphicsHelperES2::programAttributesAndLocations(GLuint programId)
{
    QVector<QPair<QString, int> > attributes;

    GLint nbrActiveAttributes = 0;
    m_funcs->glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &nbrActiveAttributes);
    for (GLint i = 0; i < nbrActiveAttributes; i++) {
        QByteArray attributeName(256, '\0');
        GLenum type;
        GLint size;
        // Size is 1 for scalar and more for struct or arrays
        // Type is the GL Type
        m_funcs->glGetActiveAttrib(programId, i, 256, NULL, &size, &type , attributeName.data());
        GLint location = m_funcs->glGetAttribLocation(programId, attributeName.constData());
        attributes.append(QPair<QString, int>(QString::fromUtf8(attributeName), location));
    }
    return attributes;
}

void QGraphicsHelperES2::vertexAttribDivisor(GLuint index, GLuint divisor)
{
    Q_UNUSED(index)
    Q_UNUSED(divisor)
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
    case MRT:
        return false;
    case Tessellation:
        return false;
    case UniformBufferObject:
        return false;
    default:
        return false;
    }
}
void QGraphicsHelperES2::drawBuffers(GLsizei , const int *)
{
    qCritical() << "drawBuffers is not supported by ES 2.0";
}

void QGraphicsHelperES2::bindFragDataLocation(GLuint , const QHash<QString, int> &)
{
    qCritical() << "bindFragDataLocation is not supported by ES 2.0";
}

} // Render
} //Qt3D

QT_END_NAMESPACE

#endif // QT_OPENGL_ES_2


