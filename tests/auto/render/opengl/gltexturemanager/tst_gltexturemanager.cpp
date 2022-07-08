// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DRender/QTextureImageDataGenerator>
#include <glresourcemanagers_p.h>
#include <glresourcemanagers_p.h>
#include <gltexture_p.h>
#include <Qt3DCore/qnodeid.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class TestImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator
{
    int m_id;
public:
    TestImageDataGenerator(int id) : m_id(id) {}

    Qt3DRender::QTextureImageDataPtr operator ()() override {
        return Qt3DRender::QTextureImageDataPtr::create();
    }

    bool operator ==(const Qt3DRender::QTextureImageDataGenerator &other) const override {
        const TestImageDataGenerator *otherFunctor = Qt3DCore::functor_cast<TestImageDataGenerator>(&other);
        return (otherFunctor != nullptr && otherFunctor->m_id == m_id);
    }

    QT3D_FUNCTOR(TestImageDataGenerator)
};

using ImageDataGeneratorPtr = QSharedPointer<TestImageDataGenerator>;

class tst_GLTextureManager : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::OpenGL::GLTextureManager manager;
        const Qt3DCore::QNodeId id = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::OpenGL::GLTexture *t = manager.getOrCreateResource(id);

        // THEN
        QVERIFY(t != nullptr);
        QVERIFY(!t->isDirty());
        QVERIFY(!t->hasTextureData());
        QVERIFY(!t->hasImagesData());
        QVERIFY(t->dataGenerator().isNull());
        QVERIFY(t->textureDataUpdates().empty());
        QCOMPARE(t->dirtyFlags(),  Qt3DRender::Render::OpenGL::GLTexture::None);
        QVERIFY(t->images().empty());

        QCOMPARE(t, manager.getOrCreateResource(id));
    }

    void checkCleanup()
    {
        // GIVEN
        Qt3DRender::Render::OpenGL::GLTextureManager manager;
        const Qt3DCore::QNodeId id = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::OpenGL::GLTexture *t = manager.getOrCreateResource(id);

        // WHEN
        Qt3DRender::Render::OpenGL::GLTexture::Image img = {
            ImageDataGeneratorPtr::create(0),
            0, 0,
            Qt3DRender::QAbstractTexture::AllFaces
        };
        t->setImages({img});

        // THEN
        QCOMPARE(t->images().size(), 1U);
        QCOMPARE(t->dirtyFlags(),  Qt3DRender::Render::OpenGL::GLTexture::TextureImageData);

        // WHEN
        manager.releaseResource(id);

        // THEN -> Cleanup should have been called
        Qt3DRender::Render::OpenGL::GLTexture *t2 = manager.getOrCreateResource(id);
        QCOMPARE(t, t2);
        QVERIFY(t->images().empty());
        QCOMPARE(t->dirtyFlags(),  Qt3DRender::Render::OpenGL::GLTexture::None);
    }
};

QTEST_APPLESS_MAIN(tst_GLTextureManager)

#include "tst_gltexturemanager.moc"
