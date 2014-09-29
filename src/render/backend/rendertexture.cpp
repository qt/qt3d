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
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <qtexture.h>
#include <texturedata.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qaspectmanager.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderTexture::RenderTexture()
    : m_gl(Q_NULLPTR)
    , m_renderer(Q_NULLPTR)
    , m_width(1)
    , m_height(1)
    , m_depth(1)
    , m_generateMipMaps(false)
    , m_target(QTexture::Target2D)
    , m_format(QTexture::RGBA8U)
    , m_magnificationFilter(QTexture::Nearest)
    , m_minificationFilter(QTexture::Nearest)
    , m_wrapMode(QTexture::ClampToEdge)
{
    // We need backend -> frontend notifications to update the status of the texture
}

void RenderTexture::setPeer(QTexture *peer)
{
    QUuid peerUuid;
    if (peer != Q_NULLPTR)
        peerUuid = peer->uuid();
    if (peerUuid != m_textureUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_textureUuid.isNull()) {
            arbiter->unregisterObserver(this, m_textureUuid);
            m_gl->destroy();
            delete m_gl;
            m_gl = Q_NULLPTR;
        }
        m_textureUuid = peerUuid;
        if (!m_textureUuid.isNull()) {
            arbiter->registerObserver(this, m_textureUuid, NodeUpdated);
            m_width = peer->width();
            m_height = peer->height();
            m_depth = peer->depth();
            m_generateMipMaps = peer->generateMipMaps();
            m_target = peer->target();
            m_format = peer->format();
            m_magnificationFilter = peer->magnificationFilter();
            m_minificationFilter = peer->minificationFilter();
            m_wrapMode = peer->wrapMode();
            // See where it is best to handle source and loading
            Q_FOREACH (TexImageDataPtr imgData, peer->imageData())
                m_imageData.append(imgData);
        }
    }
}

void RenderTexture::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

QOpenGLTexture *RenderTexture::getOrCreateGLTexture()
{
    // We need to handle texture resize
    if (m_gl)
        return m_gl;

    m_gl = buildGLTexture();

    m_gl->allocateStorage();
    if (!m_gl->isStorageAllocated()) {
        qWarning() << Q_FUNC_INFO << "texture storage allocation failed";
        return NULL;
    }

    foreach (TexImageDataPtr imgData, m_imageData) {
        setToGLTexture(imgData);
    } // of image data in texture iteration

    if (m_generateMipMaps) {
        m_gl->generateMipMaps();
    }

    int err = glGetError();
    if (err)
        qWarning() << Q_FUNC_INFO <<
                      "GL error after generating mip-maps" << QString::number(err, 16);

    return m_gl;
}

QOpenGLTexture *RenderTexture::buildGLTexture()
{
    QOpenGLTexture* glTex = new QOpenGLTexture(static_cast<QOpenGLTexture::Target>(m_target));
    glTex->setFormat(static_cast<QOpenGLTexture::TextureFormat>(m_format));
    glTex->setSize(m_width, m_height, m_depth);
    glTex->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_minificationFilter),
                            static_cast<QOpenGLTexture::Filter>(m_magnificationFilter));


    if (!glTex->create()) {
        qWarning() << Q_FUNC_INFO << "creating QOpenGLTexture failed";
        return NULL;
    }

    glTex->setWrapMode(static_cast<QOpenGLTexture::WrapMode>(m_wrapMode));
    glTex->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_minificationFilter),
                            static_cast<QOpenGLTexture::Filter>(m_magnificationFilter));

    // FIXME : make this conditional on Qt version
    // work-around issue in QOpenGLTexture DSA emulaation which rasies
    // an Invalid Enum error
    glGetError();

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
    glGetError();
}


GLint RenderTexture::textureId()
{
    return getOrCreateGLTexture()->textureId();
}

void RenderTexture::sceneChangeEvent(const QSceneChangePtr &e)
{
    bool resetTexture = false;
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("width")) {
            int oldWidth = m_width;
            m_width = propertyChange->value().toInt();
            resetTexture = (oldWidth != m_width);
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("height")) {
            int oldHeight = m_height;
            m_height = propertyChange->value().toInt();
            resetTexture = (oldHeight != m_height);
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("depth")) {
            int oldDepth = m_depth;
            m_depth = propertyChange->value().toInt();
            resetTexture = (oldDepth != m_depth);
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("mipmaps")) {
            bool oldMipMaps = m_generateMipMaps;
            m_generateMipMaps = propertyChange->value().toBool();
            resetTexture = (oldMipMaps != m_generateMipMaps);
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("minificationFilter")) {
            m_minificationFilter = static_cast<QTexture::Filter>(propertyChange->value().toInt());
            if (m_gl)
                m_gl->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_minificationFilter),
                                       static_cast<QOpenGLTexture::Filter>(m_magnificationFilter));
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("magnificationFilter")) {
            m_magnificationFilter = static_cast<QTexture::Filter>(propertyChange->value().toInt());
            if (m_gl)
                m_gl->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_minificationFilter),
                                       static_cast<QOpenGLTexture::Filter>(m_magnificationFilter));
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("wrapMode")) {
            m_wrapMode = static_cast<QTexture::WrapMode>(propertyChange->value().toInt());
            if (m_gl)
                m_gl->setWrapMode(static_cast<QOpenGLTexture::WrapMode>(m_wrapMode));
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("format")) {
            QTexture::TextureFormat oldFormat = m_format;
            m_format = static_cast<QTexture::TextureFormat>(propertyChange->value().toInt());
            resetTexture = (oldFormat != m_format);
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("target")) {
            QTexture::Target oldTarget = m_target;
            m_target = static_cast<QTexture::Target>(propertyChange->value().toInt());
            resetTexture = (oldTarget != m_target);
        }
    }

    // Will be recreated with updated values the next time
    // buildGLTexture is called
    if (resetTexture) {
        delete m_gl;
        m_gl = Q_NULLPTR;
    }

}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
