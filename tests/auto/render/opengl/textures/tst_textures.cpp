/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <qtextureimagedatagenerator.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qtextureimage.h>
#include <Qt3DRender/qtexturedata.h>

#include <renderer_p.h>
#include <glresourcemanagers_p.h>
#include <gltexture_p.h>
#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/textureimage_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/qtexture_p.h>

#include <testrenderer.h>

/**
 * @brief Dummy QTextureImageDataGenerator
 */
class TestImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator
{
    int m_id;
public:
    TestImageDataGenerator(int id) : m_id(id) {}

    Qt3DRender::QTextureImageDataPtr operator ()() override {
        return Qt3DRender::QTextureImageDataPtr::create();
    }

    bool operator ==(const Qt3DRender::QTextureImageDataGenerator &other) const override {
        const TestImageDataGenerator *otherFunctor = Qt3DRender::functor_cast<TestImageDataGenerator>(&other);
        return (otherFunctor != nullptr && otherFunctor->m_id == m_id);
    }

    QT3D_FUNCTOR(TestImageDataGenerator)
};

/**
 * @brief Dummy QTextureGenerator
 */
class TestTextureGenerator : public Qt3DRender::QTextureGenerator
{
    int m_id;
public:
    TestTextureGenerator(int id) : m_id(id) {}

    Qt3DRender::QTextureDataPtr operator ()() override {
        return Qt3DRender::QTextureDataPtr::create();
    }

    bool operator ==(const Qt3DRender::QTextureGenerator &other) const override {
        const TestTextureGenerator *otherFunctor = Qt3DRender::functor_cast<TestTextureGenerator>(&other);
        return (otherFunctor != nullptr && otherFunctor->m_id == m_id);
    }

    QT3D_FUNCTOR(TestTextureGenerator)
};

typedef QSharedPointer<TestTextureGenerator> TestTextureGeneratorPtr;

class TestTexturePrivate : public Qt3DRender::QAbstractTexturePrivate
{
public:
    int genId;
};

/**
 * @brief Test QTexture. Assign texture data functor if genId > 0.
 */
class TestTexture : public Qt3DRender::QAbstractTexture
{
public:
    TestTexture(int genId, Qt3DCore::QNode *p = nullptr)
        : QAbstractTexture(*new TestTexturePrivate(), p)
    {
        d_func()->genId = genId;
        if (genId > 0)
            d_func()->setDataFunctor(TestTextureGeneratorPtr::create(genId));
    }
private:
    Q_DECLARE_PRIVATE(TestTexture)
};

class TestSharedGLTexturePrivate : public Qt3DRender::QAbstractTexturePrivate
{
};

class TestSharedGLTexture : public Qt3DRender::QAbstractTexture
{
public:
    TestSharedGLTexture(int textureId, Qt3DCore::QNode *p = nullptr)
        : QAbstractTexture(*new TestSharedGLTexturePrivate(), p)
    {
        d_func()->m_sharedTextureId = textureId;
    }

private:
    Q_DECLARE_PRIVATE(TestSharedGLTexture)
};


/**
 * @brief Test QTextureImage
 */
class TestTextureImage : public Qt3DRender::QAbstractTextureImage
{
public:
    TestTextureImage(int genId, Qt3DCore::QNode *p = nullptr)
        : QAbstractTextureImage(p)
        , m_genId(genId)
    {
    }

    void updateGenerator()
    {
        Qt3DRender::QAbstractTextureImage::notifyDataGeneratorChanged();
    }

    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const
    {
        return Qt3DRender::QTextureImageDataGeneratorPtr(new TestImageDataGenerator(m_genId));
    }
protected:
    int m_genId;
};

class EmptyTextureImage : public Qt3DRender::QAbstractTextureImage
{
public:
    EmptyTextureImage(Qt3DCore::QNode *p = nullptr)
        : QAbstractTextureImage(p)
    {
    }

    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const
    {
        return {};
    }
};

