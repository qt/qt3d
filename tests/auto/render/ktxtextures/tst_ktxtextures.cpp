// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/private/qtexture_p.h>

class tst_KtxTextures : public QObject
{
    Q_OBJECT

private slots:
    void ktxImageData();
};

void tst_KtxTextures::ktxImageData()
{
    const struct TextureInfo {
        const char *source;
        int width;
        int height;
        int depth;
        int faces;
        int mipmapLevels;
        QOpenGLTexture::TextureFormat format;
    } textures[] = {
        { "data/16_16_PNG_ASTC_7.KTX", 16, 16, 1, 1, 1, QOpenGLTexture::RGBA_ASTC_4x4 },
        { "data/16_16_PNG_ASTC_8.KTX", 16, 16, 1, 1, 1, QOpenGLTexture::RGBA_ASTC_4x4 },
        { "data/16_16_PNG_ETC2_RGB_13.KTX", 16, 16, 1, 1, 1, QOpenGLTexture::RGB8_ETC2 },
        { "data/16_16_PNG_ETC2_RGBA_11.KTX", 16, 16, 1, 1, 1, QOpenGLTexture::RGBA8_ETC2_EAC },
        { "data/16_16_PNG_ETC_RGB_12.KTX", 16, 16, 1, 1, 1, QOpenGLTexture::RGB8_ETC1 },
    };

    for (unsigned i = 0; i < sizeof(textures)/sizeof(*textures); i++) {
        const TextureInfo *texture = &textures[i];

        Qt3DRender::QTextureImageDataPtr data = Qt3DRender::TextureLoadingHelper::loadTextureData(QUrl::fromLocalFile(QFINDTESTDATA(texture->source)), true, false);

        QVERIFY(data);
        QCOMPARE(data->width(), texture->width);
        QCOMPARE(data->height(), texture->height);
        QCOMPARE(data->faces(), texture->faces);
        QCOMPARE(data->mipLevels(), texture->mipmapLevels);
        QCOMPARE(data->format(), texture->format);
    }
}

QTEST_APPLESS_MAIN(tst_KtxTextures)

#include "tst_ktxtextures.moc"
