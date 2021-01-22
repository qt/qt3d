/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QTextureAtlasPrivate;

class QTextureAtlas : public Qt3DRender::QAbstractTexture
{
    Q_OBJECT

public:
    typedef int TextureId;
    static Q_CONSTEXPR TextureId InvalidTexture = -1;

    QTextureAtlas(Qt3DCore::QNode *parent = nullptr);
    ~QTextureAtlas();

    QOpenGLTexture::PixelFormat pixelFormat() const;
    void setPixelFormat(QOpenGLTexture::PixelFormat fmt);

    TextureId addImage(const QImage &image, int padding);
    void removeImage(TextureId id);

    int imageCount() const;

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
