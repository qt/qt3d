// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/texture_p.h>

#include "testrenderer.h"
#include <testarbiter.h>

class DummyTexture : public Qt3DRender::QAbstractTexture
{
    Q_OBJECT

public:
    explicit DummyTexture(Qt3DCore::QNode *parent = nullptr)
        : QAbstractTexture(TargetAutomatic, parent)
    {
    }
};

class FakeGenerator : public Qt3DRender::QTextureGenerator
{
public:
    QT3D_FUNCTOR(FakeGenerator)

    Qt3DRender::QTextureDataPtr operator ()() override
    {
        return {};
    }

    bool operator ==(const QTextureGenerator &) const override
    {
        return true;
    }
};

class tst_RenderTexture : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private:
    template <typename FrontendTextureType, Qt3DRender::QAbstractTexture::Target Target>
    void checkPropertyMirroring();

private slots:
    void checkDefaults();
    void checkFrontendPropertyNotifications();
    void checkPropertyMirroring();
    void checkPropertyChanges();
    void checkTextureImageBookeeping();
    void checkInitialUpdateData();
};

void tst_RenderTexture::checkDefaults()
{
    Qt3DRender::Render::Texture backend;

    QCOMPARE(backend.properties().format, Qt3DRender::QAbstractTexture::NoFormat);
    QCOMPARE(backend.properties().width, 1);
    QCOMPARE(backend.properties().height, 1);
    QCOMPARE(backend.properties().depth, 1);
    QCOMPARE(backend.properties().layers, 1);
    QCOMPARE(backend.properties().mipLevels, 1);
    QCOMPARE(backend.properties().samples, 1);
    QCOMPARE(backend.properties().generateMipMaps, false);
    QCOMPARE(backend.parameters().magnificationFilter, Qt3DRender::QAbstractTexture::Nearest);
    QCOMPARE(backend.parameters().minificationFilter, Qt3DRender::QAbstractTexture::Nearest);
    QCOMPARE(backend.parameters().wrapModeX, Qt3DRender::QTextureWrapMode::ClampToEdge);
    QCOMPARE(backend.parameters().wrapModeY, Qt3DRender::QTextureWrapMode::ClampToEdge);
    QCOMPARE(backend.parameters().wrapModeZ, Qt3DRender::QTextureWrapMode::ClampToEdge);
    QCOMPARE(backend.parameters().maximumAnisotropy, 1.0f);
    QCOMPARE(backend.parameters().comparisonFunction, Qt3DRender::QAbstractTexture::CompareLessEqual);
    QCOMPARE(backend.parameters().comparisonMode, Qt3DRender::QAbstractTexture::CompareNone);
}

void tst_RenderTexture::checkFrontendPropertyNotifications()
{
    // GIVEN
    TestArbiter arbiter;
    DummyTexture texture;
    arbiter.setArbiterOnNode(&texture);

    // WHEN
    texture.setWidth(512);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), &texture);

    arbiter.clear();

    // WHEN
    texture.setWidth(512);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 0);

    // WHEN
    texture.setHeight(256);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), &texture);

    arbiter.clear();

    // WHEN
    texture.setHeight(256);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 0);

    // WHEN
    texture.setDepth(128);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), &texture);

    arbiter.clear();

    // WHEN
    texture.setDepth(128);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 0);

    // WHEN
    texture.setLayers(16);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), &texture);

    arbiter.clear();

    // WHEN
    texture.setLayers(16);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 0);

    // WHEN
    texture.setSamples(32);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), &texture);

    arbiter.clear();

    // WHEN
    texture.setSamples(32);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 0);

    // WHEN
    Qt3DRender::QTextureImage img;
    texture.addTextureImage(&img);

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), &texture);

    arbiter.clear();

    // WHEN
    texture.removeTextureImage(&img);

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), &texture);

    arbiter.clear();
}

template <typename FrontendTextureType, Qt3DRender::QAbstractTexture::Target Target>
void tst_RenderTexture::checkPropertyMirroring()
{
    // GIVEN
    Qt3DRender::Render::Texture backend;

    FrontendTextureType frontend;
    frontend.setWidth(256);
    frontend.setHeight(128);
    frontend.setDepth(16);
    frontend.setLayers(8);
    frontend.setSamples(32);
    frontend.setMipLevels(8);

    // WHEN
    simulateInitializationSync(&frontend, &backend);

    // THEN
    QCOMPARE(backend.peerId(), frontend.id());
    QCOMPARE(backend.properties().target, Target);
    QCOMPARE(backend.properties().width, frontend.width());
    QCOMPARE(backend.properties().height, frontend.height());
    QCOMPARE(backend.properties().depth, frontend.depth());
    QCOMPARE(backend.properties().layers, frontend.layers());
    QCOMPARE(backend.properties().samples, frontend.samples());
}

void tst_RenderTexture::checkPropertyMirroring()
{
    checkPropertyMirroring<Qt3DRender::QTexture1D, Qt3DRender::QAbstractTexture::Target1D>();
    checkPropertyMirroring<Qt3DRender::QTexture1DArray, Qt3DRender::QAbstractTexture::Target1DArray>();
    checkPropertyMirroring<Qt3DRender::QTexture2D, Qt3DRender::QAbstractTexture::Target2D>();
    checkPropertyMirroring<Qt3DRender::QTexture2DArray, Qt3DRender::QAbstractTexture::Target2DArray>();
    checkPropertyMirroring<Qt3DRender::QTexture3D, Qt3DRender::QAbstractTexture::Target3D>();
    checkPropertyMirroring<Qt3DRender::QTextureCubeMap, Qt3DRender::QAbstractTexture::TargetCubeMap>();
    checkPropertyMirroring<Qt3DRender::QTextureCubeMapArray, Qt3DRender::QAbstractTexture::TargetCubeMapArray>();
    checkPropertyMirroring<Qt3DRender::QTexture2DMultisample, Qt3DRender::QAbstractTexture::Target2DMultisample>();
    checkPropertyMirroring<Qt3DRender::QTexture2DMultisampleArray, Qt3DRender::QAbstractTexture::Target2DMultisampleArray>();
    checkPropertyMirroring<Qt3DRender::QTextureRectangle, Qt3DRender::QAbstractTexture::TargetRectangle>();
    checkPropertyMirroring<Qt3DRender::QTextureBuffer, Qt3DRender::QAbstractTexture::TargetBuffer>();
}