class tst_RenderTextures : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

    Qt3DRender::QAbstractTexture *createQTexture(int genId,
                                                 const QVector<int> &imgGenIds,
                                                 bool genMipMaps)
    {
        TestTexture *tex = new TestTexture(genId);

        for (int imgGen : imgGenIds)
            tex->addTextureImage(new TestTextureImage(imgGen));
        tex->setGenerateMipMaps(genMipMaps);

        return tex;
    }

    Qt3DRender::QAbstractTexture *createQTextureWithTextureId(int textureId)
    {
        return new TestSharedGLTexture(textureId);
    }

    Qt3DRender::Render::Texture *createBackendTexture(Qt3DRender::QAbstractTexture *frontend,
                                                      Qt3DRender::Render::TextureManager *texMgr,
                                                      Qt3DRender::Render::TextureImageManager *texImgMgr,
                                                      Qt3DRender::Render::AbstractRenderer *renderer)
    {
        Qt3DRender::Render::Texture *backend = texMgr->getOrCreateResource(frontend->id());
        backend->setRenderer(renderer);
        simulateInitializationSync(frontend, backend);

        // create texture images
        for (const auto texImgFrontend : frontend->textureImages()) {
            // make sure TextureImageManager has backend node for this QTextureImage
            if (!texImgMgr->contains(texImgFrontend->id())) {
                Qt3DRender::Render::TextureImage *texImgBackend = texImgMgr->getOrCreateResource(texImgFrontend->id());
                texImgBackend->setRenderer(renderer);
                simulateInitializationSync(texImgFrontend, texImgBackend);
            }
        }

        return backend;
    }

