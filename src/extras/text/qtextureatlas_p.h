// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTEXTUREATLAS_P_H
#define QT3DEXTRAS_QTEXTUREATLAS_P_H

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

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qabstracttexture.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QTextureAtlasPrivate;

class QTextureAtlas : public Qt3DRender::QAbstractTexture
{
    Q_OBJECT

public:
    typedef int TextureId;
    static constexpr TextureId InvalidTexture = -1;

    QTextureAtlas(Qt3DCore::QNode *parent = nullptr);
    ~QTextureAtlas();

    QOpenGLTexture::PixelFormat pixelFormat() const;
    void setPixelFormat(QOpenGLTexture::PixelFormat fmt);

    TextureId addImage(const QImage &image, int padding);
    void removeImage(TextureId id);

    qsizetype imageCount() const;

    bool hasImage(TextureId id) const;
    QRect imagePosition(TextureId id) const;
    QRectF imageTexCoords(TextureId id) const;
    int imagePadding(TextureId id) const;

private:
    Q_DECLARE_PRIVATE(QTextureAtlas)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXTUREATLAS_P_H