void tst_RenderTexture::checkPropertyChanges()
{
    // GIVEN
    TestRenderer renderer;
    Qt3DRender::QSharedGLTexture frontend;
    Qt3DRender::Render::Texture backend;
    backend.setRenderer(&renderer);
    backend.unsetDirty();

    // WHEN
    frontend.setWidth(256);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.properties().width, 256);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyProperties);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setHeight(128);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.properties().height, 128);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyProperties);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setDepth(16);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.properties().depth, 16);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyProperties);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setFormat(Qt3DRender::QAbstractTexture::RGB16F);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.properties().format, Qt3DRender::QAbstractTexture::RGB16F);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyProperties);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setGenerateMipMaps(true);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.properties().generateMipMaps, true);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyProperties);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setMinificationFilter(Qt3DRender::QAbstractTexture::LinearMipMapLinear);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().minificationFilter, Qt3DRender::QAbstractTexture::LinearMipMapLinear);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().magnificationFilter, Qt3DRender::QAbstractTexture::Linear);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.wrapMode()->setX(Qt3DRender::QTextureWrapMode::Repeat);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().wrapModeX, Qt3DRender::QTextureWrapMode::Repeat);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.wrapMode()->setY(Qt3DRender::QTextureWrapMode::Repeat);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().wrapModeY, Qt3DRender::QTextureWrapMode::Repeat);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.wrapMode()->setZ(Qt3DRender::QTextureWrapMode::Repeat);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().wrapModeZ, Qt3DRender::QTextureWrapMode::Repeat);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setMaximumAnisotropy(16.f);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().maximumAnisotropy, 16.0f);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setComparisonFunction(Qt3DRender::QAbstractTexture::CompareEqual);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().comparisonFunction, Qt3DRender::QAbstractTexture::CompareEqual);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.parameters().comparisonMode, Qt3DRender::QAbstractTexture::CompareRefToTexture);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyParameters);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setLayers(32);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.properties().layers, 32);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyProperties);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setSamples(64);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.properties().samples, 64);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyProperties);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    frontend.setTextureId(883);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.sharedTextureId(), 883);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtySharedTextureId);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    Qt3DRender::QTextureImage img;
    frontend.addTextureImage(&img);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    QCOMPARE(backend.textureImageIds().size(), 1);
    QCOMPARE(backend.textureImageIds().first(), img.id());
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyImageGenerators);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();

    // WHEN
    Qt3DRender::QTextureDataUpdate updateData;
    updateData.setX(100);
    updateData.setY(100);
    updateData.setZ(100);
    frontend.updateData(updateData);
    backend.syncFromFrontEnd(&frontend, false);

    // THEN
    const std::vector<Qt3DRender::QTextureDataUpdate> pendingUpdates = backend.takePendingTextureDataUpdates();
    QCOMPARE(pendingUpdates.size(), 1U);
    QCOMPARE(pendingUpdates.front(), updateData);
    QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TexturesDirty);
    QVERIFY(backend.dirtyFlags() == Qt3DRender::Render::Texture::DirtyPendingDataUpdates);
    renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    backend.unsetDirty();
}

void tst_RenderTexture::checkTextureImageBookeeping()
{
    // GIVEN
    TestArbiter arbiter;
    DummyTexture texture;
    arbiter.setArbiterOnNode(&texture);

    QCoreApplication::processEvents();

    {
        // WHEN
        Qt3DRender::QTextureImage img;
        texture.addTextureImage(&img);

        // THEN
        QCOMPARE(img.parent(), &texture);
        QCOMPARE(texture.textureImages().size(), 1);
        QCOMPARE(texture.textureImages().first()->id(), img.id());
    }
    // WHEN -> img is destroyed

    // THEN
    QCOMPARE(texture.textureImages().size(), 0);
}

void tst_RenderTexture::checkInitialUpdateData()
{
    // GIVEN
    TestRenderer renderer;
    DummyTexture frontend;
    Qt3DRender::Render::Texture backend;

    backend.setRenderer(&renderer);

    Qt3DRender::QTextureDataUpdate updateData;
    updateData.setX(100);
    updateData.setY(100);
    updateData.setZ(100);

    // WHEN -> updateData with no backend/arbiter
    frontend.updateData(updateData);
    simulateInitializationSync(&frontend, &backend);

    // THEN -> should have received the update as part of the initial data
    const std::vector<Qt3DRender::QTextureDataUpdate> pendingUpdates = backend.takePendingTextureDataUpdates();
    QCOMPARE(pendingUpdates.size(), 1U);
    QCOMPARE(pendingUpdates.front(), updateData);
    QVERIFY(backend.dirtyFlags() & Qt3DRender::Render::Texture::DirtyPendingDataUpdates);
}

QTEST_APPLESS_MAIN(tst_RenderTexture)

#include "tst_texture.moc"