private Q_SLOTS:

    void shouldCreateSameGLTextures()
    {
        QSKIP("Texture Sharing is now disabled");
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        renderer.setNodeManagers(mgrs.data());

        // GIVEN
        Qt3DRender::QAbstractTexture *tex1a = createQTexture(-1, {1,2}, true);
        Qt3DRender::QAbstractTexture *tex1b = createQTexture(-1, {1,2}, true);

        // WHEN
        Qt3DRender::Render::Texture *bt1a = createBackendTexture(tex1a,
                                                                 mgrs->textureManager(),
                                                                 mgrs->textureImageManager(),
                                                                 &renderer);
        Qt3DRender::Render::Texture *bt1b = createBackendTexture(tex1b,
                                                                 mgrs->textureManager(),
                                                                 mgrs->textureImageManager(),
                                                                 &renderer);
        renderer.updateTexture(bt1a);
        renderer.updateTexture(bt1b);

        // THEN
        QCOMPARE(renderer.glResourceManagers()->glTextureManager()->lookupResource(bt1a->peerId()),
                 renderer.glResourceManagers()->glTextureManager()->lookupResource(bt1b->peerId()));
        renderer.shutdown();
    }

    void shouldCreateDifferentGLTexturess()
    {
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        renderer.setNodeManagers(mgrs.data());

        // GIVEN
        QVector<Qt3DRender::QAbstractTexture*> textures;
        textures << createQTexture(-1, {1,2}, true);
        textures << createQTexture(-1, {1,2}, false);
        textures << createQTexture(1, {1,2}, true);
        textures << createQTexture(1, {1,2}, false);
        textures << createQTexture(1, {1,2,3}, true);
        textures << createQTexture(1, {1,2,3}, false);

        // WHEN
        QVector<Qt3DRender::Render::Texture*> backend;
        for (auto *t : textures) {
            Qt3DRender::Render::Texture *backendTexture = createBackendTexture(t,
                                                                               mgrs->textureManager(),
                                                                               mgrs->textureImageManager(),
                                                                               &renderer);
            backend.push_back(backendTexture);
            renderer.updateTexture(backendTexture);
        }

        // THEN

        // no 2 textures must be the same
        for (int i = 0; i < backend.size(); i++)
            for (int k = i+1; k < backend.size(); k++)
                QVERIFY(renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[i]->peerId()) !=
                        renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[k]->peerId()));

        QVector<Qt3DRender::Render::OpenGL::GLTexture *> glTextures;
        for (Qt3DRender::Render::Texture *t : backend)
            glTextures.push_back(renderer.glResourceManagers()->glTextureManager()->lookupResource(t->peerId()));

        // some texture generators must be the same
        QVERIFY(glTextures[0]->textureGenerator().data() == nullptr);
        QVERIFY(glTextures[1]->textureGenerator().data() == nullptr);
        QCOMPARE(*(glTextures[2]->textureGenerator()), *(glTextures[3]->textureGenerator()));

        // some images must be the same
        QCOMPARE(glTextures[0]->images(), glTextures[1]->images());
        QCOMPARE(glTextures[0]->images(), glTextures[2]->images());
        QCOMPARE(glTextures[0]->images(), glTextures[3]->images());
        QCOMPARE(glTextures[4]->images(), glTextures[5]->images());

        QCOMPARE(glTextures[0]->properties(), glTextures[2]->properties());
        QCOMPARE(glTextures[1]->properties(), glTextures[3]->properties());
        QVERIFY(glTextures[0]->properties() != glTextures[1]->properties());

        renderer.shutdown();
    }

    void shouldCreateDifferentGLTexturesWhenUsingSharedTextureIds()
    {
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        renderer.setNodeManagers(mgrs.data());

        Qt3DRender::Render::OpenGL::GLResourceManagers *glMgrs = renderer.glResourceManagers();

        // both texture having the same sharedTextureId
        {
            // GIVEN
            Qt3DRender::QAbstractTexture *tex1a = createQTextureWithTextureId(1);
            Qt3DRender::QAbstractTexture *tex1b = createQTextureWithTextureId(1);

            // WHEN
            Qt3DRender::Render::Texture *bt1 = createBackendTexture(tex1a,
                                                                    mgrs->textureManager(),
                                                                    mgrs->textureImageManager(),
                                                                    &renderer);
            Qt3DRender::Render::Texture *bt2 = createBackendTexture(tex1b,
                                                                    mgrs->textureManager(),
                                                                    mgrs->textureImageManager(),
                                                                    &renderer);
            // THEN
            QCOMPARE(bt1->sharedTextureId(), 1);
            QCOMPARE(bt2->sharedTextureId(), 1);

            // WHEN
            renderer.updateTexture(bt1);
            renderer.updateTexture(bt2);

            // THEN
            Qt3DRender::Render::OpenGL::GLTexture *glt1 = glMgrs->glTextureManager()->lookupResource(bt1->peerId());
            Qt3DRender::Render::OpenGL::GLTexture *glt2 = glMgrs->glTextureManager()->lookupResource(bt2->peerId());
            QVERIFY(glt1 != glt2);
            QCOMPARE(glt1->sharedTextureId(), bt1->sharedTextureId());
            QCOMPARE(glt2->sharedTextureId(), bt2->sharedTextureId());
        }

        // textures having a different sharedTextureId
        {
            // GIVEN
            Qt3DRender::QAbstractTexture *tex1a = createQTextureWithTextureId(1);
            Qt3DRender::QAbstractTexture *tex1b = createQTextureWithTextureId(2);

            // WHEN
            Qt3DRender::Render::Texture *bt1 = createBackendTexture(tex1a,
                                                                    mgrs->textureManager(),
                                                                    mgrs->textureImageManager(),
                                                                    &renderer);
            Qt3DRender::Render::Texture *bt2 = createBackendTexture(tex1b,
                                                                    mgrs->textureManager(),
                                                                    mgrs->textureImageManager(),
                                                                    &renderer);
            // THEN
            QCOMPARE(bt1->sharedTextureId(), 1);
            QCOMPARE(bt2->sharedTextureId(), 2);

            // WHEN
            renderer.updateTexture(bt1);
            renderer.updateTexture(bt2);

            // THEN
            Qt3DRender::Render::OpenGL::GLTexture *glt1 = glMgrs->glTextureManager()->lookupResource(bt1->peerId());
            Qt3DRender::Render::OpenGL::GLTexture *glt2 = glMgrs->glTextureManager()->lookupResource(bt2->peerId());
            QVERIFY(glt1 != glt2);
            QCOMPARE(glt1->sharedTextureId(), bt1->sharedTextureId());
            QCOMPARE(glt2->sharedTextureId(), bt2->sharedTextureId());
        }

        renderer.shutdown();
    }

    void generatorsShouldCreateSameData()
    {
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        renderer.setNodeManagers(mgrs.data());

        // GIVEN
        QVector<Qt3DRender::QAbstractTexture*> textures;
        textures << createQTexture(1, {1}, true);
        textures << createQTexture(2, {1,2}, true);
        textures << createQTexture(1, {1,2}, true);

        // WHEN
        QVector<Qt3DRender::Render::Texture*> backend;
        for (auto *t : textures) {
            Qt3DRender::Render::Texture *backendTexture = createBackendTexture(t,
                                                                               mgrs->textureManager(),
                                                                               mgrs->textureImageManager(),
                                                                               &renderer);
            backend.push_back(backendTexture);
            renderer.updateTexture(backendTexture);
        }

        Qt3DRender::QTextureImageDataGeneratorPtr idg1a = renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[0]->peerId())->images()[0].generator;
        Qt3DRender::QTextureImageDataGeneratorPtr idg1b = renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[1]->peerId())->images()[0].generator;
        Qt3DRender::QTextureImageDataGeneratorPtr idg2 = renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[1]->peerId())->images()[1].generator;
        Qt3DRender::QTextureGeneratorPtr tg1a = renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[0]->peerId())->textureGenerator();
        Qt3DRender::QTextureGeneratorPtr tg1b = renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[2]->peerId())->textureGenerator();
        Qt3DRender::QTextureGeneratorPtr tg2 = renderer.glResourceManagers()->glTextureManager()->lookupResource(backend[1]->peerId())->textureGenerator();

        // THEN
        QVERIFY(idg1a);
        QVERIFY(idg1b);
        QVERIFY(idg2);
        QVERIFY(tg1a);
        QVERIFY(tg1b);
        QVERIFY(tg2);

        QCOMPARE(*idg1a, *idg1b);
        QVERIFY(!(*idg1a == *idg2));
        QCOMPARE(*tg1a, *tg1b);
        QVERIFY(!(*tg1a == *tg2));

        renderer.shutdown();
    }

    void checkTextureImageInitialState()
    {
        // GIVEN
        Qt3DRender::Render::TextureImage img;

        // THEN
        QCOMPARE(img.layer(), 0);
        QCOMPARE(img.mipLevel(), 0);
        QCOMPARE(img.isDirty(), false);
        QCOMPARE(img.face(), Qt3DRender::QAbstractTexture::CubeMapPositiveX);
        QVERIFY(img.dataGenerator().isNull());
    }

    void checkTextureImageCleanupState()
    {
        // GIVEN
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        TestTextureImage img(1);
        img.setLayer(2);
        img.setMipLevel(3);

        // WHEN
        Qt3DRender::Render::TextureImage texImgBackend;
        texImgBackend.setRenderer(&renderer);
        simulateInitializationSync(&img, &texImgBackend);
        texImgBackend.cleanup();

        // THEN
        QCOMPARE(texImgBackend.isDirty(), false);
        QCOMPARE(texImgBackend.layer(), 0);
        QCOMPARE(texImgBackend.mipLevel(), 0);
        QCOMPARE(texImgBackend.face(), Qt3DRender::QAbstractTexture::CubeMapPositiveX);
        QVERIFY(texImgBackend.dataGenerator().isNull());

        renderer.shutdown();
    }

    void checkTextureImageInitializeFromPeer()
    {
        // GIVEN
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        TestTextureImage img(1);

        {
            // WHEN
            img.setLayer(2);
            img.setMipLevel(3);

            Qt3DRender::Render::TextureImage texImgBackend;
            texImgBackend.setRenderer(&renderer);
            simulateInitializationSync(&img, &texImgBackend);

            // THEN
            QCOMPARE(texImgBackend.isEnabled(), true);
            QCOMPARE(texImgBackend.isDirty(), true);
            QCOMPARE(texImgBackend.peerId(), img.id());
            QCOMPARE(texImgBackend.layer(), 2);
            QCOMPARE(texImgBackend.mipLevel(), 3);
            QCOMPARE(texImgBackend.face(), Qt3DRender::QAbstractTexture::CubeMapPositiveX);
            QVERIFY(!texImgBackend.dataGenerator().isNull());
        }

        {
            // WHEN
            img.setEnabled(false);

            Qt3DRender::Render::TextureImage texImgBackend;
            texImgBackend.setRenderer(&renderer);
            simulateInitializationSync(&img, &texImgBackend);

            // THEN
            QCOMPARE(texImgBackend.isEnabled(), false);
            QCOMPARE(texImgBackend.peerId(), img.id());
        }

        renderer.shutdown();
    }

    void checkTextureImageSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::TextureImage backendImage;
        TestTextureImage textureImage(1);
        TestRenderer renderer;
        backendImage.setRenderer(&renderer);
        simulateInitializationSync(&textureImage, &backendImage);

        {
            // WHEN
            const bool newValue = false;
            textureImage.setEnabled(newValue);
            backendImage.syncFromFrontEnd(&textureImage, false);

            // THEN
            QCOMPARE(backendImage.isEnabled(), newValue);
            QVERIFY(backendImage.isDirty());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendImage.unsetDirty();
        }

        {
            // WHEN
            const int newValue = 7;
            textureImage.setLayer(newValue);
            backendImage.syncFromFrontEnd(&textureImage, false);

            // THEN
            QCOMPARE(backendImage.layer(), newValue);
            QVERIFY(backendImage.isDirty());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendImage.unsetDirty();
        }

        {
            // WHEN
            const int newValue = 3;
            textureImage.setMipLevel(newValue);
            backendImage.syncFromFrontEnd(&textureImage, false);


            // THEN
            QCOMPARE(backendImage.mipLevel(), newValue);
            QVERIFY(backendImage.isDirty());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendImage.unsetDirty();
        }

        {
            // WHEN
            const Qt3DRender::QAbstractTexture::CubeMapFace newValue = Qt3DRender::QAbstractTexture::CubeMapNegativeX;
            textureImage.setFace(newValue);
            backendImage.syncFromFrontEnd(&textureImage, false);

            // THEN
            QCOMPARE(backendImage.face(), newValue);
            QVERIFY(backendImage.isDirty());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendImage.unsetDirty();
        }

        {
            // WHEN
            textureImage.updateGenerator();
            backendImage.syncFromFrontEnd(&textureImage, false);

            // THEN
            QVERIFY(backendImage.isDirty());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendImage.unsetDirty();

            // WHEN
            textureImage.updateGenerator();
            backendImage.syncFromFrontEnd(&textureImage, false);

            // THEN
            QVERIFY(backendImage.isDirty());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendImage.unsetDirty();
        }

        renderer.shutdown();
    }

    void checkTextureImageProperlyReleaseGenerator()
    {
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        Qt3DRender::Render::TextureManager *texMgr = mgrs->textureManager();
        Qt3DRender::Render::TextureImageManager *texImgMgr = mgrs->textureImageManager();
        renderer.setNodeManagers(mgrs.data());

        // GIVEN
        Qt3DRender::QAbstractTexture* frontendTexture = createQTexture(1, {1}, true);

        Qt3DRender::Render::Texture *backendTexture = texMgr->getOrCreateResource(frontendTexture->id());
        backendTexture->setRenderer(&renderer);
        simulateInitializationSync(frontendTexture, backendTexture);

        // THEN
        QCOMPARE(backendTexture->textureImageIds().size(), 1);
        QCOMPARE(frontendTexture->textureImages().size(), 1);

        // WHEN
        TestTextureImage *texImgFrontend = static_cast<TestTextureImage *>(frontendTexture->textureImages().first());
        const Qt3DRender::QTextureImageDataGeneratorPtr frontendGenerator = texImgFrontend->dataGenerator();

        // THEN
        QVERIFY(!frontendGenerator.isNull());

        // WHEN
        Qt3DRender::Render::TextureImage *texImgBackend = texImgMgr->getOrCreateResource(texImgFrontend->id());
        texImgBackend->setRenderer(&renderer);
        simulateInitializationSync(texImgFrontend, texImgBackend);

        // THEN
        qDebug() << frontendGenerator << texImgBackend->dataGenerator();
        const Qt3DRender::QTextureImageDataGeneratorPtr backendGenerator = texImgFrontend->dataGenerator();
        QVERIFY(frontendGenerator != backendGenerator);
        QVERIFY(*frontendGenerator == *backendGenerator);

        renderer.shutdown();
    }

    void checkTextureIsMarkedForDeletion()
    {
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        Qt3DRender::Render::TextureManager *texMgr = mgrs->textureManager();
        renderer.setNodeManagers(mgrs.data());

        Qt3DRender::Render::TextureFunctor textureBackendNodeMapper(&renderer,
                                                                    texMgr);

        // GIVEN
        Qt3DRender::QAbstractTexture* frontendTexture = createQTexture(1, {1}, true);

        Qt3DRender::Render::Texture *backendTexture = static_cast<Qt3DRender::Render::Texture *>(textureBackendNodeMapper.create(creationChange(frontendTexture)));
        backendTexture->setRenderer(&renderer);
        simulateInitializationSync(frontendTexture, backendTexture);

        // THEN
        QVERIFY(backendTexture != nullptr);
        QCOMPARE(texMgr->textureIdsToCleanup().size(), 0);

        QCOMPARE(texMgr->lookupResource(frontendTexture->id()), backendTexture);

        // WHEN
        textureBackendNodeMapper.destroy(frontendTexture->id());

        // THEN
        QCOMPARE(texMgr->textureIdsToCleanup().size(), 1);
        QCOMPARE(texMgr->textureIdsToCleanup().first(), frontendTexture->id());
        QVERIFY(texMgr->lookupResource(frontendTexture->id()) == nullptr);

        renderer.shutdown();
    }

    void checkTextureDestructionReconstructionWithinSameLoop()
    {
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        Qt3DRender::Render::TextureManager *texMgr = mgrs->textureManager();
        renderer.setNodeManagers(mgrs.data());

        Qt3DRender::Render::TextureFunctor textureBackendNodeMapper(&renderer,
                                                                    texMgr);

        // GIVEN
        Qt3DRender::QAbstractTexture* frontendTexture = createQTexture(1, {1}, true);

        Qt3DRender::Render::Texture *backendTexture = static_cast<Qt3DRender::Render::Texture *>(textureBackendNodeMapper.create(creationChange(frontendTexture)));
        backendTexture->setRenderer(&renderer);
        simulateInitializationSync(frontendTexture, backendTexture);

        // WHEN
        textureBackendNodeMapper.destroy(frontendTexture->id());

        // THEN
        QCOMPARE(texMgr->textureIdsToCleanup().size(), 1);
        QCOMPARE(texMgr->textureIdsToCleanup().first(), frontendTexture->id());
        QVERIFY(texMgr->lookupResource(frontendTexture->id()) == nullptr);

        // WHEN
        backendTexture = static_cast<Qt3DRender::Render::Texture *>(textureBackendNodeMapper.create(creationChange(frontendTexture)));
        backendTexture->setRenderer(&renderer);
        simulateInitializationSync(frontendTexture, backendTexture);

        // THEN
        QVERIFY(backendTexture != nullptr);
        QCOMPARE(texMgr->textureIdsToCleanup().size(), 0);
        QCOMPARE(texMgr->lookupResource(frontendTexture->id()), backendTexture);

        renderer.shutdown();
    }

    void checkTextureImageDirtinessPropagatesToTextures()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::Render::NodeManagers> mgrs(new Qt3DRender::Render::NodeManagers());
        Qt3DRender::Render::OpenGL::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        Qt3DRender::Render::TextureManager *texMgr = mgrs->textureManager();
        Qt3DRender::Render::TextureImageManager *texImgMgr = mgrs->textureImageManager();
        renderer.setNodeManagers(mgrs.data());

        Qt3DRender::QTexture2D *texture1 = new Qt3DRender::QTexture2D();
        TestTextureImage *image1 = new TestTextureImage(1);

        Qt3DRender::QTexture2D *texture2 = new Qt3DRender::QTexture2D();
        TestTextureImage *image2 = new TestTextureImage(2);

        Qt3DRender::QTexture2D *texture3 = new Qt3DRender::QTexture2D();

        texture1->addTextureImage(image1);
        texture2->addTextureImage(image2);
        texture3->addTextureImage(image1);
        texture3->addTextureImage(image2);

        Qt3DRender::Render::Texture *backendTexture1 = texMgr->getOrCreateResource(texture1->id());
        Qt3DRender::Render::Texture *backendTexture2 = texMgr->getOrCreateResource(texture2->id());
        Qt3DRender::Render::Texture *backendTexture3 = texMgr->getOrCreateResource(texture3->id());
        Qt3DRender::Render::TextureImage *backendImage1 = texImgMgr->getOrCreateResource(image1->id());
        Qt3DRender::Render::TextureImage *backendImage2 = texImgMgr->getOrCreateResource(image2->id());

        backendTexture1->setRenderer(&renderer);
        backendTexture2->setRenderer(&renderer);
        backendTexture3->setRenderer(&renderer);
        backendImage1->setRenderer(&renderer);
        backendImage2->setRenderer(&renderer);

        simulateInitializationSync(texture1, backendTexture1);
        simulateInitializationSync(texture2, backendTexture2);
        simulateInitializationSync(texture3, backendTexture3);
        simulateInitializationSync(image1, backendImage1);
        simulateInitializationSync(image2, backendImage2);

        // THEN
        QCOMPARE(backendTexture1->textureImageIds().size(), 1);
        QCOMPARE(backendTexture1->textureImageIds().first(), image1->id());
        QCOMPARE(backendTexture2->textureImageIds().size(), 1);
        QCOMPARE(backendTexture2->textureImageIds().first(), image2->id());
        QCOMPARE(backendTexture3->textureImageIds().size(), 2);
        QCOMPARE(backendTexture3->textureImageIds().first(), image1->id());
        QCOMPARE(backendTexture3->textureImageIds().last(), image2->id());

        // WHEN
        backendTexture1->unsetDirty();
        backendTexture2->unsetDirty();
        backendTexture3->unsetDirty();
        backendImage1->unsetDirty();
        backendImage2->unsetDirty();

        // THEN
        QVERIFY(backendTexture1->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture2->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture3->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);

        // WHEN
        renderer.textureGathererJob()->run();

        // THEN
        QVERIFY(backendTexture1->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture2->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture3->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);

        // WHEN
        // Make Image1 dirty
        image1->updateGenerator();
        backendImage1->syncFromFrontEnd(image1, false);

        // THEN
        QVERIFY(backendImage1->isDirty());
        QVERIFY(backendTexture1->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture2->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture3->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);

        // WHEN
        renderer.textureGathererJob()->run();

        // THEN
        QVERIFY(backendTexture1->dirtyFlags() & Qt3DRender::Render::Texture::DirtyImageGenerators);
        QVERIFY(backendTexture2->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture3->dirtyFlags() & Qt3DRender::Render::Texture::DirtyImageGenerators);

        backendImage1->unsetDirty();
        backendTexture1->unsetDirty();
        backendTexture3->unsetDirty();

        // WHEN
        image2->updateGenerator();
        backendImage2->syncFromFrontEnd(image2, false);

        // THEN
        QVERIFY(backendImage2->isDirty());
        QVERIFY(backendTexture1->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture2->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture3->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);

        // WHEN
        renderer.textureGathererJob()->run();

        QVERIFY(backendTexture1->dirtyFlags() == Qt3DRender::Render::Texture::NotDirty);
        QVERIFY(backendTexture2->dirtyFlags() & Qt3DRender::Render::Texture::DirtyImageGenerators);
        QVERIFY(backendTexture3->dirtyFlags() & Qt3DRender::Render::Texture::DirtyImageGenerators);

        renderer.shutdown();
    }
};

QTEST_MAIN(tst_RenderTextures)

#include "tst_textures.moc"
