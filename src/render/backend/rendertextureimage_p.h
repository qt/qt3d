/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_RENDER_RENDERTEXTUREIMAGE_H
#define QT3D_RENDER_RENDERTEXTUREIMAGE_H

#include <qglobal.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DRenderer/qabstracttextureprovider.h>
#include <Qt3DRenderer/qabstracttextureimage.h>
#include <Qt3DRenderer/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

class TextureManager;
class TextureImageManager;
class TextureDataManager;

typedef uint TextureImageDNA;

class RenderTextureImage : public QBackendNode
{
public:
    RenderTextureImage();
    void cleanup();
    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;

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
    QList<QNodeId> m_referencedTextures;
    HTexture m_textureProvider;
    QNodeId m_textureProviderId;
    TextureImageDNA m_dna;
};

class RenderTextureImageFunctor : public QBackendNodeFunctor
{
public:
    explicit RenderTextureImageFunctor(TextureManager *textureManager,
                                  TextureImageManager *textureImageManager,
                                  TextureDataManager *textureDataManager);

    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_FINAL;
    QBackendNode *get(QNode *frontend) const Q_DECL_FINAL;
    void destroy(QNode *frontend) const Q_DECL_FINAL;

private:
    TextureManager *m_textureManager;
    TextureImageManager *m_textureImageManager;
    TextureDataManager *m_textureDataManager;
};


} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERTEXTUREIMAGE_H
