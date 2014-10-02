/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#include "qgraphicscontext_p.h"

#include <Qt3DRenderer/qopenglfilter.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/renderlogging.h>
#include <Qt3DRenderer/private/rendershader_p.h>
#include <Qt3DRenderer/private/rendermaterial_p.h>
#include <Qt3DRenderer/private/rendertexture_p.h>
#include <Qt3DRenderer/private/rendercommand_p.h>
#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DRenderer/private/qgraphicshelperinterface_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/texturemanager_p.h>
#include <Qt3DRenderer/private/attachmentpack_p.h>
#include <QOpenGLShaderProgram>

#if !defined(QT_OPENGL_ES_2)
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions_3_2_Core>
#include <Qt3DRenderer/private/qgraphicshelpergl2_p.h>
#include <Qt3DRenderer/private/qgraphicshelpergl3_p.h>
#else
#include <QOpenGLFunctions_ES2>
#include <Qt3DRenderer/private/qgraphicshelperes2_p.h>
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
    m_gl->functions()->glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTexUnits);
    qCDebug(Backend) << "context supports" << numTexUnits << "texture units";

    m_pinnedTextureUnits = QBitArray(numTexUnits);
    m_activeTextures.resize(numTexUnits);
    m_textureScopes.resize(numTexUnits);
}

void QGraphicsContext::beginDrawing(const QColor &clearColor)
{
    if (!m_gl || !m_surface) {
        qCWarning(Backend) << Q_FUNC_INFO << "no content or surface provided";
        return;
    }

    bool ok = m_gl->makeCurrent(m_surface);
    if (!ok) {
        qCWarning(Backend) << Q_FUNC_INFO << "make current failed";
    }

    GLint err = m_gl->functions()->glGetError();
    if (err != 0) {
        qCWarning(Backend) << Q_FUNC_INFO << "glGetError:" << err;
    }

    if (!m_initialized) {
        initialize();
    }

    m_gl->functions()->glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());

    if (m_activeShader)
        m_activeShader = NULL;
}

void QGraphicsContext::clearBackBuffer(QClearBuffer::BufferType buffers)
{
    if (buffers != QClearBuffer::None) {
        GLbitfield mask = 0;

        if (buffers & QClearBuffer::ColorBuffer)
            mask |= GL_COLOR_BUFFER_BIT;
        if (buffers & QClearBuffer::DepthBuffer)
            mask |= GL_DEPTH_BUFFER_BIT;
        if (buffers & QClearBuffer::StencilBuffer)
            mask |= GL_STENCIL_BUFFER_BIT;

        m_gl->functions()->glClear(mask);
    }
}

