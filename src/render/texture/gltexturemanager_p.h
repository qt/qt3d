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

#ifndef QT3DRENDER_RENDER_GLTEXTUREMANAGER_H
#define QT3DRENDER_RENDER_GLTEXTUREMANAGER_H

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

#include <Qt3DRender/qtexturegenerator.h>
#include <Qt3DRender/qtextureimagedatagenerator.h>
#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/gltexture_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class TextureDataManager;
class TextureImageManager;
class TextureImageDataManager;

class GLTextureManager
{
public:

    GLTextureManager(TextureManager *textureNodeManager,
                     TextureImageManager *textureImageManager,
                     TextureDataManager *textureDataManager,
                     TextureImageDataManager *textureImageDataManager);

    ~GLTextureManager();

    GLTexture *getOrCreateShared(const Texture *node);
    GLTexture *tryFindShared(const Texture *node);
    GLTexture *createUnique(const Texture *node);
    void abandon(GLTexture *tex, const Texture *node);
    bool setProperties(GLTexture *tex, const TextureProperties &props);
    bool setParameters(GLTexture *tex, const TextureParameters &params);
    bool setImages(GLTexture *tex, const QVector<HTextureImage> &images);
    QVector<GLTexture*> takeAbandonedTextures();

    /**
     * @brief Retrieves textures that have been modified
     */
    QVector<GLTexture*> takeUpdatedTextures();

    /**
     * @brief Returns whether the given GLTexture is shared between multiple TextureNodes
     */
    bool isShared(GLTexture *impl);

private:

    /**
     * @brief Check if the given GLTexture matches the TextureNode
     */
    bool isSameTexture(const GLTexture &tex, const Texture &texNode);

    /**
     * @brief
     *  Create GLTexture from given TextureNode. Also make sure the generators
     *  will be executed by jobs soon.
     */
    GLTexture *createTexture(const Texture &node, bool unique);

    QVector<GLTexture::Image> texImgsFromNodes(const QVector<HTextureImage> &images) const;

    QMutex m_mutex;

    TextureManager *m_textureNodeManager;
    TextureImageManager *m_textureImageManager;
    TextureDataManager *m_textureDataManager;
    TextureImageDataManager *m_textureImageDataManager;

    /* each non-unique texture is associated with a number of Texture nodes referencing it */
    QHash<GLTexture*, QVector<HTexture>> m_sharedTextures;

    QVector<GLTexture*> m_uniqueTextures;

    QVector<GLTexture*> m_abandonedTextures;
    QVector<GLTexture*> m_updatedTextures;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GLTEXTUREMANAGER_H
