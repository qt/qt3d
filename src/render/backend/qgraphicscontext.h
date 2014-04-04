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

#ifndef QT3D_RENDER_QGRAPHICSCONTEXT_H
#define QT3D_RENDER_QGRAPHICSCONTEXT_H

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QHash>
#include <QMatrix4x4>
#include <QBitArray>

#include <Qt3DRenderer/rendercamera.h>
#include <Qt3DRenderer/meshdata.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;
class QAbstractOpenGLFunctions;

namespace Qt3D {
namespace Render {

class QGraphicsHelperInterface;
class DrawStateSet;
class RenderShader;
class RenderCamera;
class RenderMaterial;
class RenderTexture;

enum TextureScope
{
    TextureScopeMaterial = 0,
    TextureScopeTechnique
    // per-pass for deferred rendering?
};

class QGraphicsContext
{
public:
    QGraphicsContext();
    ~QGraphicsContext();

    void setSurface(QSurface* s);

    int id() const; // unique, small integer ID of this context

    void beginDrawing();

    void endDrawing();

    void setCamera(RenderCamera* rcam);
    RenderCamera* camera() const;

    QMatrix4x4 projectionMatrix() const;
    QMatrix4x4 viewMatrix() const;

    /**
     * @brief releaseGL - release all OpenGL objects associated with
     * this context
     */
    void releaseOpenGL();
    void setOpenGLContext(QOpenGLContext* ctx);
    QOpenGLContext *openGLContext() { return m_gl; }

    void activateShader(RenderShader* shader);

    RenderMaterial* activeMaterial() const
    { return m_material; }

    void setActiveMaterial(RenderMaterial* rmat);

    /**
     * @brief activeShader
     * @return
     */
    QOpenGLShaderProgram* activeShader();

    void specifyAttribute(QString nm, AttributePtr attr);

    void specifyIndices(AttributePtr attr);

    /**
     * @brief glBufferFor - given a client-side (CPU) buffer, provide the
     * context-specific object. Initial call will create the buffer.
     * @param buf
     * @return
     */
    QOpenGLBuffer glBufferFor(BufferPtr buf);

    void setModelMatrix(const QMatrix4x4 &modelMat);

    /**
     * @brief activateTexture - make a texture active on a hardware unit
     * @param tex - the texture to activate
     * @param onUnit - option, specify the explicit unit to activate on
     * @return - the unit the texture was activated on
     */
    int activateTexture(TextureScope scope, RenderTexture* tex, int onUnit = -1);

    void deactivateTexture(RenderTexture *tex);

    void setCurrentStateSet(DrawStateSet* ss);
    DrawStateSet* currentStateSet() const;

    // Wrapper methods
    void    drawElementsInstanced(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void * indices, GLsizei instances);
    void    drawArraysInstanced(GLenum primitiveType, GLint first, GLsizei count, GLsizei instances);
    void    drawElements(GLenum primitiveType, GLsizei primitiveCount, GLint indexType, void * indices);
    void    drawArrays(GLenum primitiveType, GLint first, GLsizei count);
    void    blendEquation(GLenum mode);

private:
    void initialize();

    void decayTextureScores();

    GLint assignUnitForTexture(RenderTexture* tex);
    void deactivateTexturesWithScope(TextureScope ts);

    void resolveHighestOpenGLFunctions();

    bool m_initialized;
    const unsigned int m_id;
    QOpenGLContext* m_gl;
    QSurface* m_surface;
    QGraphicsHelperInterface* m_glHelper;

    RenderShader* m_activeShader;
    QHash<RenderShader*, QOpenGLShaderProgram*> m_shaderHash;
    QHash<BufferPtr,QOpenGLBuffer> m_bufferHash;

    // active textures, indexed by texture unit
    QVector<RenderTexture*> m_activeTextures;
    QBitArray m_pinnedTextureUnits;
    QVector<TextureScope> m_textureScopes;

    // recency score for all render-textures we've seen. Higher scores
    // mean more recently used.
    QHash<RenderTexture*, int> m_textureScores;

    RenderCamera* m_camera;
    RenderMaterial* m_material;

    DrawStateSet* m_stateSet;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QGRAPHICSCONTEXT_H
