/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRender/private/gltexture_p.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtexturegenerator.h>
#include <Qt3DRender/private/qtexture_p.h>
#include <Qt3DRender/private/qtextureimage_p.h>
#include <QSurfaceFormat>
#include <QWindow>
#include <QUrl>
#include <QOpenGLContext>
#include <cstring>

using namespace Qt3DRender;
using namespace Qt3DRender::Render;

namespace {

QTextureDataUpdate generateDataUpdate()
{
    QByteArray rawPixelData;
    rawPixelData.resize(512 * 4 * sizeof(uchar));
    uchar *pixels = reinterpret_cast<uchar *>(rawPixelData.data());
    std::memset(pixels, 128U, 512 * 4 * sizeof(uchar));

    // QTextureImageData hold our raw content and size of subcontent
    Qt3DRender::QTextureImageDataPtr imageData = Qt3DRender::QTextureImageDataPtr::create();
    imageData->setWidth(512);
    imageData->setHeight(1);
    imageData->setMipLevels(1);
    imageData->setLayers(1);
    // We upload the 4 components of an RGBA pixel as 4 uchar
    imageData->setPixelFormat(QOpenGLTexture::RGBA);
    imageData->setPixelType(QOpenGLTexture::UInt8);
    imageData->setData(rawPixelData, 4, false);

    Qt3DRender::QTextureDataUpdate update;
    update.setX(0);
    update.setY(10);
    update.setMipLevel(0);
    update.setData(imageData);

    return update;
}

} // anonymous

class FakeTexture2DGenerator : public QTextureGenerator
{
public:
    QT3D_FUNCTOR(FakeTexture2DGenerator)

    QTextureDataPtr operator ()() override
    {
        QTextureDataPtr generatedData = QTextureDataPtr::create();
        generatedData->setTarget(QAbstractTexture::Target2D);
#ifndef QT_OPENGL_ES_2
        generatedData->setFormat(QAbstractTexture::RGBA8_UNorm);
#else
        generatedData->setFormat(QAbstractTexture::RGBAFormat);
#endif
        generatedData->setWidth(512);
        generatedData->setHeight(512);
        generatedData->setDepth(1);
        generatedData->setLayers(1);
        return generatedData;
    }

    bool operator ==(const QTextureGenerator &) const override
    {
        return true;
    }
};

class FakeTexture2DGeneratorAutomatic : public QTextureGenerator
{
public:
    QT3D_FUNCTOR(FakeTexture2DGenerator)

    QTextureDataPtr operator ()() override
    {
        QTextureDataPtr generatedData = QTextureDataPtr::create();
        generatedData->setTarget(QAbstractTexture::TargetAutomatic);
#ifndef QT_OPENGL_ES_2
        generatedData->setFormat(QAbstractTexture::RGBA8_UNorm);
#else
        generatedData->setFormat(QAbstractTexture::RGBAFormat);
#endif
        generatedData->setWidth(512);
        generatedData->setHeight(512);
        generatedData->setDepth(1);
        generatedData->setLayers(1);
        return generatedData;
    }

    bool operator ==(const QTextureGenerator &) const override
    {
        return true;
    }
};

class FakeDownloadTexture2DGenerator : public QTextureGenerator
{
public:
    QT3D_FUNCTOR(FakeDownloadTexture2DGenerator)

    QTextureDataPtr operator ()() override
    {
        if (m_hasBeenDownloaded) {
            generatedData->setTarget(QAbstractTexture::Target2D);
            generatedData->setFormat(QAbstractTexture::RGBA8_UNorm);
            generatedData->setWidth(512);
            generatedData->setHeight(512);
            generatedData->setDepth(1);
            generatedData->setLayers(1);
        } else {
            generatedData->setTarget(QAbstractTexture::TargetAutomatic);
        }
        return generatedData;
    }

    bool operator ==(const QTextureGenerator &) const override
    {
        return true;
    }

    void setHasBeenDownloaded(bool downloaded)
    {
        m_hasBeenDownloaded = downloaded;
    }

private:
    QTextureDataPtr generatedData = QTextureDataPtr::create();
    bool m_hasBeenDownloaded = false;
};



