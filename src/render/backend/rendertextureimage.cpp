/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rendertextureimage_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/texturedatamanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderTextureImage::RenderTextureImage()
    : QBackendNode()
    , m_layer(0)
    , m_mipmapLevel(0)
    , m_face(QAbstractTextureProvider::CubeMapPositiveX)
    , m_dirty(true)
    , m_textureManager(Q_NULLPTR)
    , m_textureImageManager(Q_NULLPTR)
    , m_textureDataManager(Q_NULLPTR)
    , m_dna(0)
{
}

void RenderTextureImage::cleanup()
{
    m_layer = 0;
    m_mipmapLevel = 0;
    m_dirty = true;
    m_face = QAbstractTextureProvider::CubeMapPositiveX;
    m_functor.reset();
    m_textureManager = Q_NULLPTR;
    m_textureImageManager = Q_NULLPTR;
    m_textureDataManager = Q_NULLPTR;
    m_referencedTextures.clear();
    m_dna = 0;
}

void RenderTextureImage::updateFromPeer(QNode *peer)
{
    QAbstractTextureImage *textureImage = static_cast<QAbstractTextureImage *>(peer);
    m_layer = textureImage->layer();
    m_mipmapLevel = textureImage->mipmapLevel();
    m_face = textureImage->cubeMapFace();
    m_functor = textureImage->dataFunctor();
    // Notify the RenderTexture that we are one of its TextureImage
    if (!peer->parentNode()) {
        qWarning() << "Not QAbstractTextureProvider parent found";
    } else {
        m_textureProviderId = peer->parentNode()->id();
        m_textureProvider = m_textureManager->lookupHandle(m_textureProviderId);
        RenderTexture *txt = m_textureManager->data(m_textureProvider);
        // Notify the RenderTexture that it has a new RenderTextureImage and needs an update
        txt->addTextureImageData(m_textureImageManager->lookupHandle(peerUuid()));
        if (txt != Q_NULLPTR)
            txt->addToPendingTextureJobs();
    }
}

void RenderTextureImage::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    if (e->type() == NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("layer")) {
            m_layer = propertyChange->value().toInt();
            m_dirty = true;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("mipmapLevel")) {
            m_mipmapLevel = propertyChange->value().toInt();
            m_dirty = true;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("cubeMapFace")) {
            m_face = static_cast<QAbstractTextureProvider::CubeMapFace>(propertyChange->value().toInt());
            m_dirty = true;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("dataFunctor")) {
            m_functor = propertyChange->value().value<QTextureDataFunctorPtr>();
            m_dirty = true;
        }
    }
    if (m_dirty) {// Notify the RenderTexture that we were updated and request it to schedule an update job
        RenderTexture *txt = m_textureManager->data(m_textureProvider);
        if (txt != Q_NULLPTR)
            txt->addToPendingTextureJobs();
    }
}

void RenderTextureImage::setTextureManager(TextureManager *manager)
{
    m_textureManager = manager;
}

void RenderTextureImage::setTextureImageManager(TextureImageManager *manager)
{
    m_textureImageManager = manager;
}

void RenderTextureImage::setTextureDataManager(TextureDataManager *manager)
{
    m_textureDataManager = manager;
}

void RenderTextureImage::unsetDirty()
{
    m_dirty = false;
}

// Called by LoadDataTextureJob when the texture data has been successfully load
void RenderTextureImage::setTextureDataHandle(HTextureData handle)
{
    m_textureDataHandle = handle;
    updateDNA();
}

void RenderTextureImage::updateDNA()
{
    m_dna = ::qHash(m_layer + m_mipmapLevel + static_cast<int>(m_face) + m_textureDataHandle);
}

RenderTextureImageFunctor::RenderTextureImageFunctor(TextureManager *textureManager,
                                                     TextureImageManager *textureImageManager,
                                                     TextureDataManager *textureDataManager)
    : m_textureManager(textureManager)
    , m_textureImageManager(textureImageManager)
    , m_textureDataManager(textureDataManager)
{
}

QBackendNode *RenderTextureImageFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    RenderTextureImage *backend = m_textureImageManager->getOrCreateResource(frontend->id());
    backend->setFactory(factory);
    backend->setTextureManager(m_textureManager);
    backend->setTextureImageManager(m_textureImageManager);
    backend->setTextureDataManager(m_textureDataManager);
    backend->setPeer(frontend);
    return backend;
}

QBackendNode *RenderTextureImageFunctor::get(const QNodeId &id) const
{
    return m_textureImageManager->lookupResource(id);
}

void RenderTextureImageFunctor::destroy(const QNodeId &id) const
{
    m_textureImageManager->releaseResource(id);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
