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

#include "qgraphicscontext.h"

#include "rendercamera.h"
#include "rendershader.h"
#include "rendermaterial.h"
#include "rendertexture.h"
#include "rendercommand.h"
#include "parameter.h"
#include "drawstate.h"
#include "qgraphicshelperinterface.h"

#include "renderlogging.h"
#include <QOpenGLShaderProgram>

#if !defined(QT_OPENGL_ES_2)
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions_3_2_Core>
#include "qgraphicshelpergl2.h"
#include "qgraphicshelpergl3.h"
#else
#include <QOpenGLFunctions_ES2>
#include "qgraphicshelperes2.h"
#endif

#include <QSurface>
#include <QOpenGLTexture>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

static QHash<unsigned int, QGraphicsContext*> static_contexts;

unsigned int nextFreeContextId()
{
    for (unsigned int i=0; i < 0xffff; ++i) {
        if (!static_contexts.contains(i))
            return i;
    }

    qFatal("Couldn't find free context ID");
    return 0;
}

QGraphicsContext::QGraphicsContext()
    : m_initialized(false)
    , m_id(nextFreeContextId())
    , m_gl(0)
    , m_surface(0)
    , m_glHelper(0)
    , m_activeShader(0)
    , m_camera(0)
    , m_material(0)
    , m_stateSet(0)
{
    static_contexts[m_id] = this;
}

QGraphicsContext::~QGraphicsContext()
{
    releaseOpenGL();

    Q_ASSERT(static_contexts[m_id] == this);
    static_contexts.remove(m_id);
}

void QGraphicsContext::setSurface(QSurface *s)
{
    qCDebug(Backend) << Q_FUNC_INFO;
    m_surface = s;
}

void QGraphicsContext::initialize()
{
    m_initialized = true;

    Q_ASSERT(m_gl);

    GLint numTexUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTexUnits);
    qCDebug(Backend) << "context supports" << numTexUnits << "texture units";

    m_pinnedTextureUnits = QBitArray(numTexUnits);
    m_activeTextures.resize(numTexUnits);
    m_textureScopes.resize(numTexUnits);
}

