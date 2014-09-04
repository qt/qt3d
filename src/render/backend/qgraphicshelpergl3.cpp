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

#ifndef QT_OPENGL_ES_2

#include "qgraphicshelpergl3_p.h"
#include <QOpenGLFunctions_3_2_Core>
#include "renderlogging.h"
#include <private/attachmentpack_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

QGraphicsHelperGL3::QGraphicsHelperGL3() :
    m_funcs(0)
{
}

void QGraphicsHelperGL3::initializeHelper(QOpenGLContext *context,
                                          QAbstractOpenGLFunctions *functions)
{
    Q_UNUSED(context)
    m_funcs = static_cast<QOpenGLFunctions_3_2_Core*>(functions);
    const bool ok = m_funcs->initializeOpenGLFunctions();
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void QGraphicsHelperGL3::drawElementsInstanced(GLenum primitiveType,
                                               GLsizei primitiveCount,
                                               GLint indexType,
                                               void *indices,
                                               GLsizei instances)
{
    // glDrawElements OpenGL 3.1 or greater
    m_funcs->glDrawElementsInstanced(primitiveType,
                                     primitiveCount,
                                     indexType,
                                     indices,
                                     instances);
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
                                      void *indices)
{
    m_funcs->glDrawElements(primitiveType,
                            primitiveCount,
                            indexType,
                            indices);
}

void QGraphicsHelperGL3::drawArrays(GLenum primitiveType,
                                    GLint first,
                                    GLsizei count)
{
    m_funcs->glDrawArrays(primitiveType,
                          first,
                          count);
}

void QGraphicsHelperGL3::useProgram(GLuint programId)
{
    m_funcs->glUseProgram(programId);
}

QVector<QPair<QString, int> > QGraphicsHelperGL3::programUniformsAndLocations(GLuint programId)
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

QVector<QPair<QString, int> > QGraphicsHelperGL3::programAttributesAndLocations(GLuint programId)
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

void QGraphicsHelperGL3::vertexAttribDivisor(GLuint index, GLuint divisor)
{
    Q_UNUSED(index)
    Q_UNUSED(divisor)
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

bool QGraphicsHelperGL3::supportUniformBlock() const
{
    return true;
}

GLuint QGraphicsHelperGL3::createFrameBufferObject()
{
    qDebug() << Q_FUNC_INFO;
    GLuint id;
    m_funcs->glGenFramebuffers(1, &id);
    qDebug() << Q_FUNC_INFO << id;
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
        return true;
    default:
        return false;
    }
}


void QGraphicsHelperGL3::drawBuffers(GLsizei n, const int *bufs)
{
    GLenum drawBufs[n];

    for (int i = 0; i < n; i++)
        drawBufs[i] = GL_COLOR_ATTACHMENT0 + bufs[i];
    m_funcs->glDrawBuffers(n, drawBufs);
}

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // !QT_OPENGL_ES_2
