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

#include "textureimage_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

TextureImage::TextureImage()
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

void TextureImage::cleanup()
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

void TextureImage::updateFromPeer(Qt3DCore::QNode *peer)
{
    QAbstractTextureImage *textureImage = static_cast<QAbstractTextureImage *>(peer);
    m_layer = textureImage->layer();
    m_mipmapLevel = textureImage->mipmapLevel();
    m_face = textureImage->cubeMapFace();
    m_functor = textureImage->dataFunctor();
    // Notify the Texture that we are one of its TextureImage
    if (!peer->parentNode()) {
        qWarning() << "Not QAbstractTextureProvider parent found";
    } else {
        m_textureProviderId = peer->parentNode()->id();
        m_textureProvider = m_textureManager->lookupHandle(m_textureProviderId);
        Texture *txt = m_textureManager->data(m_textureProvider);
        // Notify the Texture that it has a new TextureImage and needs an update
        txt->addTextureImageData(m_textureImageManager->lookupHandle(peerUuid()));
        if (txt != Q_NULLPTR)
            txt->addToPendingTextureJobs();
    }
}

void TextureImage::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
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
    if (m_dirty) {// Notify the Texture that we were updated and request it to schedule an update job
        Texture *txt = m_textureManager->data(m_textureProvider);
        if (txt != Q_NULLPTR)
            txt->addToPendingTextureJobs();
    }
}

void TextureImage::setTextureManager(TextureManager *manager)
{
    m_textureManager = manager;
}

void TextureImage::setTextureImageManager(TextureImageManager *manager)
{
    m_textureImageManager = manager;
}

void TextureImage::setTextureDataManager(TextureDataManager *manager)
{
    m_textureDataManager = manager;
}

void TextureImage::unsetDirty()
{
    m_dirty = false;
}

// Called by LoadDataTextureJob when the texture data has been successfully load
void TextureImage::setTextureDataHandle(HTextureData handle)
{
    m_textureDataHandle = handle;
    updateDNA();
}

void TextureImage::updateDNA()
{
    m_dna = ::qHash(m_layer
                    + (m_mipmapLevel << 4)
                    + (static_cast<int>(m_face) << 8)
                    + (m_textureDataHandle.handle() << 12));
}

TextureImageFunctor::TextureImageFunctor(TextureManager *textureManager,
                                                     TextureImageManager *textureImageManager,
                                                     TextureDataManager *textureDataManager)
    : m_textureManager(textureManager)
    , m_textureImageManager(textureImageManager)
    , m_textureDataManager(textureDataManager)
{
}

Qt3DCore::QBackendNode *TextureImageFunctor::create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const
{
    TextureImage *backend = m_textureImageManager->getOrCreateResource(frontend->id());
    backend->setFactory(factory);
    backend->setTextureManager(m_textureManager);
    backend->setTextureImageManager(m_textureImageManager);
    backend->setTextureDataManager(m_textureDataManager);
    backend->setPeer(frontend);
    return backend;
}

Qt3DCore::QBackendNode *TextureImageFunctor::get(const Qt3DCore::QNodeId &id) const
{
    return m_textureImageManager->lookupResource(id);
}

void TextureImageFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    m_textureImageManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
