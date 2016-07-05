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

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <qtextureimagedatagenerator.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qtextureimage.h>
#include <Qt3DRender/qtexturedata.h>

#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/textureimage_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/gltexturemanager_p.h>
#include <Qt3DRender/private/gltexture_p.h>
#include <Qt3DRender/private/qtexture_p.h>

/**
 * @brief Dummy QTextureImageDataGenerator
 */
class TestImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator
{
    int m_id;
public:
    TestImageDataGenerator(int id) : m_id(id) {}

    Qt3DRender::QTextureImageDataPtr operator ()() {
        return Qt3DRender::QTextureImageDataPtr::create();
    }

    bool operator ==(const Qt3DRender::QTextureImageDataGenerator &other) const {
        const TestImageDataGenerator *otherFunctor = functor_cast<TestImageDataGenerator>(&other);
        return (otherFunctor != Q_NULLPTR && otherFunctor->m_id == m_id);
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

    Qt3DRender::QTextureDataPtr operator ()() {
        return Qt3DRender::QTextureDataPtr::create();
    }

    bool operator ==(const Qt3DRender::QTextureGenerator &other) const {
        const TestTextureGenerator *otherFunctor = functor_cast<TestTextureGenerator>(&other);
        return (otherFunctor != Q_NULLPTR && otherFunctor->m_id == m_id);
    }

    QT3D_FUNCTOR(TestTextureGenerator)
};

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
            d_func()->m_dataFunctor.reset(new TestTextureGenerator(genId));
    }
private:
    Q_DECLARE_PRIVATE(TestTexture)
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

protected:
    int m_genId;
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const {
        return Qt3DRender::QTextureImageDataGeneratorPtr(new TestImageDataGenerator(m_genId));
    }
};

class tst_RenderTextures : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

    Qt3DRender::QAbstractTexture *createQTexture(int genId, const QVector<int> &imgGenIds, bool genMipMaps) {
        TestTexture *tex = new TestTexture(genId);

        for (int imgGen : imgGenIds)
            tex->addTextureImage(new TestTextureImage(imgGen));
        tex->setGenerateMipMaps(genMipMaps);

        return tex;
    }

    Qt3DRender::Render::Texture *createBackendTexture(Qt3DRender::QAbstractTexture *frontend, Qt3DRender::Render::TextureManager *texMgr, Qt3DRender::Render::GLTextureManager *glTexMgr, Qt3DRender::Render::TextureImageManager *texImgMgr) {
        Qt3DRender::Render::Texture *backend = texMgr->getOrCreateResource(frontend->id());
        backend->setTextureManager(glTexMgr);
        backend->setTextureImageManager(texImgMgr);
        simulateInitialization(frontend, backend);

        // create texture images
        for (const auto texImgFrontend : frontend->textureImages()) {
            // make sure TextureImageManager has backend node for this QTextureImage
            if (!texImgMgr->contains(texImgFrontend->id())) {
                Qt3DRender::Render::TextureImage *texImgBackend = texImgMgr->getOrCreateResource(texImgFrontend->id());
                texImgBackend->setTextureManager(texMgr);
                simulateInitialization(texImgFrontend, texImgBackend);
            }
            backend->addTextureImage(texImgFrontend->id());
        }

        return backend;
    }