void QGraphicsContext::endDrawing()
{
    m_gl->doneCurrent();
    m_gl->swapBuffers(m_surface);
    m_stateSet = Q_NULLPTR;
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
    m_gl->functions()->glViewport(m_viewport.x() * m_surface->size().width(),
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
        QOpenGLShaderProgram *prog = shader->getOrCreateProgram(this);
        Q_ASSERT(prog);
        m_shaderHash[shader] = prog;
        shader->initializeUniforms(m_glHelper->programUniformsAndLocations(prog->programId()));
        shader->initializeAttributes(m_glHelper->programAttributesAndLocations(prog->programId()));
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

void QGraphicsContext::activateRenderTarget(RenderTarget *renderTarget, const AttachmentPack &attachments)
{
    GLuint fboId = 0;
    if (renderTarget != Q_NULLPTR) {
        // New RenderTarget
        if (!m_renderTargets.contains(renderTarget->renderTargetUuid())) {
            // The FBO is created and its attachments are set once
            if ((fboId = m_glHelper->createFrameBufferObject()) != 0) {
                // Insert FBO into hash
                m_renderTargets.insert(renderTarget->renderTargetUuid(), fboId);
                // Bind FBO
                m_glHelper->bindFrameBufferObject(fboId);
                bindFrameBufferAttachmentHelper(attachments);
            } else {
                qCritical() << "Failed to create FBO";
            }
        } else {
            fboId = m_renderTargets.value(renderTarget->renderTargetUuid());

            // We need to check if  one of the attachment was resized
            bool needsResize = false;
            Q_FOREACH (const Attachment &attachment, attachments.attachments()) {
                RenderTexture *rTex = m_renderer->textureManager()->lookupResource(attachment.m_textureUuid);
                if (rTex != Q_NULLPTR)
                    needsResize |= rTex->isTextureReset();
            }
            if (needsResize) {
                m_glHelper->bindFrameBufferObject(fboId);
                bindFrameBufferAttachmentHelper(attachments);
            }
        }
    }
    m_glHelper->bindFrameBufferObject(fboId);
}

void QGraphicsContext::bindFrameBufferAttachmentHelper(const AttachmentPack &attachments)
{
    // Set FBO attachments
    int drawBuffers[QRenderAttachment::ColorAttachment15 + 1];
    int i = 0;
    Q_FOREACH (const Attachment &attachment, attachments.attachments()) {
        RenderTexture *rTex = m_renderer->textureManager()->lookupResource(attachment.m_textureUuid);
        if (rTex != Q_NULLPTR) {
            QOpenGLTexture *glTex = rTex->getOrCreateGLTexture();
            m_glHelper->bindFrameBufferAttachment(glTex, attachment);
            if (attachment.m_type <= QRenderAttachment::ColorAttachment15)
                drawBuffers[i++] = attachment.m_type;
        }
    }
    if (m_glHelper->checkFrameBufferComplete()) {
        if (i > 1) {// We need MRT
            if (m_glHelper->supportsFeature(QGraphicsHelperInterface::MRT)) {
                // Set up MRT, glDrawBuffers...
                m_glHelper->drawBuffers(i, drawBuffers);
            }
            qDebug() << "FBO Complete";
        }
    } else {
        qWarning() << "FBO incomplete";
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
void QGraphicsContext::executeCommand(const RenderCommand *)
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

    int err = m_gl->functions()->glGetError();
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
    glFunctions = m_gl->versionFunctions<QOpenGLFunctions_ES2>();
    m_glHelper = new QGraphicsHelperES2();

    qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 2/ES2 Helper";
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
    Q_ASSERT_X(m_glHelper, "QGraphicsContext::resolveHighestOpenGLFunctions", "unable to create valid helper for available OpenGL version");
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
    m_contextInfo->setVendor(QString::fromUtf8(reinterpret_cast<const char *>(m_gl->functions()->glGetString(GL_VENDOR))));
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

void QGraphicsContext::setCurrentStateSet(RenderStateSet *ss)
{
    if (ss == m_stateSet)
        return;

    ss->apply(this);
    m_stateSet = ss;
}

RenderStateSet *QGraphicsContext::currentStateSet() const
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

void QGraphicsContext::setVerticesPerPatch(GLint verticesPerPatch)
{
    m_glHelper->setVerticesPerPatch(verticesPerPatch);
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

void QGraphicsContext::bindFragOutputs(GLuint shader, const QHash<QString, int> &outputs)
{
    if (m_glHelper->supportsFeature(QGraphicsHelperInterface::MRT))
        m_glHelper->bindFragDataLocation(shader, outputs);
}

void QGraphicsContext::bindUniform(const QVariant &v, const ShaderUniform &description)
{
    m_glHelper->bindUniform(v, description);
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
    m_activeShader->updateUniforms(this, uniforms);
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
                             QGraphicsContext::elementType(attr->type()),
                             attr->byteOffset(),
                             QGraphicsContext::tupleSizeFromType(attr->type()),
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

GLint QGraphicsContext::elementType(GLint type)
{
    switch (type) {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
        return GL_FLOAT;

#ifndef QT_OPENGL_ES_2 // Otherwise compile error as Qt defines GL_DOUBLE as GL_FLOAT when using ES2
    case GL_DOUBLE:
#ifdef GL_DOUBLE_VEC3 // For compiling on pre GL 4.1 systems
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
#endif
        return GL_DOUBLE;
#endif
    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return GL_INVALID_VALUE;
}

GLint QGraphicsContext::tupleSizeFromType(GLint type)
{
    switch (type) {
    case GL_FLOAT:
#ifndef QT_OPENGL_ES_2 // Otherwise compile error as Qt defines GL_DOUBLE as GL_FLOAT when using ES2
    case GL_DOUBLE:
#endif
    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_INT:
        break; // fall through

    case GL_FLOAT_VEC2:
#ifdef GL_DOUBLE_VEC2 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC2:
#endif
        return 2;

    case GL_FLOAT_VEC3:
#ifdef GL_DOUBLE_VEC3 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC3:
#endif
        return 3;

    case GL_FLOAT_VEC4:
#ifdef GL_DOUBLE_VEC4 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC4:
#endif
        return 4;

    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return 1;
}

GLuint QGraphicsContext::byteSizeFromType(GLint type)
{
    switch (type) {
    case GL_FLOAT:          return sizeof(float);
#ifndef QT_OPENGL_ES_2 // Otherwise compile error as Qt defines GL_DOUBLE as GL_FLOAT when using ES2
    case GL_DOUBLE:         return sizeof(double);
#endif
    case GL_UNSIGNED_BYTE:  return sizeof(unsigned char);
    case GL_UNSIGNED_INT:   return sizeof(GLuint);

    case GL_FLOAT_VEC2:     return sizeof(float) * 2;
    case GL_FLOAT_VEC3:     return sizeof(float) * 3;
    case GL_FLOAT_VEC4:     return sizeof(float) * 4;
#ifdef GL_DOUBLE_VEC3 // Required to compile on pre GL 4.1 systems
    case GL_DOUBLE_VEC2:    return sizeof(double) * 2;
    case GL_DOUBLE_VEC3:    return sizeof(double) * 3;
    case GL_DOUBLE_VEC4:    return sizeof(double) * 4;
#endif
    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return 0;
}

} // Render
} // Qt3D of namespace

QT_END_NAMESPACE
