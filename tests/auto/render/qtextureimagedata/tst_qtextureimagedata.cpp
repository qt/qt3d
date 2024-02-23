// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>

#include <Qt3DRender/private/qtextureimagedata_p.h>
#include <Qt3DRender/qtextureimagedata.h>

#include "testarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QTextureImageData : public QObject
{
    Q_OBJECT
public:
    ~tst_QTextureImageData()
    {
    }

private Q_SLOTS:

    void checkSaneDefaults()
    {
        Qt3DRender::QTextureImageData *tid = new Qt3DRender::QTextureImageData();

        QCOMPARE(tid->width(), -1);
        QCOMPARE(tid->height(), -1);
        QCOMPARE(tid->depth(), -1);
        QCOMPARE(tid->layers(), -1);
        QCOMPARE(tid->mipLevels(), -1);
        QCOMPARE(tid->target(), QOpenGLTexture::Target2D);
        QCOMPARE(tid->format(), QOpenGLTexture::NoFormat);
        QCOMPARE(tid->pixelFormat(), QOpenGLTexture::RGBA);
        QCOMPARE(tid->pixelType(), QOpenGLTexture::UInt8);
        QCOMPARE(tid->isCompressed(), false);
        QCOMPARE(tid->alignment(), 1);
    }

    void checkTextureDataUsesFunctor() {
        Qt3DRender::QTextureImageData *tid = new Qt3DRender::QTextureImageData();

        tid->setLayers(1);
        tid->setFaces(1);
        tid->setMipLevels(1);
        tid->setData({}, [](QByteArray, int, int, int) {
            return QByteArray("a");
        }, false);

        QByteArray data = tid->data();
        QCOMPARE(data.data()[0], 'a');
    }
};

QTEST_MAIN(tst_QTextureImageData)

#include "tst_qtextureimagedata.moc"