void QGraphicsContext::beginDrawing()
{
    if (!m_gl || !m_surface) {
        qCWarning(Backend) << Q_FUNC_INFO << "no content or surface provided";
        return;
    }

    bool ok = m_gl->makeCurrent(m_surface);
    if (!ok) {
        qCWarning(Backend) << Q_FUNC_INFO << "make current failed";
    }

    GLint err = glGetError();
    if (err != 0) {
        qCWarning(Backend) << Q_FUNC_INFO << "glGetError:" << err;
    }

    if (!m_initialized) {
        initialize();
    }

    QVector4D cc(0.5, 0.5, 1.0, 1.0);
    glClearColor(cc[0], cc[1], cc[2], cc[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_activeShader)
        m_activeShader = NULL;
}

void QGraphicsContext::endDrawing()
{
    m_gl->doneCurrent();
    m_gl->swapBuffers(m_surface);

    decayTextureScores();
}

void QGraphicsContext::setCamera(RenderCamera *rcam)
{
    m_camera = rcam;
}

RenderCamera *QGraphicsContext::camera() const
{
    return m_camera;
}

void QGraphicsContext::setViewport(const QRectF &viewport)
{
    m_viewport = viewport;
    // Qt3D 0------------------> 1  OpenGL  1^
    //      |                                |
    //      |                                |
    //      |                                |
    //      V                                |
    //      1                                0---------------------> 1
    // The Viewport is defined between 0 and 1 which allows us to automatically
    // scale to the size of the provided window surface
    glViewport(m_viewport.x() * m_surface->size().width(),
               (1.0 - m_viewport.y() - m_viewport.height()) * m_surface->size().height(),
               m_viewport.width()* m_surface->size().width(),
               m_viewport.height() * m_surface->size().height());
}


QMatrix4x4 QGraphicsContext::projectionMatrix() const
{
    return m_camera->projection();
}

QMatrix4x4 QGraphicsContext::viewMatrix() const
{
    return m_camera->viewMatrix();
}

void QGraphicsContext::releaseOpenGL()
{
    // m_shaderHash.clear
    m_bufferHash.clear();
}

void QGraphicsContext::setOpenGLContext(QOpenGLContext* ctx)
{
    releaseOpenGL();
    m_gl = ctx;
    //    m_gl->setParent(this);

    // The Context should be made current to the surface
    // otherwise gl functions initialization fails

    Q_ASSERT(m_surface);
    m_gl->makeCurrent(m_surface);
    this->resolveHighestOpenGLFunctions();
}

void QGraphicsContext::activateShader(RenderShader *shader)
{
    if (shader == NULL) {
        m_activeShader = NULL;
        m_material = NULL;
        m_glHelper->useProgram(0);
        return;
    }

    if (!m_shaderHash.contains(shader)) {
        QOpenGLShaderProgram* prog = shader->getOrCreateProgram();
        Q_ASSERT(prog);
        m_shaderHash[shader] = prog;
        m_activeShader = NULL;
    }

    if (m_activeShader == shader) {
        // no op
    } else {
        m_activeShader = shader;
        QOpenGLShaderProgram* prog = m_shaderHash[shader];
        prog->bind();
        // ensure material uniforms are re-applied
        m_material = NULL;
    }
}

void QGraphicsContext::setActiveMaterial(RenderMaterial *rmat)
{
    if (m_material == rmat)
        return;

    deactivateTexturesWithScope(TextureScopeMaterial);
    m_material = rmat;
}

void QGraphicsContext::executeCommand(const RenderCommand *command)
{

}

void QGraphicsContext::setModelMatrix(const QMatrix4x4& modelMat)
{
    const QVector<int>& locs(m_activeShader->standardUniformLocations());
    QOpenGLShaderProgram* prog = activeShader();

    GLint progId = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &progId);
    if (progId != (int) prog->programId()) {
        qCWarning(Backend) << "current program mismatch, very bad:" << progId << prog->programId();
        return;
    }

    int sz = locs.size();
    for (int i=0; i<sz; ++i) {
        Parameter::StandardUniform su = static_cast<Parameter::StandardUniform>(i);
        int loc = locs.at(i);
        if (loc == -1)
            continue;

        switch (su) {
        case Parameter::None: break;

        case Parameter::ModelMatrix:
            prog->setUniformValue(loc, modelMat); break;

        case Parameter::ViewMatrix:
            prog->setUniformValue(loc, viewMatrix()); break;

        case Parameter::ProjectionMatrix:
            prog->setUniformValue(loc, projectionMatrix()); break;

        case Parameter::ModelView:
            prog->setUniformValue(loc, viewMatrix() * modelMat); break;

        case Parameter::ModelViewProjection:
            prog->setUniformValue(loc, projectionMatrix() * viewMatrix() * modelMat); break;

        case Parameter::ModelInverse:
            prog->setUniformValue(loc, modelMat.inverted()); break;

        case Parameter::ViewInverse:
            prog->setUniformValue(loc, viewMatrix().inverted()); break;

        case Parameter::ProjectionInverse:
            prog->setUniformValue(loc, projectionMatrix().inverted()); break;

        case Parameter::ModelViewInverse:
            prog->setUniformValue(loc, (viewMatrix() * modelMat).inverted()); break;

        case Parameter::ModelViewProjectionInverse:
            prog->setUniformValue(loc, (projectionMatrix() * viewMatrix() * modelMat).inverted());
            break;

        case Parameter::ModelNormal:
            prog->setUniformValue(loc, modelMat.normalMatrix());
            break;

        case Parameter::ModelViewNormal:
            prog->setUniformValue(loc, (viewMatrix() * modelMat).normalMatrix());
            break;
        }

        int err = glGetError();
        if (err)
            qCWarning(Backend) << "GL error after setting matrix" << QString::number(err, 16) << i << loc;

    } // of standard uniforms
}

