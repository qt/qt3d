// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/qshaderimage.h>
#include <Qt3DRender/private/qshaderimage_p.h>
#include <Qt3DRender/private/shaderimage_p.h>
#include <Qt3DRender/qtexture.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_ShaderImage : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::ShaderImage backendShaderImage;

        // THEN
        QCOMPARE(backendShaderImage.isEnabled(), false);
        QVERIFY(backendShaderImage.peerId().isNull());
        QCOMPARE(backendShaderImage.textureId(), Qt3DCore::QNodeId());
        QCOMPARE(backendShaderImage.mipLevel(), int(0));
        QCOMPARE(backendShaderImage.layer(), int(0));
        QCOMPARE(backendShaderImage.layered(), false);
        QCOMPARE(backendShaderImage.access(), Qt3DRender::QShaderImage::ReadWrite);
        QCOMPARE(backendShaderImage.format(), Qt3DRender::QShaderImage::NoFormat);
    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DRender::Render::ShaderImage backendShaderImage;

        // WHEN
        backendShaderImage.setEnabled(true);
        backendShaderImage.setTextureId(Qt3DCore::QNodeId::createId());
        backendShaderImage.setMipLevel(8);
        backendShaderImage.setLayer(200);
        backendShaderImage.setLayered(true);
        backendShaderImage.setAccess(Qt3DRender::QShaderImage::WriteOnly);
        backendShaderImage.setFormat(Qt3DRender::QShaderImage::R32F);

        backendShaderImage.cleanup();

        // THEN
        QCOMPARE(backendShaderImage.isEnabled(), false);
        QCOMPARE(backendShaderImage.textureId(), Qt3DCore::QNodeId());
        QCOMPARE(backendShaderImage.mipLevel(), int(0));
        QCOMPARE(backendShaderImage.layer(), int(0));
        QCOMPARE(backendShaderImage.layered(), false);
        QCOMPARE(backendShaderImage.access(), Qt3DRender::QShaderImage::ReadWrite);
        QCOMPARE(backendShaderImage.format(), Qt3DRender::QShaderImage::NoFormat);

    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;
        TestRenderer renderer;

        {
            // WHEN
            Qt3DRender::Render::ShaderImage backendShaderImage;
            backendShaderImage.setRenderer(&renderer);
            simulateInitializationSync(&shaderImage, &backendShaderImage);

            // THEN
            QCOMPARE(backendShaderImage.isEnabled(), true);
            QCOMPARE(backendShaderImage.peerId(), shaderImage.id());
            QCOMPARE(backendShaderImage.textureId(), Qt3DCore::QNodeId());
            QCOMPARE(backendShaderImage.mipLevel(), 0);
            QCOMPARE(backendShaderImage.layer(), 0);
            QCOMPARE(backendShaderImage.layered(), false);
            QCOMPARE(backendShaderImage.access(),  Qt3DRender::QShaderImage::ReadWrite);
            QCOMPARE(backendShaderImage.format(),  Qt3DRender::QShaderImage::Automatic);
        }
        {
            // WHEN
            Qt3DRender::Render::ShaderImage backendShaderImage;
            shaderImage.setEnabled(false);
            backendShaderImage.setRenderer(&renderer);
            simulateInitializationSync(&shaderImage, &backendShaderImage);

            // THEN
            QCOMPARE(backendShaderImage.peerId(), shaderImage.id());
            QCOMPARE(backendShaderImage.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;
        Qt3DRender::Render::ShaderImage backendShaderImage;
        TestRenderer renderer;
        backendShaderImage.setRenderer(&renderer);
        simulateInitializationSync(&shaderImage, &backendShaderImage);

        {
             // WHEN
             const bool newValue = false;
             shaderImage.setEnabled(newValue);
             backendShaderImage.syncFromFrontEnd(&shaderImage, false);

             // THEN
            QCOMPARE(backendShaderImage.isEnabled(), newValue);
        }
        {
             // WHEN
            Qt3DRender::QTexture2D tex2D;
            shaderImage.setTexture(&tex2D);
            backendShaderImage.syncFromFrontEnd(&shaderImage, false);

             // THEN
            QCOMPARE(backendShaderImage.textureId(), tex2D.id());
        }
        {
             // WHEN
             const int newValue = 883;
             shaderImage.setMipLevel(newValue);
             backendShaderImage.syncFromFrontEnd(&shaderImage, false);

             // THEN
            QCOMPARE(backendShaderImage.mipLevel(), newValue);
        }
        {
             // WHEN
             const int newValue = 1584;
             shaderImage.setLayer(newValue);
             backendShaderImage.syncFromFrontEnd(&shaderImage, false);

             // THEN
            QCOMPARE(backendShaderImage.layer(), newValue);
        }
        {
             // WHEN
             const bool newValue = true;
             shaderImage.setLayered(newValue);
             backendShaderImage.syncFromFrontEnd(&shaderImage, false);

             // THEN
            QCOMPARE(backendShaderImage.layered(), newValue);
        }
        {
             // WHEN
             const  Qt3DRender::QShaderImage::Access newValue = Qt3DRender::QShaderImage::WriteOnly;
             shaderImage.setAccess(newValue);
             backendShaderImage.syncFromFrontEnd(&shaderImage, false);

             // THEN
            QCOMPARE(backendShaderImage.access(), newValue);
        }
        {
             // WHEN
             const  Qt3DRender::QShaderImage::ImageFormat newValue =  Qt3DRender::QShaderImage::RG16F;
             shaderImage.setFormat(newValue);
             backendShaderImage.syncFromFrontEnd(&shaderImage, false);

             // THEN
            QCOMPARE(backendShaderImage.format(), newValue);
        }
    }

};

QTEST_MAIN(tst_ShaderImage)

#include "tst_shaderimage.moc"
