// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTEXTUREATLAS_P_P_H
#define QT3DEXTRAS_QTEXTUREATLAS_P_P_H

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

#include <QtCore/qscopedpointer.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DRender/private/qtexturegenerator_p.h>
#include <Qt3DExtras/private/areaallocator_p.h>
#include <Qt3DExtras/private/qtextureatlas_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

// Used to store texture info within atlas
struct AtlasTexture
{
    QRect position;
    int padding = 0;
};

// data shared between QTextureAtlasPrivate and the QTextureGenerators
// we use this extra indirection so we can lazily copy the sub-images
// into the actual texture image in the backend texture loader thread.
class QTextureAtlasData
{
public:
    QTextureAtlasData(int w, int h, QImage::Format fmt);
    ~QTextureAtlasData();

    int width() const { return m_image.width(); }
    int height() const { return m_image.height(); }

    void addImage(const AtlasTexture &texture, const QImage &image);
    QByteArray createUpdatedImageData();

private:
    struct Update {
        AtlasTexture textureInfo;
        QImage image;
    };

    QMutex m_mutex;
    QImage m_image;
    QList<Update> m_updates;
};

typedef QSharedPointer<QTextureAtlasData> QTextureAtlasDataPtr;

class QTextureAtlasPrivate : public Qt3DRender::QAbstractTexturePrivate
{
public:
    QTextureAtlasPrivate();
    ~QTextureAtlasPrivate();

    Q_DECLARE_PUBLIC(QTextureAtlas)

    QTextureAtlas::TextureId m_currId = 1;  // IDs for new sub-textures
    int m_currGen = 0;

    QTextureAtlasDataPtr m_data;
    QScopedPointer<AreaAllocator> m_allocator;
    QOpenGLTexture::PixelFormat m_pixelFormat;
    QHash<QTextureAtlas::TextureId, AtlasTexture> m_textures;
};

class QTextureAtlasGenerator : public Qt3DRender::QTextureGenerator
{
public:
    QTextureAtlasGenerator(const QTextureAtlasPrivate *texAtlas);
    ~QTextureAtlasGenerator();
    Qt3DRender::QTextureDataPtr operator()() override;
    bool operator==(const QTextureGenerator &other) const override;

    QT3D_FUNCTOR(QTextureAtlasGenerator)

private:
    QTextureAtlasDataPtr m_data;
    Qt3DRender::QAbstractTexture::TextureFormat m_format;
    QOpenGLTexture::PixelFormat m_pixelFormat;
    int m_generation;
    Qt3DCore::QNodeId m_atlasId;
};
typedef QSharedPointer<QTextureAtlasGenerator> QTextureAtlasGeneratorPtr;

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXTUREATLAS_P_P_H