int QGraphicsContext::activateTexture(TextureScope scope, RenderTexture *tex, int onUnit)
{
    if (onUnit == -1) {
        onUnit = assignUnitForTexture(tex);

        // check we didn't overflow the available units
        if (onUnit == -1)
            return onUnit;
    }

    // actually re-bind if required
    if (m_activeTextures[onUnit] != tex) {
        QOpenGLTexture* glTex = tex->getOrCreateGLTexture();
        glTex->bind(onUnit);
    }

    int err = glGetError();
    if (err)
        qCWarning(Backend) << "GL error after activating texture" << QString::number(err, 16)
                   << tex->textureId() << "on unit" << onUnit;

    m_textureScores[tex] = 200;
    m_pinnedTextureUnits[onUnit] = true;
    m_textureScopes[onUnit] = scope;

    return onUnit;
}

void QGraphicsContext::deactivateTexturesWithScope(TextureScope ts)
{
    for (int u=0; u<m_activeTextures.size(); ++u) {
        if (!m_pinnedTextureUnits[u])
            continue; // inactive, ignore

        if (m_textureScopes[u] == ts) {
            m_pinnedTextureUnits[u] = false;
        }
    } // of units iteration
}

/*!
 * Finds the highest supported opengl version and internally use the most optimized
 * helper for a given version.
 */
void QGraphicsContext::resolveHighestOpenGLFunctions()
{
    Q_ASSERT(m_gl);

    QAbstractOpenGLFunctions *glFunctions = Q_NULLPTR;

#if defined QT_OPENGL_ES_2
    if ((glFunctions = m_gl->versionFunctions<QOpenGLFunctions_ES2>()) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 2/ES2 Helper";
        m_glHelper = new QGraphicsHelperES2();
    }
#else
    if ((glFunctions = m_gl->versionFunctions<QOpenGLFunctions_3_2_Core>()) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 3.2";
        m_glHelper = new QGraphicsHelperGL3();
    }
    else if ((glFunctions = m_gl->versionFunctions<QOpenGLFunctions_2_0>()) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 2 Helper";
        m_glHelper = new QGraphicsHelperGL2();
    }
#endif
    if (m_glHelper != Q_NULLPTR)
        m_glHelper->initializeHelper(m_gl, glFunctions);
}

void QGraphicsContext::deactivateTexture(RenderTexture* tex)
{
    for (int u=0; u<m_activeTextures.size(); ++u) {
        if (m_activeTextures[u] == tex) {
            Q_ASSERT(m_pinnedTextureUnits[u]);
            m_pinnedTextureUnits[u] = false;
            return;
        }
    } // of units iteration

    qCWarning(Backend) << Q_FUNC_INFO << "texture not active:" << tex;
}

void QGraphicsContext::setCurrentStateSet(DrawStateSet *ss)
{
    if (ss == m_stateSet)
        return;

    ss->apply(this);
    m_stateSet = ss;
}

DrawStateSet *QGraphicsContext::currentStateSet() const
{
    return m_stateSet;
}

/*!
 * Wraps an OpenGL call to glDrawElementsInstanced.
 * If the call is not supported by the system's OpenGL version,
 * it is simulated with a loop.
 */
void QGraphicsContext::drawElementsInstanced(GLenum primitiveType,
                                             GLsizei primitiveCount,
                                             GLint indexType,
                                             void *indices,
                                             GLsizei instances)
{
    m_glHelper->drawElementsInstanced(primitiveType,
                                      primitiveCount,
                                      indexType,
                                      indices,
                                      instances);
}

/*!
 * Wraps an OpenGL call to glDrawArraysInstanced.
 */
void QGraphicsContext::drawArraysInstanced(GLenum primitiveType,
                                           GLint first,
                                           GLsizei count,
                                           GLsizei instances)
{
    m_glHelper->drawArraysInstanced(primitiveType,
                                    first,
                                    count,
                                    instances);
}

