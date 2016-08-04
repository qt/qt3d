/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_APITEXTUREMANAGER_P_H
#define QT3DRENDER_RENDER_APITEXTUREMANAGER_P_H

#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/private/textureimage_p.h>
#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/qtexturegenerator.h>
#include <Qt3DRender/qtextureimagedatagenerator.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

// Manages instances of APITexture. This includes sharing between multiple
// Texture nodes, updating APITextures and deleting abandoned instances.
template <class APITexture, class APITextureImage>
class APITextureManager
{
public:

    explicit APITextureManager(TextureManager *textureNodeManager,
                               TextureImageManager *textureImageManager,
                               TextureDataManager *textureDataManager,
                               TextureImageDataManager *textureImageDataManager)
        : m_mutex(QMutex::Recursive)
        , m_textureNodeManager(textureNodeManager)
        , m_textureImageManager(textureImageManager)
        , m_textureDataManager(textureDataManager)
        , m_textureImageDataManager(textureImageDataManager)
    {
    }

    ~APITextureManager()
    {
    }

    // Returns a APITexture that matches the given QTexture node. Will make sure
    // that texture data generator jobs are launched, if necessary. The APITexture
    // may be shared between multiple texture backend nodes
    APITexture *getOrCreateShared(const Texture *node)
    {
        Q_ASSERT(node);
        QMutexLocker lock(&m_mutex);

        APITexture *shared = tryFindShared(node);
        if (shared)
            return shared;

        // get TextureNode handle
        const HTexture handle = m_textureNodeManager->lookupHandle(node->peerId());
        Q_ASSERT(!handle.isNull());

        // no matching shared texture was found; create a new one:
        APITexture *newTex = createTexture(*node, false);
        m_sharedTextures[newTex] << handle;

        return newTex;
    }

    // If there is already a shared texture with the properties of the given
    // texture node, return this instance, else NULL.
    APITexture *tryFindShared(const Texture *node)
    {
        Q_ASSERT(node);
        QMutexLocker lock(&m_mutex);

        // get TextureNode handle
        const HTexture handle = m_textureNodeManager->lookupHandle(node->peerId());
        Q_ASSERT(!handle.isNull());

        // search for existing texture
        const auto end = m_sharedTextures.end();
        for (auto it = m_sharedTextures.begin(); it != end; ++it) {
            if (isSameTexture(*it.key(), *node)) {
                QVector<HTexture> &values = it.value();
                if (values.contains(handle))
                    values.push_back(handle);
                return it.key();
            }
        }
        return nullptr;
    }

    // Returns a APITexture that matches the given QTexture node. Will make sure
    // that texture data generator jobs are launched, if necessary.
    APITexture *createUnique(const Texture *node)
    {
        QMutexLocker lock(&m_mutex);

        Q_ASSERT(node);
        APITexture *newTex = createTexture(*node, true);
        m_uniqueTextures << newTex;
        return newTex;
    }

    // De-associate the given APITexture from the backend node. If the texture
    // is no longer referenced by any other node, it will be deleted.
    void abandon(APITexture *tex, const Texture *node)
    {
        QMutexLocker lock(&m_mutex);

        if (tex->isUnique()) {
            m_uniqueTextures.removeAll(tex);
            m_abandonedTextures << tex;
        } else {
            // search for texture
            auto it = m_sharedTextures.find(tex);
            if (it != m_sharedTextures.end()) {
                // remove TextureNode reference
                const HTexture handle = m_textureNodeManager->lookupHandle(node->peerId());
                Q_ASSERT(!handle.isNull());
                it.value().removeAll(handle);

                // if that was the last reference, mark the texture to be deleted
                if (it.value().empty()) {
                    m_abandonedTextures << tex;
                    m_sharedTextures.erase(it);
                }
            } else {
                qWarning() << "[Qt3DRender::TextureManager] abandon: could not find Texture";
            }
        }
    }

    // Change the properties of the given texture, if it is a non-shared texture
    // Returns true, if it was changed successfully, false otherwise
    bool setProperties(APITexture *tex, const TextureProperties &props)
    {
        QMutexLocker lock(&m_mutex);
        Q_ASSERT(tex);

        if (isShared(tex))
            return false;

        tex->setProperties(props);
        m_updatedTextures << tex;

        return true;
    }

    // Change the parameters of the given texture, if it is a non-shared texture
    // Returns true, if it was changed successfully, false otherwise
    bool setParameters(APITexture *tex, const TextureParameters &params)
    {
        QMutexLocker lock(&m_mutex);
        Q_ASSERT(tex);

        if (isShared(tex))
            return false;

        tex->setParameters(params);
        m_updatedTextures << tex;

        return true;
    }

    // Change the texture images of the given texture, if it is a non-shared texture
    // Return true, if it was changed successfully, false otherwise
    bool setImages(APITexture *tex, const QVector<HTextureImage> &images)
    {
        QMutexLocker lock(&m_mutex);
        Q_ASSERT(tex);

        if (isShared(tex))
            return false;

        // create Image structs
        QVector<APITextureImage> texImgs = texImgsFromNodes(images);
        if (texImgs.size() != images.size())
            return false;

        tex->setImages(texImgs);
        m_updatedTextures << tex;

        return true;
    }

