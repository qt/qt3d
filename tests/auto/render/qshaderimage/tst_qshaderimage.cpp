// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/qshaderimage.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/private/qshaderimage_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QShaderImage : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QAbstractTexture*>("Qt3DRender::QAbstractTexture*");

    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;

        // THEN
        QVERIFY(shaderImage.texture() == nullptr);
        QCOMPARE(shaderImage.layered(), false);
        QCOMPARE(shaderImage.mipLevel(), 0);
        QCOMPARE(shaderImage.layer(), 0);
        QCOMPARE(shaderImage.access(), Qt3DRender::QShaderImage::ReadWrite);
        QCOMPARE(shaderImage.format(), Qt3DRender::QShaderImage::Automatic);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;

        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(textureChanged(Qt3DRender::QAbstractTexture*)));
            Qt3DRender::QTexture2D newValue;
            shaderImage.setTexture(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.texture(), &newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderImage.setTexture(&newValue);

            // THEN
            QCOMPARE(shaderImage.texture(), &newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(layeredChanged(bool)));
            const bool newValue = true;
            shaderImage.setLayered(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.layered(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderImage.setLayered(newValue);

            // THEN
            QCOMPARE(shaderImage.layered(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(mipLevelChanged(int)));
            const int newValue = 12;
            shaderImage.setMipLevel(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.mipLevel(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderImage.setMipLevel(newValue);

            // THEN
            QCOMPARE(shaderImage.mipLevel(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(layerChanged(int)));
            const int newValue = 2;
            shaderImage.setLayer(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.layer(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderImage.setLayer(newValue);

            // THEN
            QCOMPARE(shaderImage.layer(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(accessChanged(Access)));
            const Qt3DRender::QShaderImage::Access newValue = Qt3DRender::QShaderImage::ReadOnly;
            shaderImage.setAccess(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.access(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderImage.setAccess(newValue);

            // THEN
            QCOMPARE(shaderImage.access(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(formatChanged(ImageFormat)));
            const Qt3DRender::QShaderImage::ImageFormat newValue = Qt3DRender::QShaderImage::RG8U;
            shaderImage.setFormat(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.format(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderImage.setFormat(newValue);

            // THEN
            QCOMPARE(shaderImage.format(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkTextureUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);
        Qt3DRender::QTexture2D t;

        {
            // WHEN
            shaderImage.setTexture(&t);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setTexture(&t);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLayeredUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setLayered(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setLayered(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkMipLevelUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setMipLevel(5);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setMipLevel(5);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLayerUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setLayer(8);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setLayer(8);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkAccessUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setAccess(Qt3DRender::QShaderImage::WriteOnly);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setAccess(Qt3DRender::QShaderImage::WriteOnly);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFormatUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setFormat(Qt3DRender::QShaderImage::RG16F);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setFormat(Qt3DRender::QShaderImage::RG16F);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkTextureBookeeping()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;
        QSignalSpy spy(&shaderImage, SIGNAL(textureChanged(Qt3DRender::QAbstractTexture*)));

        {
            // WHEN
            Qt3DRender::QTexture2D tex;
            shaderImage.setTexture(&tex);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.texture(), &tex);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
        }
        // THEN
        QCOMPARE(spy.size(), 1);
        QVERIFY(shaderImage.texture() == nullptr);
    }

};

QTEST_MAIN(tst_QShaderImage)

#include "tst_qshaderimage.moc"