/*!
 * Wraps an OpenGL call to glDrawElements.
 */
void QGraphicsContext::drawElements(GLenum primitiveType,
                                    GLsizei primitiveCount,
                                    GLint indexType,
                                    void *indices)
{
    m_glHelper->drawElements(primitiveType,
                             primitiveCount,
                             indexType,
                             indices);
}

/*!
 * Wraps an OpenGL call to glDrawArrays.
 */
void QGraphicsContext::drawArrays(GLenum primitiveType,
                                  GLint first,
                                  GLsizei count)
{
    m_glHelper->drawArrays(primitiveType,
                           first,
                           count);
}

void QGraphicsContext::blendEquation(GLenum mode)
{
    m_glHelper->blendEquation(mode);
}

GLint QGraphicsContext::assignUnitForTexture(RenderTexture *tex)
{
    int lowestScoredUnit = -1;
    int lowestScore = 0xfffffff;

    for (int u=0; u<m_activeTextures.size(); ++u) {
        if (m_activeTextures[u] == tex) {
            return u;
        }

        if (!m_pinnedTextureUnits[u]) {
            int score = m_textureScores[tex];
            if (score < lowestScore) {
                lowestScore = score;
                lowestScoredUnit = u;
            }
        }
    } // of units iteration

    if (lowestScoredUnit == -1) {
        qCWarning(Backend) << Q_FUNC_INFO << "NO free texture units!";
        return GL_INVALID_VALUE;
    }

    return lowestScoredUnit;
}

void QGraphicsContext::decayTextureScores()
{
    // FIXME - very inefficient use of QHash here, both for
    // traversal coherency and subsequent modification.
    Q_FOREACH (RenderTexture* t, m_textureScores.keys()) {
        if ((m_textureScores[t]--) <= 0) {
            qCDebug(Backend) << "removing inactive texture" << t;
            m_textureScores.remove((t));
        }
    }
}

QOpenGLShaderProgram* QGraphicsContext::activeShader()
{
    Q_ASSERT(m_activeShader);
    return m_shaderHash[m_activeShader];
}


void QGraphicsContext::specifyAttribute(QString nm, AttributePtr attr)
{
    QOpenGLBuffer buf = glBufferFor(attr->buffer());
    buf.bind();

    QOpenGLShaderProgram* prog = activeShader();
    int location = prog->attributeLocation(nm);
    if (location < 0) {
        qCWarning(Backend) << "failed to resolve location for attribute:" << nm;
        return;
    }
    prog->enableAttributeArray(location);
    prog->setAttributeBuffer(location,
                             elementType(attr->type()),
                             attr->byteOffset(),
                             tupleSizeFromType(attr->type()),
                             attr->byteStride());

    if (attr->divisor() != 0) {
        // Done by the helper if it supports it
        m_glHelper->vertexAttribDivisor(location, attr->divisor());
    }

    buf.release();
}

void QGraphicsContext::specifyIndices(AttributePtr attr)
{
    if (attr->buffer()->type() != QOpenGLBuffer::IndexBuffer) {
        qCWarning(Backend) << Q_FUNC_INFO << "provided buffer is not correct type";
        return;
    }

    QOpenGLBuffer buf = glBufferFor(attr->buffer());
    if (!buf.bind())
        qCWarning(Backend) << Q_FUNC_INFO << "binding index buffer failed";

    // bind within the current VAO
}

QOpenGLBuffer QGraphicsContext::glBufferFor(BufferPtr buf)
{
    if (m_bufferHash.contains(buf))
        return m_bufferHash.value(buf);

    QOpenGLBuffer glbuf = buf->createGL();
    m_bufferHash[buf] = glbuf;
    buf->upload(glbuf);

    return glbuf;
}

} // Render
} // Qt3D of namespace

QT_END_NAMESPACE