    // Retrieves abandoned textures. This should be regularly called from the OpenGL thread
    // to make sure needed GL resources are de-allocated.
    QVector<APITexture*> takeAbandonedTextures()
    {
        QMutexLocker lock(&m_mutex);
        const QVector<APITexture*> ret = m_abandonedTextures;
        m_abandonedTextures.clear();
        return ret;
    }

    // Retrieves textures that have been modified
    QVector<APITexture*> takeUpdatedTextures()
    {
        QMutexLocker lock(&m_mutex);
        const QVector<APITexture*> ret = m_updatedTextures;
        m_updatedTextures.clear();
        return ret;
    }

    // Returns whether the given APITexture is shared between multiple TextureNodes
    bool isShared(APITexture *impl)
    {
        QMutexLocker lock(&m_mutex);
        Q_ASSERT(impl);

        if (impl->isUnique())
            return false;

        auto it = m_sharedTextures.find(impl);
        if (it == m_sharedTextures.cend())
            return false;

        return it.value().size() > 1;
    }

private:

    // Check if the given APITexture matches the TextureNode
    bool isSameTexture(const APITexture &tex, const Texture &texNode)
    {
        // make sure there either are no texture generators, or the two are the same
        if (tex.textureGenerator().isNull() != texNode.dataGenerator().isNull())
            return false;
        if (!tex.textureGenerator().isNull() && !(*tex.textureGenerator() == *texNode.dataGenerator()))
            return false;

        // make sure the image generators are the same
        const QVector<APITextureImage> texImgGens = tex.images();
        const QVector<HTextureImage> texImgs = texNode.textureImages();
        if (texImgGens.size() != texImgs.size())
            return false;
        for (int i = 0; i < texImgGens.size(); ++i) {
            const TextureImage *img = m_textureImageManager->data(texImgs[i]);
            Q_ASSERT(img != nullptr);
            if (!(*img->dataGenerator() == *texImgGens[i].generator)
                    || img->layer() != texImgGens[i].layer
                    || img->face() != texImgGens[i].face
                    || img->mipLevel() != texImgGens[i].mipLevel)
                return false;
        }

        // if the texture has a texture generator, this generator will mostly determine
        // the properties of the texture.
        if (!tex.textureGenerator().isNull()) {
            return (tex.properties().generateMipMaps == texNode.properties().generateMipMaps
                    && tex.parameters() == texNode.parameters());
        }

        // if it doesn't have a texture generator, but texture image generators,
        // few more properties will influence the texture type
        if (!texImgGens.empty()) {
            return (tex.properties().target == texNode.properties().target
                    && tex.properties().format == texNode.properties().format
                    && tex.properties().generateMipMaps == texNode.properties().generateMipMaps
                    && tex.parameters() == texNode.parameters());
        }

        // texture without images
        return tex.properties() == texNode.properties()
                && tex.parameters() == tex.parameters();
    }

    // Create APITexture from given TextureNode. Also make sure the generators
    // will be executed by jobs soon.
    APITexture *createTexture(const Texture &node, bool unique)
    {
        QMutexLocker lock(&m_mutex);

        // get handle of given texture node
        const HTexture handle = m_textureNodeManager->lookupHandle(node.peerId());
        Q_ASSERT(!handle.isNull());

        // create Image structs
        const QVector<APITextureImage> texImgs = texImgsFromNodes(node.textureImages());
        if (texImgs.empty() && !node.textureImages().empty())
            return nullptr;

        // no matching shared texture was found, create a new one
        APITexture *newTex = new APITexture(m_textureDataManager, m_textureImageDataManager, node.dataGenerator(), unique);
        newTex->setProperties(node.properties());
        newTex->setParameters(node.parameters());
        newTex->setImages(texImgs);

        m_updatedTextures << newTex;

        return newTex;
    }

    QVector<APITextureImage> texImgsFromNodes(const QVector<HTextureImage> &images) const
    {
        QVector<APITextureImage> ret;
        ret.resize(images.size());

        for (int i = 0; i < images.size(); ++i) {
            const TextureImage *img = m_textureImageManager->data(images[i]);
            if (!img) {
                qWarning() << "[Qt3DRender::TextureManager] invalid TextureImage handle";
                return QVector<APITextureImage>();
            }

            ret[i].generator = img->dataGenerator();
            ret[i].face = img->face();
            ret[i].layer = img->layer();
            ret[i].mipLevel = img->mipLevel();
        }

        return ret;
    }

    QMutex m_mutex;

    TextureManager *m_textureNodeManager;
    TextureImageManager *m_textureImageManager;
    TextureDataManager *m_textureDataManager;
    TextureImageDataManager *m_textureImageDataManager;

    /* each non-unique texture is associated with a number of Texture nodes referencing it */
    QHash<APITexture*,QVector<HTexture>> m_sharedTextures;

    QVector<APITexture*> m_uniqueTextures;

    QVector<APITexture*> m_abandonedTextures;
    QVector<APITexture*> m_updatedTextures;
};


} // Render

} // Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_APITEXTUREMANAGER_P_H
