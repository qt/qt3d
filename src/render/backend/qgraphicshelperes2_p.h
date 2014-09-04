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

#ifndef QT3D_RENDER_QGRAPHICSHELPERES2_H
#define QT3D_RENDER_QGRAPHICSHELPERES2_H

#include <Qt3DRenderer/private/qgraphicshelperinterface_p.h>
#include <QOpenGLContext>

#ifdef QT_OPENGL_ES_2

QT_BEGIN_NAMESPACE

class QOpenGLFunctions_ES2;

namespace Qt3D {
namespace Render {

class QGraphicsHelperES2 : public QGraphicsHelperInterface
{
public:
    QGraphicsHelperES2();
    virtual ~QGraphicsHelperES2();

    // QGraphicHelperInterface interface
    void initializeHelper(QOpenGLContext *context, QAbstractOpenGLFunctions *functions) Q_DECL_OVERRIDE;
    void drawElementsInstanced(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void *indices, GLsizei instances) Q_DECL_OVERRIDE;
    void drawArraysInstanced(GLenum primitiveType, GLint first, GLsizei count, GLsizei instances) Q_DECL_OVERRIDE;
    void drawElements(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void *indices) Q_DECL_OVERRIDE;
    void drawArrays(GLenum primitiveType, GLint first, GLsizei count) Q_DECL_OVERRIDE;
    void useProgram(GLuint programId) Q_DECL_OVERRIDE;
    QVector<QPair<QString, int> > programUniformsAndLocations(GLuint programId) Q_DECL_OVERRIDE;
    QVector<QPair<QString, int> > programAttributesAndLocations(GLuint programId) Q_DECL_OVERRIDE;
    void vertexAttribDivisor(GLuint index, GLuint divisor) Q_DECL_OVERRIDE;
    void blendEquation(GLenum mode) Q_DECL_OVERRIDE;
    void alphaTest(GLenum mode1, GLenum mode2) Q_DECL_OVERRIDE;
    void depthTest(GLenum mode) Q_DECL_OVERRIDE;
    void depthMask(GLenum mode) Q_DECL_OVERRIDE;
    void cullFace(GLenum mode) Q_DECL_OVERRIDE;
    void frontFace(GLenum mode) Q_DECL_OVERRIDE;
    bool supportUniformBlock() const Q_DECL_OVERRIDE;
    GLuint createFrameBufferObject() Q_DECL_OVERRIDE;
    void releaseFrameBufferObject(GLuint frameBufferId) Q_DECL_OVERRIDE;
    void bindFrameBufferObject(GLuint frameBufferId) Q_DECL_OVERRIDE;
    bool checkFrameBufferComplete() Q_DECL_OVERRIDE;
    void bindFrameBufferAttachment(QOpenGLTexture *texture, const Attachment &attachment) Q_DECL_OVERRIDE;
    bool supportsFeature(Feature feature) const Q_DECL_OVERRIDE;
    void drawBuffers(GLsizei n, const int *bufs) Q_DECL_OVERRIDE;

private:
    QOpenGLFunctions_ES2 *m_funcs;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT_OPENGL_ES2

#endif // QT3D_RENDER_QGRAPHICSHELPERES2_H
