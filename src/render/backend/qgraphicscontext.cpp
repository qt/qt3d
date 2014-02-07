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

#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <QSurface>
#include <QOpenGLTexture>

namespace Qt3D
{

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
    , m_surface(0)
    , m_funcs(0)
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
    m_surface = s;
}

void QGraphicsContext::initialize()
{
    m_initialized = true;

    Q_ASSERT(m_gl);

    GLint numTexUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTexUnits);
    qDebug() << "context supports" << numTexUnits << "texture units";

    m_pinnedTextureUnits = QBitArray(numTexUnits);
    m_activeTextures.resize(numTexUnits);
    m_textureScopes.resize(numTexUnits);
}

void QGraphicsContext::beginDrawing()
{
    if (!m_gl || !m_surface) {
        qWarning() << Q_FUNC_INFO << "no content or surface provided";
        return;
    }

    bool ok = m_gl->makeCurrent(m_surface);
    if (!ok) {
        qWarning() << Q_FUNC_INFO << "make current failed";
    }

    GLint err = glGetError();
    if (err != 0) {
        qWarning() << Q_FUNC_INFO << "glGetError:" << err;
    }

    if (!m_initialized) {
        initialize();
    }

    QVector4D cc = m_camera->clearColor();
    glClearColor(cc[0], cc[1], cc[2], cc[3]);
    glClear(m_camera->clearMask());

    QRectF vp(m_camera->viewport());
    // default to the entire surface
    if (vp.isEmpty())
        vp = QRectF(QPointF(0,0), m_surface->size());
    glViewport(vp.x(), vp.y(), vp.width(), vp.height());

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


QMatrix4x4 QGraphicsContext::projectionMatrix() const
{
    return m_camera->projection();
}

QMatrix4x4 QGraphicsContext::viewMatrix() const
{
    return m_camera->view();
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

    m_funcs = m_gl->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (m_funcs)
        m_funcs->initializeOpenGLFunctions();
}

void QGraphicsContext::activateShader(RenderShader *shader)
{
    if (shader == NULL) {
        m_activeShader = NULL;
        m_material = NULL;
        m_funcs->glUseProgram(0);
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

void QGraphicsContext::setModelMatrix(const QMatrix4x4& modelMat)
{
    const QVector<int>& locs(m_activeShader->standardUniformLocations());
    QOpenGLShaderProgram* prog = activeShader();

    GLint progId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &progId);
    if (progId != (int) prog->programId()) {
        qWarning() << "current program mismatch, very bad:" << progId << prog->programId();
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
            qWarning() << "GL error after setting matrix" << QString::number(err, 16) << i << loc;

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
        qWarning() << "GL error after activating texture" << QString::number(err, 16)
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

void QGraphicsContext::deactivateTexture(RenderTexture* tex)
{
    for (int u=0; u<m_activeTextures.size(); ++u) {
        if (m_activeTextures[u] == tex) {
            Q_ASSERT(m_pinnedTextureUnits[u]);
            m_pinnedTextureUnits[u] = false;
            return;
        }
    } // of units iteration

    qWarning() << Q_FUNC_INFO << "texture not active:" << tex;
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
        qWarning() << Q_FUNC_INFO << "NO free texture units!";
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
            qDebug() << "removing inactive texture" << t;
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
        qWarning() << "failed to resolve location for attribute:" << nm;
        return;
    }

    prog->enableAttributeArray(location);
    prog->setAttributeBuffer(location,
                          elementType(attr->type()),
                          attr->byteOffset(),
                          tupleSizeFromType(attr->type()),
                          attr->byteStride());

    if (attr->divisor() != 0) {
        // TODO - only if supported!
        m_funcs->glVertexAttribDivisor(location, attr->divisor());
    }

    buf.release();
}

void QGraphicsContext::specifyIndices(AttributePtr attr)
{
    if (attr->buffer()->type() != QOpenGLBuffer::IndexBuffer) {
        qWarning() << Q_FUNC_INFO << "provided buffer is not correct type";
        return;
    }

    QOpenGLBuffer buf = glBufferFor(attr->buffer());
    if (!buf.bind())
        qWarning() << Q_FUNC_INFO << "binding index buffer failed";

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

} // of namespace