private Q_SLOTS:

    void shouldCreateSameGLTextures() {
        Qt3DRender::Render::NodeManagers *mgrs = new Qt3DRender::Render::NodeManagers();

        // GIVEN
        auto *tex1a = createQTexture(-1, {1,2}, true);
        auto *tex1b = createQTexture(-1, {1,2}, true);

        // WHEN
        auto *bt1a = createBackendTexture(tex1a, mgrs->textureManager(), mgrs->glTextureManager(), mgrs->textureImageManager());
        auto *bt1b = createBackendTexture(tex1b, mgrs->textureManager(), mgrs->glTextureManager(), mgrs->textureImageManager());

        // THEN
        QCOMPARE(bt1a->texture(), bt1b->texture());
    }

    void shouldCreateDifferentGLTexturess() {
        Qt3DRender::Render::NodeManagers *mgrs = new Qt3DRender::Render::NodeManagers();

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
        for (auto *t : textures)
            backend << createBackendTexture(t, mgrs->textureManager(), mgrs->glTextureManager(), mgrs->textureImageManager());

        // THEN

        // no 2 textures must be the same
        for (int i = 0; i < backend.size(); i++)
            for (int k = i+1; k < backend.size(); k++)
                QVERIFY(backend[i]->texture() != backend[k]->texture());

        // some texture generators must be the same
        QVERIFY(backend[0]->texture()->textureGenerator().data() == nullptr);
        QVERIFY(backend[1]->texture()->textureGenerator().data() == nullptr);
        QCOMPARE(*backend[2]->texture()->textureGenerator(), *backend[3]->texture()->textureGenerator());

        // some images must be the same
        QCOMPARE(backend[0]->texture()->images(), backend[1]->texture()->images());
        QCOMPARE(backend[0]->texture()->images(), backend[2]->texture()->images());
        QCOMPARE(backend[0]->texture()->images(), backend[3]->texture()->images());
        QCOMPARE(backend[4]->texture()->images(), backend[5]->texture()->images());

        QCOMPARE(backend[0]->texture()->properties(), backend[2]->texture()->properties());
        QCOMPARE(backend[1]->texture()->properties(), backend[3]->texture()->properties());
        QVERIFY(backend[0]->texture()->properties() != backend[1]->texture()->properties());
    }

    void generatorsShouldCreateSameData() {
        Qt3DRender::Render::NodeManagers *mgrs = new Qt3DRender::Render::NodeManagers();

        // GIVEN
        QVector<Qt3DRender::QAbstractTexture*> textures;
        textures << createQTexture(1, {1}, true);
        textures << createQTexture(2, {1,2}, true);
        textures << createQTexture(1, {1,2}, true);

        // WHEN
        QVector<Qt3DRender::Render::Texture*> backend;
        for (auto *t : textures)
            backend << createBackendTexture(t, mgrs->textureManager(), mgrs->glTextureManager(), mgrs->textureImageManager());

        Qt3DRender::QTextureImageDataGeneratorPtr idg1a = backend[0]->texture()->images()[0].generator;
        Qt3DRender::QTextureImageDataGeneratorPtr idg1b = backend[1]->texture()->images()[0].generator;
        Qt3DRender::QTextureImageDataGeneratorPtr idg2 = backend[1]->texture()->images()[1].generator;
        Qt3DRender::QTextureGeneratorPtr tg1a = backend[0]->texture()->textureGenerator();
        Qt3DRender::QTextureGeneratorPtr tg1b = backend[2]->texture()->textureGenerator();
        Qt3DRender::QTextureGeneratorPtr tg2 = backend[1]->texture()->textureGenerator();

        // THEN
        QCOMPARE(*idg1a, *idg1b);
        QVERIFY(!(*idg1a == *idg2));
        QCOMPARE(*tg1a, *tg1b);
        QVERIFY(!(*tg1a == *tg2));
        Qt3DRender::Render::TextureImageDataManager *imgDataMgr = mgrs->textureImageDataManager();
        Qt3DRender::Render::TextureDataManager *texDataMgr = mgrs->textureDataManager();
        QVERIFY(imgDataMgr->getData(idg1a) == nullptr);
        QVERIFY(imgDataMgr->getData(idg2) == nullptr);
        QVERIFY(texDataMgr->getData(tg1a) == nullptr);
        QVERIFY(texDataMgr->getData(tg2) == nullptr);

        // WHEN
        for (const auto gen : imgDataMgr->pendingGenerators())
            imgDataMgr->assignData(gen, (*gen)());
        for (const auto gen : texDataMgr->pendingGenerators())
            texDataMgr->assignData(gen, (*gen)());

        // THEN
        QVERIFY(imgDataMgr->getData(idg1a) != nullptr);
        QVERIFY(imgDataMgr->getData(idg1b) != nullptr);
        QVERIFY(imgDataMgr->getData(idg2) != nullptr);
        QVERIFY(texDataMgr->getData(tg1a) != nullptr);
        QVERIFY(texDataMgr->getData(tg1b) != nullptr);
        QVERIFY(texDataMgr->getData(tg2) != nullptr);

        QCOMPARE(imgDataMgr->getData(idg1a), imgDataMgr->getData(idg1b));
        QVERIFY(imgDataMgr->getData(idg1a) != imgDataMgr->getData(idg2));

        QCOMPARE(texDataMgr->getData(tg1a), texDataMgr->getData(tg1b));
        QVERIFY(texDataMgr->getData(tg1a) != texDataMgr->getData(tg2));
    }

};

QTEST_APPLESS_MAIN(tst_RenderTextures)

#include "tst_textures.moc"
