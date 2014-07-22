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
#include "qparameter.h"
#include "drawstate.h"
#include "qgraphicshelperinterface.h"
#include "qopenglfilter.h"
#include "renderer.h"
#include "texturemanager.h"
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
    , m_gl(Q_NULLPTR)
    , m_surface(Q_NULLPTR)
    , m_glHelper(Q_NULLPTR)
    , m_activeShader(Q_NULLPTR)
    , m_camera(Q_NULLPTR)
    , m_material(Q_NULLPTR)
    , m_stateSet(Q_NULLPTR)
    , m_contextInfo(new QOpenGLFilter())
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
    resolveHighestOpenGLFunctions();
}

// That assumes that the shaderProgram in RenderShader stays the same
void QGraphicsContext::activateShader(RenderShader *shader)
{
    if (shader == Q_NULLPTR) {
        m_activeShader = Q_NULLPTR;
        m_material = Q_NULLPTR;
        m_glHelper->useProgram(0);
        return;
    }

    if (!m_shaderHash.contains(shader)) {
        QOpenGLShaderProgram* prog = shader->getOrCreateProgram();
        Q_ASSERT(prog);
        m_shaderHash[shader] = prog;
        shader->initializeUniforms(m_glHelper->programUniformsAndLocations(prog->programId()));
        shader->initializeAttributes(m_glHelper->programAttributesAndLocations(prog->programId()));
        // TO DO : Set Attributes m_glHelper->programAttributesAndLocations(prog->programId());
        m_activeShader = Q_NULLPTR;
    }

    if (m_activeShader == shader) {

        // no op
    } else {
        m_activeShader = shader;
        QOpenGLShaderProgram* prog = m_shaderHash[shader];
        prog->bind();
        // ensure material uniforms are re-applied
        m_material = Q_NULLPTR;
    }
}

void QGraphicsContext::setActiveMaterial(RenderMaterial *rmat)
{
    if (m_material == rmat)
        return;

    deactivateTexturesWithScope(TextureScopeMaterial);
    m_material = rmat;
}

// TO DO : Try to move what's in Renderer here
void QGraphicsContext::executeCommand(const RenderCommand *command)
{
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

    if (m_gl->isOpenGLES()) {
#if defined QT_OPENGL_ES_2
        glFunctions = m_gl->versionFunctions<QOpenGLFunctions_ES2>();
        m_glHelper = new QGraphicsHelperES2();
#endif
        qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 2/ES2 Helper";
    }
    else if ((glFunctions = m_gl->versionFunctions<QOpenGLFunctions_3_2_Core>()) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 3.2";
        m_glHelper = new QGraphicsHelperGL3();
    }
    else if ((glFunctions = m_gl->versionFunctions<QOpenGLFunctions_2_0>()) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 2 Helper";
        m_glHelper = new QGraphicsHelperGL2();
    }
    if (m_glHelper != Q_NULLPTR)
        m_glHelper->initializeHelper(m_gl, glFunctions);

    // Set Vendor and Extensions of reference OpenGLFilter
    QStringList extensions;
    Q_FOREACH (const QByteArray &ext, m_gl->extensions().values())
        extensions << QString::fromUtf8(ext);
    m_contextInfo->setMajorVersion(m_gl->format().version().first);
    m_contextInfo->setMinorVersion(m_gl->format().version().second);
    m_contextInfo->setApi(m_gl->isOpenGLES() ? QOpenGLFilter::ES : QOpenGLFilter::Desktop);
    m_contextInfo->setProfile(static_cast<QOpenGLFilter::Profile>(m_gl->format().profile()));
    m_contextInfo->setExtensions(extensions);
    m_contextInfo->setVendor(QString::fromUtf8(reinterpret_cast<const char *>(glGetString(GL_VENDOR))));
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

QOpenGLFilter *QGraphicsContext::contextInfo() const
{
    return m_contextInfo;
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

void QGraphicsContext::alphaTest(GLenum mode1, GLenum mode2)
{
    m_glHelper->alphaTest(mode1, mode2);
}

void QGraphicsContext::depthTest(GLenum mode)
{
    m_glHelper->depthTest(mode);
}

void QGraphicsContext::depthMask(GLenum mode)
{
    m_glHelper->depthMask(mode);
}

void QGraphicsContext::cullFace(GLenum mode)
{
    m_glHelper->cullFace(mode);
}

void QGraphicsContext::frontFace(GLenum mode)
{
    m_glHelper->frontFace(mode);
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

void QGraphicsContext::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

// It will be easier if the QGraphicContext applies the QUniformPack
// than the other way around
void QGraphicsContext::setUniforms(const QUniformPack &uniforms)
{
    // Activate textures and update TextureUniform in the pack
    // with the correct textureUnit
    deactivateTexturesWithScope(TextureScopeMaterial);

    QHash<QString, const QUniformValue *> uniformValues = uniforms.uniforms();
    Q_FOREACH (const QUniformPack::NamedTexture &namedTex, uniforms.textures()) {
        RenderTexture *t = m_renderer->textureManager()->lookupResource(namedTex.texId);
        int texUnit = -1;
        const TextureUniform *texUniform = Q_NULLPTR;
        // TO DO : Rework the way textures are loaded
        if (t != Q_NULLPTR &&
                (texUnit = activateTexture(TextureScopeMaterial, t)) != -1 &&
                uniformValues.contains(namedTex.glslName) &&
                (texUniform = static_cast<const TextureUniform *>(uniformValues[namedTex.glslName])) != Q_NULLPTR) {
            const_cast<TextureUniform *>(texUniform)->setTextureUnit(texUnit);
        }
    }
    m_activeShader->updateUniforms(uniforms);
}

void QGraphicsContext::specifyAttribute(QString nm, AttributePtr attr)
{
    QOpenGLBuffer buf = glBufferFor(attr->buffer().staticCast<Buffer>());
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
    if (attr->buffer().staticCast<Buffer>()->type() != QOpenGLBuffer::IndexBuffer) {
        qCWarning(Backend) << Q_FUNC_INFO << "provided buffer is not correct type";
        return;
    }

    QOpenGLBuffer buf = glBufferFor(attr->buffer().staticCast<Buffer>());
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
