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

#include "qgraphicshelpergl2.h"
#include <QOpenGLFunctions_2_0>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

QGraphicsHelperGL2::QGraphicsHelperGL2() :
    m_funcs(0)
{

}

void QGraphicsHelperGL2::initializeHelper(QOpenGLContext *context,
                                          QAbstractOpenGLFunctions *functions)
{
    Q_UNUSED(context)
    m_funcs = static_cast<QOpenGLFunctions_2_0*>(functions);
    const bool ok = m_funcs->initializeOpenGLFunctions();
    Q_ASSERT(ok);
    Q_UNUSED(ok);
    // Check Vertex Array Object extension is present
    Q_ASSERT(context->hasExtension(QByteArrayLiteral("GL_ARB_vertex_array_object")));
}

void QGraphicsHelperGL2::drawElementsInstanced(GLenum primitiveType,
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

void QGraphicsHelperGL2::drawArraysInstanced(GLenum primitiveType,
                                             GLint first,
                                             GLsizei count,
                                             GLsizei instances)
{
    for (GLint i = 0; i < instances; i++)
        drawArrays(primitiveType,
                   first,
                   count);
}

void QGraphicsHelperGL2::drawElements(GLenum primitiveType,
                                      GLsizei primitiveCount,
                                      GLint indexType,
                                      void *indices)
{
    m_funcs->glDrawElements(primitiveType,
                            primitiveCount,
                            indexType,
                            indices);
}

void QGraphicsHelperGL2::drawArrays(GLenum primitiveType,
                                    GLint first,
                                    GLsizei count)
{
    m_funcs->glDrawArrays(primitiveType,
                          first,
                          count);
}

void QGraphicsHelperGL2::useProgram(GLuint programId)
{
    m_funcs->glUseProgram(programId);
}

QVector<QPair<QString, int> > QGraphicsHelperGL2::programUniformsAndLocations(GLuint programId)
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

QVector<QPair<QString, int> > QGraphicsHelperGL2::programAttributesAndLocations(GLuint programId)
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

void QGraphicsHelperGL2::vertexAttribDivisor(GLuint index,
                                             GLuint divisor)
{
    Q_UNUSED(index)
    Q_UNUSED(divisor)
}

void QGraphicsHelperGL2::blendEquation(GLenum mode)
{
    m_funcs->glBlendEquation(mode);
}

void QGraphicsHelperGL2::alphaTest(GLenum mode1, GLenum mode2)
{
    m_funcs->glEnable(GL_ALPHA_TEST);
    m_funcs->glAlphaFunc(mode1, mode2);
}

void QGraphicsHelperGL2::depthTest(GLenum mode)
{
    m_funcs->glEnable(GL_DEPTH_TEST);
    m_funcs->glDepthFunc(mode);
}

void QGraphicsHelperGL2::depthMask(GLenum mode)
{
    m_funcs->glDepthMask(mode);
}

void QGraphicsHelperGL2::cullFace(GLenum mode)
{
    m_funcs->glEnable(GL_CULL_FACE);
    m_funcs->glCullFace(mode);
}

void QGraphicsHelperGL2::frontFace(GLenum mode)
{
    m_funcs->glFrontFace(mode);
}

bool QGraphicsHelperGL2::supportUniformBlock() const
{
    return false;
}

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // !QT_OPENGL_ES_2
