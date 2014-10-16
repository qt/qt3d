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

#include "rendertexture_p.h"

#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <qtexture.h>
#include <texturedata.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qaspectmanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderTexture::RenderTexture()
    : QBackendNode()
    , m_gl(Q_NULLPTR)
    , m_width(1)
    , m_height(1)
    , m_depth(1)
    , m_generateMipMaps(false)
    , m_target(QTexture::Target2D)
    , m_format(QTexture::RGBA8U)
    , m_magnificationFilter(QTexture::Nearest)
    , m_minificationFilter(QTexture::Nearest)
    , m_wrapMode(QTexture::ClampToEdge)
    , m_maximumAnisotropy(1.0f)
    , m_isDirty(false)
    , m_filtersAndWrapUpdated(false)
    , m_lock(new QMutex())
{
    // We need backend -> frontend notifications to update the status of the texture
}

RenderTexture::~RenderTexture()
{
    cleanup();
}

void RenderTexture::cleanup()
{
    if (m_lock != Q_NULLPTR)
        delete m_lock;
    m_lock = Q_NULLPTR;
}

void RenderTexture::updateFromPeer(QNode *peer)
{
    QTexture *texture = static_cast<QTexture *>(peer);

    QMutexLocker lock(m_lock);
    if (texture != Q_NULLPTR) {
        m_isDirty = true;
        m_width = texture->width();
        m_height = texture->height();
        m_depth = texture->depth();
        m_generateMipMaps = texture->generateMipMaps();
        m_target = texture->target();
        m_format = texture->format();
        m_magnificationFilter = texture->magnificationFilter();
        m_minificationFilter = texture->minificationFilter();
        m_wrapMode = texture->wrapMode();
        m_maximumAnisotropy = texture->maximumAnisotropy();
        // See where it is best to handle source and loading
        Q_FOREACH (TexImageDataPtr imgData, texture->imageData())
            m_imageData.append(imgData);
    }
}

QOpenGLTexture *RenderTexture::getOrCreateGLTexture()
{
    // m_gl HAS to be destroyed in the OpenGL Thread
    // Will be recreated with updated values the next time
    // buildGLTexture is called


    // getOrCreateGLTexture is called by the OpenGL Render Thread
    // sceneChangeEvent is called by the QAspectThread
    // only the OpenGL Render Thread can set isDirty to false
    // only a sceneChangeEvent in QAspectThread can set isDirty to true
    // We need the lock to make sure there are no races when the OpenGL
    // thread tries to read isDirty or one of the texture properties in case
    // we are receiving a sceneChangeEvent that modifies isDirty or one of the properties
    // at the same time

    QMutexLocker lock(m_lock);
    if (m_isDirty) {
        delete m_gl;
        m_gl = Q_NULLPTR;
        m_isDirty = false;
    }

    if (m_gl != Q_NULLPTR) {
        if (m_filtersAndWrapUpdated) {
            updateWrapAndFilters();
            m_filtersAndWrapUpdated = false;
        }
        return m_gl;
    }

    m_gl = buildGLTexture();

    m_gl->allocateStorage();
    if (!m_gl->isStorageAllocated()) {
        qWarning() << Q_FUNC_INFO << "texture storage allocation failed";
        return Q_NULLPTR;
    }

    foreach (TexImageDataPtr imgData, m_imageData) {
        setToGLTexture(imgData);
    } // of image data in texture iteration

    // Filters and WrapMode are set
    updateWrapAndFilters();
    m_filtersAndWrapUpdated = false;

    // Ideally we might want to abstract that and use the QGraphicsContext as a wrapper
    // around that.
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext()) {
        int err = ctx->functions()->glGetError();
        if (err)
            qWarning() << Q_FUNC_INFO << "GL error after generating mip-maps" << QString::number(err, 16);
    }

    return m_gl;
}

QOpenGLTexture *RenderTexture::buildGLTexture()
{
    QOpenGLTexture* glTex = new QOpenGLTexture(static_cast<QOpenGLTexture::Target>(m_target));
    glTex->setFormat(static_cast<QOpenGLTexture::TextureFormat>(m_format));
    glTex->setSize(m_width, m_height, m_depth);

    if (m_generateMipMaps)
        glTex->setMipLevels(glTex->maximumMipLevels());

    if (!glTex->create()) {
        qWarning() << Q_FUNC_INFO << "creating QOpenGLTexture failed";
        return NULL;
    }

    // FIXME : make this conditional on Qt version
    // work-around issue in QOpenGLTexture DSA emulaation which rasies
    // an Invalid Enum error
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext())
        ctx->functions()->glGetError();

    return glTex;
}