class tst_GLTexture : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QWindow> m_window;
    QOpenGLContext m_glContext;

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<TextureProperties>("Qt3DRender::Render::TexturePropertiers");
    }

    void init()
    {
        m_window.reset(new QWindow);
        m_window->setSurfaceType(QWindow::OpenGLSurface);
        m_window->setGeometry(0, 0, 10, 10);
        m_window->create();

        if (!m_glContext.create()) {
            qWarning() << "Failed to create OpenGL context";
            return;
        }

        if (!m_glContext.makeCurrent(m_window.data())) {
            qWarning() << "Failed to make OpenGL context current";
            return;
        }
    }

    void cleanup()
    {
        m_glContext.doneCurrent();
    }

    void checkDontCreateInternalTextureIfNothingSpecified()
    {
        // GIVEN
        GLTexture texture;

        // THEN
        QVERIFY(texture.getGLTexture() == nullptr);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN -> shouldn't have created a texture since format and target
        // were not specified
        QVERIFY(texture.getGLTexture() == nullptr);

        // Cleanup
        texture.destroy();
    }

    void checkCreatesInternalTexture()
    {
        // GIVEN
        GLTexture texture;

        // WHEN
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_SNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        texture.setProperties(props);

        // THEN
        QCOMPARE(texture.dirtyFlags(), GLTexture::Properties);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTexture = texture.getGLTexture();
        QCOMPARE(internalTexture->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTexture->width(), 1);
        QCOMPARE(internalTexture->height(), 1);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());

        // Cleanup
        texture.destroy();
    }

    void checkShouldnotCreateTextureIfTargetIsAutomaticOrFormatUnspecified()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;

        props.target = QAbstractTexture::TargetAutomatic;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_SNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        // WHEN
        texture.setProperties(props);

        // THEN
        QCOMPARE(texture.dirtyFlags(), GLTexture::Properties);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN  -> shouldn't have created a texture since target is automatic
        QVERIFY(texture.getGLTexture() == nullptr);
        QCOMPARE(texture.dirtyFlags(), GLTexture::Properties);

        // WHEN
        props.target = QAbstractTexture::Target2D;
        props.format = QAbstractTexture::NoFormat;
        texture.setProperties(props);
        texture.createOrUpdateGLTexture();

        // THEN  -> shouldn't have created a texture since format is not specified
        QVERIFY(texture.getGLTexture() == nullptr);
        QCOMPARE(texture.dirtyFlags(), GLTexture::Properties);

        // WHEN
        props.target = QAbstractTexture::Target2D;
        props.format = QAbstractTexture::Automatic;
        texture.setProperties(props);
        texture.createOrUpdateGLTexture();

        // THEN  -> shouldn't have created a texture since format is not specified
        QVERIFY(texture.getGLTexture() == nullptr);
        QCOMPARE(texture.dirtyFlags(), GLTexture::Properties);

        // WHEN
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_SNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        texture.setProperties(props);
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTexture = texture.getGLTexture();
        QCOMPARE(internalTexture->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTexture->width(), 1);
        QCOMPARE(internalTexture->height(), 1);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());

        // Cleanup
        texture.destroy();
    }

    void checkCreateTextureWithDataGenerator()
    {
        // GIVEN
        GLTexture texture;
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTex = texture.getGLTexture();
        QVERIFY(internalTex == nullptr);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());

        // WHEN
        auto genTex2D = QSharedPointer<FakeTexture2DGenerator>::create();
        texture.setGenerator(genTex2D);
        texture.createOrUpdateGLTexture();

        // THEN
        QVERIFY(texture.getGLTexture() != nullptr);
        QVERIFY(texture.getGLTexture() != internalTex);

        internalTex = texture.getGLTexture();
        QCOMPARE(internalTex->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTex->width(), 512);
        QCOMPARE(internalTex->height(), 512);

        QVERIFY(texture.hasTextureData());
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasImagesData());

        // Cleanup
        texture.destroy();
    }

    void checkHandlesDelayedReceptionOfDataGenerator()
    {
        // GIVEN
        GLTexture texture;

        // WHEN
        auto genDownloadTex2D = QSharedPointer<FakeDownloadTexture2DGenerator>::create();
        texture.setGenerator(genDownloadTex2D);

        // THEN
        QVERIFY(!texture.hasTextureData());
        QVERIFY(texture.isDirty());

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN -> shouldn't have created internal texture since target is automatic
        // but generator should have run to return a QTextureDataPtr
        QVERIFY(texture.getGLTexture() == nullptr);
        QVERIFY(texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());

        // WHEN
        genDownloadTex2D->setHasBeenDownloaded(true);
        texture.setGenerator(genDownloadTex2D);

        // THEN
        QVERIFY(texture.getGLTexture() == nullptr);
        QVERIFY(!texture.hasTextureData());
        QVERIFY(texture.isDirty());

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN -> Should have created the texture has target shouldn't be automatic
        QVERIFY(texture.hasTextureData());
        QVERIFY(!texture.isDirty());
        QOpenGLTexture *internalTex = texture.getGLTexture();
        QVERIFY(internalTex != nullptr);
        QCOMPARE(internalTex->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTex->width(), 512);
        QCOMPARE(internalTex->height(), 512);

        // Cleanup
        texture.destroy();
    }

    void checkTakesTargetFromGeneratorIfTargetNotSpecified()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;
        props.target = QAbstractTexture::TargetAutomatic;

        // WHEN
        auto genDownloadTex2D = QSharedPointer<FakeTexture2DGenerator>::create();
        texture.setGenerator(genDownloadTex2D);
        texture.setProperties(props);

        // THEN
        QVERIFY(!texture.hasTextureData());
        QVERIFY(texture.isDirty());

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN -> Should have created the texture has target shouldn't be automatic
        QVERIFY(texture.hasTextureData());
        QVERIFY(!texture.isDirty());
        QOpenGLTexture *internalTex = texture.getGLTexture();
        QVERIFY(internalTex != nullptr);
        QCOMPARE(internalTex->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTex->width(), 512);
        QCOMPARE(internalTex->height(), 512);

        // Cleanup
        texture.destroy();
    }

    void checkKeepsSetTargetIfGeneratorTargetNotSpecified()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;

        // WHEN
        auto genDownloadTex2D = QSharedPointer<FakeTexture2DGeneratorAutomatic>::create();
        texture.setGenerator(genDownloadTex2D);
        texture.setProperties(props);

        // THEN
        QVERIFY(!texture.hasTextureData());
        QVERIFY(texture.isDirty());

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN -> Should have created the texture has target shouldn't be automatic
        QVERIFY(texture.hasTextureData());
        QVERIFY(!texture.isDirty());
        QOpenGLTexture *internalTex = texture.getGLTexture();
        QVERIFY(internalTex != nullptr);
        QCOMPARE(internalTex->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTex->width(), 512);
        QCOMPARE(internalTex->height(), 512);

        // Cleanup
        texture.destroy();
    }

    void checkDontCreateTextureIfGeneratorAndTargetAreAutomatic()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;
        props.target = QAbstractTexture::TargetAutomatic;

        // WHEN
        auto genDownloadTex2D = QSharedPointer<FakeTexture2DGeneratorAutomatic>::create();
        texture.setGenerator(genDownloadTex2D);
        texture.setProperties(props);

        // THEN
        QVERIFY(!texture.hasTextureData());
        QVERIFY(texture.isDirty());

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN -> Should have created the texture has target shouldn't be automatic
        QVERIFY(!texture.hasTextureData());
        QVERIFY(texture.isDirty());
        QVERIFY(texture.getGLTexture() == nullptr);

        // Cleanup
        texture.destroy();
    }

    void checkRecreatesTextureOnPropertyChanged()
    {
        // GIVEN
        GLTexture texture;

        // WHEN
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_SNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        texture.setProperties(props);

        // THEN
        QCOMPARE(texture.dirtyFlags(), GLTexture::Properties);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTexture = texture.getGLTexture();
        QCOMPARE(internalTexture->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTexture->width(), 1);
        QCOMPARE(internalTexture->height(), 1);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());

        // WHEN
        props.width = 256;
        props.height = 256;
        texture.setProperties(props);
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTexture2 = texture.getGLTexture();
        QCOMPARE(internalTexture2->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTexture2->width(), 256);
        QCOMPARE(internalTexture2->height(), 256);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());
        QVERIFY(texture.wasTextureRecreated());

        // Technically pointer should be different but that's up to the system
        // so can't really be tested

        // Cleanup
        texture.destroy();
    }

    void checkDoesntRecreateTextureOnParameterChanged()
    {
        // GIVEN
        GLTexture texture;

        // WHEN
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_SNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        texture.setProperties(props);

        TextureParameters params;
        params.magnificationFilter = QAbstractTexture::Linear;
        params.minificationFilter = QAbstractTexture::Linear;
        texture.setParameters(params);

        // THEN
        QCOMPARE(texture.dirtyFlags(), GLTexture::Properties|GLTexture::Parameters);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTexture = texture.getGLTexture();
        QCOMPARE(internalTexture->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTexture->width(), 1);
        QCOMPARE(internalTexture->height(), 1);
        QCOMPARE(internalTexture->minificationFilter(), QOpenGLTexture::Linear);
        QCOMPARE(internalTexture->magnificationFilter(), QOpenGLTexture::Linear);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());

        // WHEN
        params.magnificationFilter = QAbstractTexture::Nearest;
        params.minificationFilter = QAbstractTexture::Nearest;
        texture.setParameters(params);

        // THEN
        QCOMPARE(texture.dirtyFlags(),GLTexture::Parameters);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTexture2 = texture.getGLTexture();
        QCOMPARE(internalTexture->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTexture->width(), 1);
        QCOMPARE(internalTexture->height(), 1);
        QCOMPARE(internalTexture->minificationFilter(), QOpenGLTexture::Nearest);
        QCOMPARE(internalTexture->magnificationFilter(), QOpenGLTexture::Nearest);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.hasImagesData());
        QCOMPARE(internalTexture, internalTexture2);
        QVERIFY(!texture.wasTextureRecreated());

        // Cleanup
        texture.destroy();
    }

    void checkDestroy()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_SNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        texture.setProperties(props);

        // WHEN
        texture.createOrUpdateGLTexture();
        texture.setGenerator(QSharedPointer<FakeTexture2DGenerator>::create());
        QVector<GLTexture::Image> images;
        QImageTextureDataFunctorPtr gen = QImageTextureDataFunctorPtr::create(QUrl("qrc:/image.jpg"), true);
        images.push_back({gen, 1, 0, QAbstractTexture::CubeMapPositiveX});
        texture.setImages(images);

        // THEN
        QVERIFY(texture.getGLTexture() != nullptr);

        // WHEN
        texture.destroy();

        // THEN
        QVERIFY(texture.getGLTexture() == nullptr);
        QVERIFY(!texture.isDirty());
        QCOMPARE(texture.properties(), TextureProperties());
        QCOMPARE(texture.parameters(), TextureParameters());
        QCOMPARE(texture.images().size(), 0);
        QVERIFY(!texture.hasImagesData());
        QVERIFY(!texture.hasTextureData());
        QVERIFY(!texture.dataGenerator());
    }

    void checkRecreateTextureOnNewImageDataChanged()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_UNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        props.width = 1;
        props.height = 1;
        texture.setProperties(props);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QVERIFY(texture.getGLTexture() != nullptr);
        QVERIFY(!texture.isDirty());

        // WHEN
        texture.setGenerator(QSharedPointer<FakeTexture2DGenerator>::create());
        QVector<GLTexture::Image> images;
        // Test image has a size of 512/512
        QImageTextureDataFunctorPtr gen = QImageTextureDataFunctorPtr::create(QUrl("qrc:/image.jpg"), true);
        images.push_back({gen, 1, 0, QAbstractTexture::CubeMapPositiveX});
        texture.setImages(images);

        // THEN
        QCOMPARE(texture.dirtyFlags(), GLTexture::TextureImageData|GLTexture::TextureData);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QOpenGLTexture *internalTexture = texture.getGLTexture();
        QCOMPARE(internalTexture->target(), QOpenGLTexture::Target2D);
        QCOMPARE(internalTexture->width(), 512);
        QCOMPARE(internalTexture->height(), 512);
        QVERIFY(!texture.isDirty());
        QVERIFY(texture.hasTextureData());
        QVERIFY(texture.wasTextureRecreated());

        // Cleanup
        texture.destroy();
    }

    void checkDoesntRecreateTextureOnNewTextureDataUpdate()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_UNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        props.width = 512;
        props.height = 512;
        texture.setProperties(props);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QVERIFY(texture.getGLTexture() != nullptr);
        QVERIFY(!texture.isDirty());

        // WHEN
        QByteArray rawPixelData;
        rawPixelData.resize(512 * 4 * sizeof(uchar));
        uchar *pixels = reinterpret_cast<uchar *>(rawPixelData.data());
        std::memset(pixels, 128U, 512 * 4 * sizeof(uchar));

        // QTextureImageData hold our raw content and size of subcontent
        Qt3DRender::QTextureImageDataPtr imageData = Qt3DRender::QTextureImageDataPtr::create();
        imageData->setWidth(512);
        imageData->setHeight(1);
        imageData->setMipLevels(1);
        imageData->setLayers(1);
        // We upload the 4 components of an RGBA pixel as 4 uchar
        imageData->setPixelFormat(QOpenGLTexture::RGBA);
        imageData->setPixelType(QOpenGLTexture::UInt8);
        imageData->setData(rawPixelData, 4, false);

        Qt3DRender::QTextureDataUpdate update;
        update.setX(0);
        update.setY(10);
        update.setMipLevel(0);
        update.setData(imageData);

        texture.addTextureDataUpdates({update});

        // THEN
        QCOMPARE(texture.dirtyFlags(), GLTexture::TextureData);

        // WHEN
        texture.createOrUpdateGLTexture();
        QVERIFY(texture.getGLTexture() != nullptr);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.wasTextureRecreated());

        // Cleanup
        texture.destroy();
    }

    void checkDoesntCrashWhenSendingIncompatibleTextureDataUpdate_data()
    {
        QTest::addColumn<TextureProperties>("properties");
        QTest::addColumn<Qt3DRender::QTextureDataUpdate>("update");

        QTextureDataUpdate update;
        TextureProperties props;
        props.target = QAbstractTexture::Target2D;
#ifndef QT_OPENGL_ES_2
        props.format = QAbstractTexture::RGBA8_UNorm;
#else
        props.format = QAbstractTexture::RGBAFormat;
#endif
        props.width = 512;
        props.height = 512;
        QTest::newRow("empty") << props << update;

        update = generateDataUpdate();
        update.setMipLevel(4);
        QTest::newRow("invalid_mip_level") << props << update;

        update.setX(512);
        QTest::newRow("out_of_bound_x") << props << update;

        update.setX(0);
        update.setY(512);
        QTest::newRow("out_of_bound_y") << props << update;

#ifndef QT_OPENGL_ES_2
        update.setX(0);
        update.setY(0);
        update.setZ(512);
        props.target = QAbstractTexture::Target3D;
        props.depth = 512;
        QTest::newRow("out_of_bound_z") << props << update;
#endif
        update.setY(0);
        update.setZ(0);
        update.setX(256);
        update.data()->setWidth(512);
        QTest::newRow("out_of_bound_width") << props << update;

        update.setX(0);
        update.setY(256);
        update.setZ(0);
        update.data()->setHeight(512);
        QTest::newRow("out_of_bound_height") << props << update;

#ifndef QT_OPENGL_ES_2
        update.setX(0);
        update.setY(0);
        update.setZ(256);
        update.data()->setDepth(512);
        QTest::newRow("out_of_bound_depth") << props << update;

        update.setX(0);
        update.setY(0);
        update.setZ(0);
        update.data()->setDepth(0);
        update.setLayer(8);
        props.target = QAbstractTexture::Target2DArray;
        props.layers = 4;
        QTest::newRow("out_of_bound_layer") << props << update;
#endif
    }


    void checkDoesntCrashWhenSendingIncompatibleTextureDataUpdate()
    {
        // GIVEN
        GLTexture texture;
        QFETCH(TextureProperties, properties);
        QFETCH(QTextureDataUpdate, update);
        texture.setProperties(properties);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QVERIFY(texture.getGLTexture() != nullptr);
        QVERIFY(!texture.isDirty());

        // WHEN
        texture.addTextureDataUpdates({update});

        // THEN
        QCOMPARE(texture.dirtyFlags(), GLTexture::TextureData);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN -> shouldn't crash
        QVERIFY(texture.getGLTexture() != nullptr);
        QVERIFY(!texture.isDirty());
        QVERIFY(!texture.wasTextureRecreated());

        // Cleanup
        texture.destroy();
    }

    void checkPropertiesAfterLoadTextureDataFromImages()
    {
        // GIVEN
        GLTexture texture;
        TextureProperties props;
        props.target = QAbstractTexture::TargetCubeMap;
        props.format = QAbstractTexture::Automatic;
        props.width = 1;
        props.height = 1;
        texture.setProperties(props);
        QVector<GLTexture::Image> images;
        // test a image texture data generator whose url is invalid
        QImageTextureDataFunctorPtr gen = QImageTextureDataFunctorPtr::create(QUrl(), true);
        images.push_back({gen, 0, 0, QAbstractTexture::CubeMapPositiveX});
        texture.setImages(images);

        // WHEN
        texture.createOrUpdateGLTexture();

        // THEN
        QCOMPARE(texture.properties().format, QAbstractTexture::Automatic);

        // WHEN
        // test a image texture data generator whose url is valid
        gen = QImageTextureDataFunctorPtr::create(QUrl("qrc:/image.jpg"), true);
        images.clear();
        images.push_back({gen, 0, 0, QAbstractTexture::CubeMapPositiveX});
        texture.setImages(images);
        texture.createOrUpdateGLTexture();

        // THEN
        QVERIFY(texture.properties().format != QAbstractTexture::Automatic);
        QVERIFY(texture.properties().format != QAbstractTexture::NoFormat);

        // Cleanup
        texture.destroy();
    }
};

QTEST_MAIN(tst_GLTexture);

#include "tst_gltexture.moc"
