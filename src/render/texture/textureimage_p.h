// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtextureimagedatagenerator.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class TextureManager;
class TextureImageManager;
class TextureImageDataManager;

/**
 * Backend class for QAbstractTextureImage.
 * Will only hold the generator and some info values.
 */
class Q_3DRENDERSHARED_PRIVATE_EXPORT TextureImage : public BackendNode
{
public:
    TextureImage();
    ~TextureImage();

    void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    inline int layer() const { return m_layer; }
    inline int mipLevel() const { return m_mipLevel; }
    inline QAbstractTexture::CubeMapFace face() const { return m_face; }
    inline QTextureImageDataGeneratorPtr dataGenerator() const { return m_generator; }
    inline bool isDirty() const { return m_dirty; }
    void unsetDirty();

private:
    bool m_dirty;
    int m_layer;
    int m_mipLevel;
    QAbstractTexture::CubeMapFace m_face;
    QTextureImageDataGeneratorPtr m_generator;
};

class TextureImageFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit TextureImageFunctor(AbstractRenderer *renderer,
                                 TextureImageManager *textureImageManager);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    AbstractRenderer *m_renderer;
    TextureImageManager *m_textureImageManager;
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const TextureImage &textureImage)
{
    QDebugStateSaver saver(dbg);
    dbg << "QNodeId =" << textureImage.peerId()
        << "mip level =" << textureImage.mipLevel()
        << "layer =" << textureImage.layer()
        << "cube face =" << textureImage.face()
        << "dataGenerator =" << textureImage.dataGenerator() << Qt::endl;
    return dbg;
}
#endif


} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_TEXTUREIMAGE_H
