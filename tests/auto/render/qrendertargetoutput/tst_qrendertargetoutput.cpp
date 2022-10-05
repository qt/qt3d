// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <QObject>
#include <QMetaObject>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/private/qrendertargetoutput_p.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtexture.h>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QRenderTargetOutput : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QRenderTargetOutput::AttachmentPoint>("AttachmentPoint");
        qRegisterMetaType<Qt3DRender::QAbstractTexture::CubeMapFace>("QAbstractTexture::CubeMapFace");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QRenderTargetOutput renderTargetOutput;

        // THEN
        QCOMPARE(renderTargetOutput.attachmentPoint(), Qt3DRender::QRenderTargetOutput::Color0);
        QVERIFY(renderTargetOutput.texture() == nullptr);
        QCOMPARE(renderTargetOutput.mipLevel(), 0);
        QCOMPARE(renderTargetOutput.layer(), 0);
        QCOMPARE(renderTargetOutput.face(), Qt3DRender::QAbstractTexture::CubeMapNegativeX);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QRenderTargetOutput renderTargetOutput;

        {
            // WHEN
            QSignalSpy spy(&renderTargetOutput, SIGNAL(attachmentPointChanged(AttachmentPoint)));
            const Qt3DRender::QRenderTargetOutput::AttachmentPoint newValue = Qt3DRender::QRenderTargetOutput::Color15;
            renderTargetOutput.setAttachmentPoint(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderTargetOutput.attachmentPoint(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderTargetOutput.setAttachmentPoint(newValue);

            // THEN
            QCOMPARE(renderTargetOutput.attachmentPoint(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&renderTargetOutput, SIGNAL(textureChanged(QAbstractTexture *)));
            Qt3DRender::QTexture3D newValue;
            renderTargetOutput.setTexture(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderTargetOutput.texture(), &newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderTargetOutput.setTexture(&newValue);

            // THEN
            QCOMPARE(renderTargetOutput.texture(), &newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&renderTargetOutput, SIGNAL(mipLevelChanged(int)));
            const int newValue = 5;
            renderTargetOutput.setMipLevel(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderTargetOutput.mipLevel(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderTargetOutput.setMipLevel(newValue);

            // THEN
            QCOMPARE(renderTargetOutput.mipLevel(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&renderTargetOutput, SIGNAL(layerChanged(int)));
            const int newValue = 300;
            renderTargetOutput.setLayer(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderTargetOutput.layer(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderTargetOutput.setLayer(newValue);

            // THEN
            QCOMPARE(renderTargetOutput.layer(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&renderTargetOutput, SIGNAL(faceChanged(QAbstractTexture::CubeMapFace)));
            const Qt3DRender::QAbstractTexture::CubeMapFace newValue = Qt3DRender::QAbstractTexture::CubeMapNegativeZ;
            renderTargetOutput.setFace(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderTargetOutput.face(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderTargetOutput.setFace(newValue);

            // THEN
            QCOMPARE(renderTargetOutput.face(), newValue);
            QCOMPARE(spy.size(), 0);

        }
    }

    void checkAttachmentPointUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderTargetOutput renderTargetOutput;
        arbiter.setArbiterOnNode(&renderTargetOutput);

        {
            // WHEN
            renderTargetOutput.setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderTargetOutput);

            arbiter.clear();
        }

        {
            // WHEN
            renderTargetOutput.setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkTextureUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderTargetOutput renderTargetOutput;
        Qt3DRender::QTexture3D texture;
        arbiter.setArbiterOnNode(&renderTargetOutput);

        {
            // WHEN
            renderTargetOutput.setTexture(&texture);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderTargetOutput);

            arbiter.clear();
        }

        {
            // WHEN
            renderTargetOutput.setTexture(&texture);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkMipLevelUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderTargetOutput renderTargetOutput;
        arbiter.setArbiterOnNode(&renderTargetOutput);

        {
            // WHEN
            renderTargetOutput.setMipLevel(6);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderTargetOutput);

            arbiter.clear();
        }

        {
            // WHEN
            renderTargetOutput.setMipLevel(6);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLayerUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderTargetOutput renderTargetOutput;
        arbiter.setArbiterOnNode(&renderTargetOutput);

        {
            // WHEN
            renderTargetOutput.setLayer(8);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderTargetOutput);

            arbiter.clear();
        }

        {
            // WHEN
            renderTargetOutput.setLayer(8);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFaceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderTargetOutput renderTargetOutput;
        arbiter.setArbiterOnNode(&renderTargetOutput);

        {
            // WHEN
            renderTargetOutput.setFace(Qt3DRender::QAbstractTexture::CubeMapPositiveY);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderTargetOutput);

            arbiter.clear();
        }

        {
            // WHEN
            renderTargetOutput.setFace(Qt3DRender::QAbstractTexture::CubeMapPositiveY);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QRenderTargetOutput)

#include "tst_qrendertargetoutput.moc"
