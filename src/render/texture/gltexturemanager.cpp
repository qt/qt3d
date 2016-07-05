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

#include "gltexturemanager_p.h"

#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/gltexture_p.h>
#include <Qt3DRender/private/textureimage_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {


/**
 *   \class GLTextureManager
 *   \internal
 *   \brief Manages instances of GLTexture.
 *
 *   This includes sharing between multiple Texture nodes, updating GLTextures
 *   and deleting abandoned instances.
 */

GLTextureManager::GLTextureManager(TextureManager *textureNodeManager,
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

GLTextureManager::~GLTextureManager()
{
}

/**
 * \internal
 * Check if the given GLTexture matches the TextureNode
 */
bool GLTextureManager::isSameTexture(const GLTexture &tex, const Texture &texNode)
{
    // make sure there either are no texture generators, or the two are the same
    if (tex.textureGenerator().isNull() != texNode.dataGenerator().isNull())
        return false;
    if (!tex.textureGenerator().isNull() && !(*tex.textureGenerator() == *texNode.dataGenerator()))
        return false;

    // make sure the image generators are the same
    const QVector<GLTexture::Image> texImgGens = tex.images();
    const QVector<HTextureImage> texImgs = texNode.textureImages();
    if (texImgGens.size() != texImgs.size())
        return false;
    for (int i = 0; i < texImgGens.size(); i++) {
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

/**
 * \internal
 * Create GLTexture from given TextureNode
 */
GLTexture *GLTextureManager::createTexture(const Texture &node, bool unique)
{
    QMutexLocker lock(&m_mutex);

    // get handle of given texture node
    const HTexture handle = m_textureNodeManager->lookupHandle(node.peerId());
    Q_ASSERT(!handle.isNull());

    // create Image structs
    const QVector<GLTexture::Image> texImgs = texImgsFromNodes(node.textureImages());
    if (texImgs.empty() && !node.textureImages().empty())
        return Q_NULLPTR;

    // no matching shared texture was found, create a new one
    GLTexture *newTex = new GLTexture(m_textureDataManager, m_textureImageDataManager, node.dataGenerator(), unique);
    newTex->setProperties(node.properties());
    newTex->setParameters(node.parameters());
    newTex->setImages(texImgs);

    m_updatedTextures << newTex;

    return newTex;
}


/**
 * \internal
 * Returns a GLTexture that matches the given QTexture node.
 * Will make sure that texture data generator jobs are launched,
 * if necessary. The GLTexture may be shared between multiple
 * texture backend nodes
 */
GLTexture *GLTextureManager::getOrCreateShared(const Texture *node)
{
    QMutexLocker lock(&m_mutex);

    Q_ASSERT(node);

    GLTexture *shared = tryFindShared(node);
    if (shared)
        return shared;

    // get TextureNode handle
    const HTexture handle = m_textureNodeManager->lookupHandle(node->peerId());
    Q_ASSERT(!handle.isNull());

    // no matching shared texture was found; create a new one:
    GLTexture *newTex = createTexture(*node, false);
    m_sharedTextures[newTex] << handle;

    return newTex;
}

/**
 * \internal
 * If there is already a shared texture with the properties of
 * the given texture node, return this instance, else NULL.
 */
GLTexture *GLTextureManager::tryFindShared(const Texture *node)
{
    QMutexLocker lock(&m_mutex);

    Q_ASSERT(node);

    // get TextureNode handle
    const HTexture handle = m_textureNodeManager->lookupHandle(node->peerId());
    Q_ASSERT(!handle.isNull());

    // search for existing texture
    const auto end = m_sharedTextures.end();
    for (auto it = m_sharedTextures.begin(); it != end; ++it) {
        if (isSameTexture(*it.key(), *node)) {
            if (!it.value().contains(handle))
                it.value() << handle;
            return it.key();
        }
    }
    return Q_NULLPTR;
}

/**
 * \internal
 * Returns a GLTexture that matches the given QTexture node.
 * Will make sure that texture data generator jobs are launched,
 * if necessary.
 */
GLTexture *GLTextureManager::createUnique(const Texture *node)
{
    QMutexLocker lock(&m_mutex);

    Q_ASSERT(node);
    GLTexture *newTex = createTexture(*node, true);
    m_uniqueTextures << newTex;
    return newTex;
}

/**
 * \internal
 * De-associate the given GLTexture from the backend node. If the texture
 * is no longer referenced by any other node, it will be deleted.
 */
void GLTextureManager::abandon(GLTexture *tex, const Texture *node)
{
    QMutexLocker lock(&m_mutex);

    if (tex->isUnique()) {
        m_uniqueTextures.removeAll(tex);
        m_abandonedTextures << tex;
    }
    else {
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

/**
 * \internal
 * Change the properties of the given texture, if it is a non-shared texture
 * \return True, if it was changed successfully, false otherwise
 */
bool GLTextureManager::setProperties(GLTexture *tex, const TextureProperties &props)
{
    QMutexLocker lock(&m_mutex);
    Q_ASSERT(tex);

    if (isShared(tex))
        return false;

    tex->setProperties(props);
    m_updatedTextures << tex;

    return true;
}

/**
 * \internal
 * Change the parameters of the given texture, if it is a non-shared texture
 * \return True, if it was changed successfully, false otherwise
 */
bool GLTextureManager::setParameters(GLTexture *tex, const TextureParameters &params)
{
    QMutexLocker lock(&m_mutex);
    Q_ASSERT(tex);

    if (isShared(tex))
        return false;

    tex->setParameters(params);
    m_updatedTextures << tex;

    return true;
}

/**
 * \internal
 * Change the texture images of the given texture, if it is a non-shared texture
 * \return True, if it was changed successfully, false otherwise
 */
bool GLTextureManager::setImages(GLTexture *tex, const QVector<HTextureImage> &images)
{
    QMutexLocker lock(&m_mutex);
    Q_ASSERT(tex);

    if (isShared(tex))
        return false;

    // create Image structs
    QVector<GLTexture::Image> texImgs = texImgsFromNodes(images);
    if (texImgs.size() != images.size())
        return false;

    tex->setImages(texImgs);
    m_updatedTextures << tex;

    return true;
}

/**
 * \internal
 *   Retrieves abandoned textures. This should be regularly called from the OpenGL thread
 *   to make sure needed GL resources are de-allocated.
 */
QVector<GLTexture*> GLTextureManager::takeAbandonedTextures()
{
    QMutexLocker lock(&m_mutex);
    const QVector<GLTexture*> ret = m_abandonedTextures;
    m_abandonedTextures.clear();
    return ret;
}

/**
 * \internal
 *  Retrieves textures that have been modified
 */
QVector<GLTexture*> GLTextureManager::takeUpdatedTextures()
{
    QMutexLocker lock(&m_mutex);
    const QVector<GLTexture*> ret = m_updatedTextures;
    m_updatedTextures.clear();
    return ret;
}

QVector<GLTexture::Image> GLTextureManager::texImgsFromNodes(const QVector<HTextureImage> &images) const
{
    QVector<GLTexture::Image> ret;
    ret.resize(images.size());

    for (int i = 0; i < images.size(); i++) {
        const TextureImage *img = m_textureImageManager->data(images[i]);
        if (!img) {
            qWarning() << "[Qt3DRender::TextureManager] invalid TextureImage handle";
            return QVector<GLTexture::Image>();
        }

        ret[i].generator = img->dataGenerator();
        ret[i].face = img->face();
        ret[i].layer = img->layer();
        ret[i].mipLevel = img->mipLevel();
    }

    return ret;
}

bool GLTextureManager::isShared(GLTexture *impl)
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


} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
