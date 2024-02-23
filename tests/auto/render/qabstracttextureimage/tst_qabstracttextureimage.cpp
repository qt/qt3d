// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qabstracttextureimage.h>
#include <Qt3DRender/private/qabstracttextureimage_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class FakeTextureImage : public Qt3DRender::QAbstractTextureImage
{
protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override
    {
        return Qt3DRender::QTextureImageDataGeneratorPtr();
    }
};

class tst_QAbstractTextureImage : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QAbstractTexture::CubeMapFace>("QAbstractTexture::CubeMapFace");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        FakeTextureImage abstractTextureImage;

        // THEN
        QCOMPARE(abstractTextureImage.mipLevel(), 0);
        QCOMPARE(abstractTextureImage.layer(), 0);
        QCOMPARE(abstractTextureImage.face(), Qt3DRender::QAbstractTexture::CubeMapPositiveX);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        FakeTextureImage abstractTextureImage;

        {
            // WHEN
            QSignalSpy spy(&abstractTextureImage, SIGNAL(mipLevelChanged(int)));
            const int newValue = 5;
            abstractTextureImage.setMipLevel(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTextureImage.mipLevel(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            abstractTextureImage.setMipLevel(newValue);

            // THEN
            QCOMPARE(abstractTextureImage.mipLevel(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTextureImage, SIGNAL(layerChanged(int)));
            const int newValue = 12;
            abstractTextureImage.setLayer(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTextureImage.layer(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            abstractTextureImage.setLayer(newValue);

            // THEN
            QCOMPARE(abstractTextureImage.layer(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTextureImage, SIGNAL(faceChanged(Qt3DRender::QAbstractTexture::CubeMapFace)));
            const Qt3DRender::QAbstractTexture::CubeMapFace newValue =  Qt3DRender::QAbstractTexture::CubeMapNegativeZ;
            abstractTextureImage.setFace(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTextureImage.face(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            abstractTextureImage.setFace(newValue);

            // THEN
            QCOMPARE(abstractTextureImage.face(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkMipLevelUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTextureImage abstractTextureImage;
        arbiter.setArbiterOnNode(&abstractTextureImage);

        {
            // WHEN
            abstractTextureImage.setMipLevel(9);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &abstractTextureImage);

            arbiter.clear();
        }

        {
            // WHEN
            abstractTextureImage.setMipLevel(9);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLayerUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTextureImage abstractTextureImage;
        arbiter.setArbiterOnNode(&abstractTextureImage);

        {
            // WHEN
            abstractTextureImage.setLayer(12);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &abstractTextureImage);

            arbiter.clear();
        }

        {
            // WHEN
            abstractTextureImage.setLayer(12);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFaceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTextureImage abstractTextureImage;
        arbiter.setArbiterOnNode(&abstractTextureImage);

        {
            // WHEN
            abstractTextureImage.setFace(Qt3DRender::QAbstractTexture::CubeMapPositiveY);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &abstractTextureImage);

            arbiter.clear();
        }

        {
            // WHEN
            abstractTextureImage.setFace(Qt3DRender::QAbstractTexture::CubeMapPositiveY);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QAbstractTextureImage)

#include "tst_qabstracttextureimage.moc"