void RenderTexture::setToGLTexture(TexImageDataPtr imgData)
{
    Q_ASSERT(m_gl && m_gl->isCreated() && m_gl->isStorageAllocated());
    // ensure we don't accidently cause a detach / copy of the raw bytes
    const QByteArray& bytes(imgData->data());
    if (imgData->isCompressed()) {
        m_gl->setCompressedData(imgData->mipMapLevel(),
                                imgData->layer(),
                                imgData->cubeFace(),
                                bytes.size(),
                                bytes.constData());
    } else {
        QOpenGLPixelTransferOptions uploadOptions;
        uploadOptions.setAlignment(1);
        m_gl->setData(imgData->mipMapLevel(),
                      imgData->layer(),
                      imgData->cubeFace(),
                      imgData->pixelFormat(),
                      imgData->pixelType(),
                      bytes.constData(),
                      &uploadOptions);
    }

    // FIXME : make this conditional on Qt version
    // work-around issue in QOpenGLTexture DSA emulaation which rasies
    // an Invalid Enum error
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext())
        ctx->functions()->glGetError();
}

void RenderTexture::updateWrapAndFilters()
{
    m_gl->setWrapMode(static_cast<QOpenGLTexture::WrapMode>(m_wrapMode));
    m_gl->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_minificationFilter),
                           static_cast<QOpenGLTexture::Filter>(m_magnificationFilter));
    m_gl->setMaximumAnisotropy(m_maximumAnisotropy);
}


GLint RenderTexture::textureId()
{
    return getOrCreateGLTexture()->textureId();
}

bool RenderTexture::isTextureReset() const
{
    QMutexLocker lock(m_lock);
    return m_isDirty;
}

void RenderTexture::sceneChangeEvent(const QSceneChangePtr &e)
{
    // The QOpenGLTexture has to be manipulated from the RenderThread only
    if (e->type() == NodeUpdated) {

        // We lock here so that we're sure the texture cannot be rebuilt while we are
        // modifying one of its properties
        QMutexLocker lock(m_lock);

        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("width")) {
            int oldWidth = m_width;
            m_width = propertyChange->value().toInt();
            m_isDirty = (oldWidth != m_width);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("height")) {
            int oldHeight = m_height;
            m_height = propertyChange->value().toInt();
            m_isDirty = (oldHeight != m_height);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("depth")) {
            int oldDepth = m_depth;
            m_depth = propertyChange->value().toInt();
            m_isDirty = (oldDepth != m_depth);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("mipmaps")) {
            bool oldMipMaps = m_generateMipMaps;
            m_generateMipMaps = propertyChange->value().toBool();
            m_isDirty = (oldMipMaps != m_generateMipMaps);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("minificationFilter")) {
            QTexture::Filter oldMinFilter = m_minificationFilter;
            m_minificationFilter = static_cast<QTexture::Filter>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated = (oldMinFilter != m_minificationFilter);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("magnificationFilter")) {
            QTexture::Filter oldMagFilter = m_magnificationFilter;
            m_magnificationFilter = static_cast<QTexture::Filter>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated = (oldMagFilter != m_magnificationFilter);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapMode")) {
            QTexture::WrapMode oldWrapMode = m_wrapMode;
            m_wrapMode = static_cast<QTexture::WrapMode>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated = (oldWrapMode != m_wrapMode);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("format")) {
            QTexture::TextureFormat oldFormat = m_format;
            m_format = static_cast<QTexture::TextureFormat>(propertyChange->value().toInt());
            m_isDirty = (oldFormat != m_format);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("target")) {
            QTexture::Target oldTarget = m_target;
            m_target = static_cast<QTexture::Target>(propertyChange->value().toInt());
            m_isDirty = (oldTarget != m_target);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("maximumAnisotropy")) {
            float oldMaximumAnisotropy = m_maximumAnisotropy;
            m_maximumAnisotropy = propertyChange->value().toFloat();
            m_filtersAndWrapUpdated = !qFuzzyCompare(oldMaximumAnisotropy, m_maximumAnisotropy);
        }
    }
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
