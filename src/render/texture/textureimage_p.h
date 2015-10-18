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

#ifndef QT3DRENDER_RENDER_TEXTUREIMAGE_H
#define QT3DRENDER_RENDER_TEXTUREIMAGE_H

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

#include <qglobal.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DRender/qabstracttextureprovider.h>
#include <Qt3DRender/qabstracttextureimage.h>
#include <Qt3DRender/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class TextureManager;
class TextureImageManager;
class TextureDataManager;

typedef uint TextureImageDNA;

class TextureImage : public Qt3DCore::QBackendNode
{
public:
    TextureImage();
    void cleanup();
    void updateFromPeer(Qt3DCore::QNode *peer) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE;

    int m_layer;
    int m_mipmapLevel;
    QAbstractTextureProvider::CubeMapFace m_face;
    bool m_dirty;
    inline TextureImageDNA dna() const { return m_dna; }

    inline int layer() const { return m_layer; }
    inline int mipmapLevel() const { return m_mipmapLevel; }
    inline QAbstractTextureProvider::CubeMapFace face() const { return m_face; }

    void setTextureManager(TextureManager *manager);
    void setTextureImageManager(TextureImageManager *manager);
    void setTextureDataManager(TextureDataManager *manager);
    void unsetDirty();

    inline bool isDirty() const { return m_dirty; }
    inline QTextureDataFunctorPtr textureDataFunctor() const { return m_functor; }

    void setTextureDataHandle(HTextureData handle);

    inline HTextureData textureDataHandle() const { return m_textureDataHandle; }
    inline QTextureDataFunctorPtr dataFunctor() const { return m_functor; }

private:
    void updateDNA();

    QTextureDataFunctorPtr m_functor;
    HTextureData m_textureDataHandle;
    TextureManager *m_textureManager;
    TextureImageManager *m_textureImageManager;
    TextureDataManager *m_textureDataManager;
    QList<Qt3DCore::QNodeId> m_referencedTextures;
    HTexture m_textureProvider;
    Qt3DCore::QNodeId m_textureProviderId;
    TextureImageDNA m_dna;
};

class TextureImageFunctor : public Qt3DCore::QBackendNodeFunctor
{
public:
    explicit TextureImageFunctor(TextureManager *textureManager,
                                  TextureImageManager *textureImageManager,
                                  TextureDataManager *textureDataManager);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const Q_DECL_FINAL;
    Qt3DCore::QBackendNode *get(const Qt3DCore::QNodeId &id) const Q_DECL_FINAL;
    void destroy(const Qt3DCore::QNodeId &id) const Q_DECL_FINAL;

private:
    TextureManager *m_textureManager;
    TextureImageManager *m_textureImageManager;
    TextureDataManager *m_textureDataManager;
};


} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_TEXTUREIMAGE_H
