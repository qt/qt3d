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

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderTexture::RenderTexture() :
    m_peer(Q_NULLPTR),
    m_gl(Q_NULLPTR)
{
}

void RenderTexture::setPeer(QTexture *peer)
{
    m_peer = peer;
}

void RenderTexture::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

QOpenGLTexture *RenderTexture::getOrCreateGLTexture()
{
    if (m_gl)
        return m_gl;

    m_gl = buildGLTexture();

    m_gl->allocateStorage();
    if (!m_gl->isStorageAllocated()) {
        qWarning() << Q_FUNC_INFO << "texture storage allocation failed";
        return NULL;
    }

    foreach (TexImageDataPtr imgData, m_peer->imageData()) {
        setToGLTexture(imgData);
    } // of image data in texture iteration

    if (m_peer->generateMipMaps()) {
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
    QOpenGLTexture* glTex = new QOpenGLTexture(static_cast<QOpenGLTexture::Target>(m_peer->target()));
    glTex->setFormat(static_cast<QOpenGLTexture::TextureFormat>(m_peer->format()));
    glTex->setSize(m_peer->width(),
                   m_peer->height(),
                   m_peer->depth());

    if (!glTex->create()) {
        qWarning() << Q_FUNC_INFO << "creating QOpenGLTexture failed";
        return NULL;
    }

    glTex->setWrapMode(static_cast<QOpenGLTexture::WrapMode>(m_peer->wrapMode()));
    glTex->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_peer->minificationFilter()),
                            static_cast<QOpenGLTexture::Filter>(m_peer->magnificationFilter()));

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

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
