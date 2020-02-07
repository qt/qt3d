/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
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
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DRender/qabstracttextureimage.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"

class FakeTexture : public Qt3DRender::QAbstractTexture
{
public:
    int sharedTextureId() const
    {
        return static_cast<Qt3DRender::QAbstractTexturePrivate *>(d_ptr.get())->m_sharedTextureId;
    }

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override
    {
        Qt3DRender::QAbstractTexture::sceneChangeEvent(change);
    }

};

class FakeTextureImage : public Qt3DRender::QAbstractTextureImage
{
protected:
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override
    {
        return Qt3DRender::QTextureImageDataGeneratorPtr();
    }
};

class tst_QAbstractTexture : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QAbstractTexture::TextureFormat>("TextureFormat");
        qRegisterMetaType<Qt3DRender::QAbstractTexture::Filter>("Filter");
        qRegisterMetaType<Qt3DRender::QAbstractTexture::ComparisonFunction>("ComparisonFunction");
        qRegisterMetaType<Qt3DRender::QAbstractTexture::ComparisonMode>("ComparisonMode");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        FakeTexture abstractTexture;

        // THEN
        QCOMPARE(abstractTexture.target(), Qt3DRender::QAbstractTexture::Target2D);
        QCOMPARE(abstractTexture.format(), Qt3DRender::QAbstractTexture::Automatic);
        QCOMPARE(abstractTexture.generateMipMaps(), false);
        QCOMPARE(abstractTexture.wrapMode()->x(), Qt3DRender::QTextureWrapMode::ClampToEdge);
        QCOMPARE(abstractTexture.wrapMode()->y(), Qt3DRender::QTextureWrapMode::ClampToEdge);
        QCOMPARE(abstractTexture.wrapMode()->z(), Qt3DRender::QTextureWrapMode::ClampToEdge);
        QCOMPARE(abstractTexture.status(), Qt3DRender::QAbstractTexture::None);
        QCOMPARE(abstractTexture.width(), 1);
        QCOMPARE(abstractTexture.height(), 1);
        QCOMPARE(abstractTexture.depth(), 1);
        QCOMPARE(abstractTexture.magnificationFilter(), Qt3DRender::QAbstractTexture::Nearest);
        QCOMPARE(abstractTexture.minificationFilter(),  Qt3DRender::QAbstractTexture::Nearest);
        QCOMPARE(abstractTexture.maximumAnisotropy(), 1.0f);
        QCOMPARE(abstractTexture.comparisonFunction(), Qt3DRender::QAbstractTexture::CompareLessEqual);
        QCOMPARE(abstractTexture.comparisonMode(), Qt3DRender::QAbstractTexture::CompareNone);
        QCOMPARE(abstractTexture.layers(), 1);
        QCOMPARE(abstractTexture.samples(), 1);
        QCOMPARE(abstractTexture.textureImages().size(), 0);
        QCOMPARE(abstractTexture.samples(), 1);
        QCOMPARE(abstractTexture.sharedTextureId(), -1);
        QCOMPARE(abstractTexture.handleType(), Qt3DRender::QAbstractTexture::NoHandle);
        QCOMPARE(abstractTexture.handle(), QVariant());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        FakeTexture abstractTexture;

        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(formatChanged(TextureFormat)));
            const Qt3DRender::QAbstractTexture::TextureFormat newValue = Qt3DRender::QAbstractTexture::R8I;
            abstractTexture.setFormat(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.format(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setFormat(newValue);

            // THEN
            QCOMPARE(abstractTexture.format(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(generateMipMapsChanged(bool)));
            const bool newValue = true;
            abstractTexture.setGenerateMipMaps(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.generateMipMaps(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setGenerateMipMaps(newValue);

            // THEN
            QCOMPARE(abstractTexture.generateMipMaps(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(widthChanged(int)));
            const int newValue = 383;
            abstractTexture.setWidth(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.width(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setWidth(newValue);

            // THEN
            QCOMPARE(abstractTexture.width(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(heightChanged(int)));
            const int newValue = 427;
            abstractTexture.setHeight(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.height(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setHeight(newValue);

            // THEN
            QCOMPARE(abstractTexture.height(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(depthChanged(int)));
            const int newValue = 454;
            abstractTexture.setDepth(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.depth(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setDepth(newValue);

            // THEN
            QCOMPARE(abstractTexture.depth(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(magnificationFilterChanged(Qt3DRender::QAbstractTexture::Filter)));
            const Qt3DRender::QAbstractTexture::Filter newValue = Qt3DRender::QAbstractTexture::LinearMipMapLinear;
            abstractTexture.setMagnificationFilter(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.magnificationFilter(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setMagnificationFilter(newValue);

            // THEN
            QCOMPARE(abstractTexture.magnificationFilter(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(minificationFilterChanged(Filter)));
            const Qt3DRender::QAbstractTexture::Filter newValue = Qt3DRender::QAbstractTexture::LinearMipMapNearest;
            abstractTexture.setMinificationFilter(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.minificationFilter(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setMinificationFilter(newValue);

            // THEN
            QCOMPARE(abstractTexture.minificationFilter(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(maximumAnisotropyChanged(float)));
            const float newValue = 100.0f;
            abstractTexture.setMaximumAnisotropy(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.maximumAnisotropy(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setMaximumAnisotropy(newValue);

            // THEN
            QCOMPARE(abstractTexture.maximumAnisotropy(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(comparisonFunctionChanged(Qt3DRender::QAbstractTexture::ComparisonFunction)));
            const Qt3DRender::QAbstractTexture::ComparisonFunction newValue = Qt3DRender::QAbstractTexture::CompareGreaterEqual;
            abstractTexture.setComparisonFunction(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.comparisonFunction(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setComparisonFunction(newValue);

            // THEN
            QCOMPARE(abstractTexture.comparisonFunction(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(comparisonModeChanged(Qt3DRender::QAbstractTexture::ComparisonMode)));
            const Qt3DRender::QAbstractTexture::ComparisonMode newValue = Qt3DRender::QAbstractTexture::CompareRefToTexture;
            abstractTexture.setComparisonMode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.comparisonMode(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setComparisonMode(newValue);

            // THEN
            QCOMPARE(abstractTexture.comparisonMode(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(layersChanged(int)));
            const int newValue = 512;
            abstractTexture.setLayers(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.layers(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setLayers(newValue);

            // THEN
            QCOMPARE(abstractTexture.layers(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&abstractTexture, SIGNAL(samplesChanged(int)));
            const int newValue = 1024;
            abstractTexture.setSamples(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(abstractTexture.samples(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            abstractTexture.setSamples(newValue);

            // THEN
            QCOMPARE(abstractTexture.samples(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        FakeTexture abstractTexture;

        abstractTexture.setFormat(Qt3DRender::QAbstractTexture::RG3B2);
        abstractTexture.setGenerateMipMaps(true);
        abstractTexture.setWidth(350);
        abstractTexture.setHeight(383);
        abstractTexture.setDepth(396);
        abstractTexture.setMagnificationFilter(Qt3DRender::QAbstractTexture::NearestMipMapLinear);
        abstractTexture.setMinificationFilter(Qt3DRender::QAbstractTexture::NearestMipMapNearest);
        abstractTexture.setMaximumAnisotropy(12.0f);
        abstractTexture.setComparisonFunction(Qt3DRender::QAbstractTexture::CommpareNotEqual);
        abstractTexture.setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);
        abstractTexture.setLayers(128);
        abstractTexture.setSamples(256);
        abstractTexture.setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::ClampToBorder));

        FakeTextureImage image;
        FakeTextureImage image2;
        abstractTexture.addTextureImage(&image);
        abstractTexture.addTextureImage(&image2);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&abstractTexture);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 3); // Texture + Images

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QAbstractTextureData>>(creationChanges.first());
            const Qt3DRender::QAbstractTextureData cloneData = creationChangeData->data;

            QCOMPARE(abstractTexture.target(), cloneData.target);
            QCOMPARE(abstractTexture.format(), cloneData.format);
            QCOMPARE(abstractTexture.generateMipMaps(), cloneData.autoMipMap);
            QCOMPARE(abstractTexture.wrapMode()->x(), cloneData.wrapModeX);
            QCOMPARE(abstractTexture.wrapMode()->x(), cloneData.wrapModeY);
            QCOMPARE(abstractTexture.wrapMode()->x(), cloneData.wrapModeZ);
            QCOMPARE(abstractTexture.width(), cloneData.width);
            QCOMPARE(abstractTexture.height(), cloneData.height);
            QCOMPARE(abstractTexture.depth(), cloneData.depth);
            QCOMPARE(abstractTexture.magnificationFilter(), cloneData.magFilter);
            QCOMPARE(abstractTexture.minificationFilter(), cloneData.minFilter);
            QCOMPARE(abstractTexture.maximumAnisotropy(), cloneData.maximumAnisotropy);
            QCOMPARE(abstractTexture.comparisonFunction(), cloneData.comparisonFunction);
            QCOMPARE(abstractTexture.comparisonMode(), cloneData.comparisonMode);
            QCOMPARE(abstractTexture.layers(), cloneData.layers);
            QCOMPARE(abstractTexture.samples(), cloneData.samples);
            QCOMPARE(abstractTexture.sharedTextureId(), cloneData.sharedTextureId);
            QCOMPARE(abstractTexture.textureImages().size(), cloneData.textureImageIds.size());

            for (int i = 0, m = abstractTexture.textureImages().size(); i < m; ++i)
                QCOMPARE(abstractTexture.textureImages().at(i)->id(), cloneData.textureImageIds.at(i));

            QCOMPARE(abstractTexture.id(), creationChangeData->subjectId());
            QCOMPARE(abstractTexture.isEnabled(), true);
            QCOMPARE(abstractTexture.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(abstractTexture.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        abstractTexture.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&abstractTexture);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 3); // Texture + Images

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QAbstractTextureData>>(creationChanges.first());
            const Qt3DRender::QAbstractTextureData cloneData = creationChangeData->data;

            QCOMPARE(abstractTexture.target(), cloneData.target);
            QCOMPARE(abstractTexture.format(), cloneData.format);
            QCOMPARE(abstractTexture.generateMipMaps(), cloneData.autoMipMap);
            QCOMPARE(abstractTexture.wrapMode()->x(), cloneData.wrapModeX);
            QCOMPARE(abstractTexture.wrapMode()->x(), cloneData.wrapModeY);
            QCOMPARE(abstractTexture.wrapMode()->x(), cloneData.wrapModeZ);
            QCOMPARE(abstractTexture.width(), cloneData.width);
            QCOMPARE(abstractTexture.height(), cloneData.height);
            QCOMPARE(abstractTexture.depth(), cloneData.depth);
            QCOMPARE(abstractTexture.magnificationFilter(), cloneData.magFilter);
            QCOMPARE(abstractTexture.minificationFilter(), cloneData.minFilter);
            QCOMPARE(abstractTexture.maximumAnisotropy(), cloneData.maximumAnisotropy);
            QCOMPARE(abstractTexture.comparisonFunction(), cloneData.comparisonFunction);
            QCOMPARE(abstractTexture.comparisonMode(), cloneData.comparisonMode);
            QCOMPARE(abstractTexture.layers(), cloneData.layers);
            QCOMPARE(abstractTexture.samples(), cloneData.samples);
            QCOMPARE(abstractTexture.sharedTextureId(), cloneData.sharedTextureId);
            QCOMPARE(abstractTexture.textureImages().size(), cloneData.textureImageIds.size());

            for (int i = 0, m = abstractTexture.textureImages().size(); i < m; ++i)
                QCOMPARE(abstractTexture.textureImages().at(i)->id(), cloneData.textureImageIds.at(i));

            QCOMPARE(abstractTexture.id(), creationChangeData->subjectId());
            QCOMPARE(abstractTexture.isEnabled(), false);
            QCOMPARE(abstractTexture.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(abstractTexture.metaObject(), creationChangeData->metaObject());
        }
    }

    void checkFormatUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setFormat(Qt3DRender::QAbstractTexture::RG8_UNorm);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setFormat(Qt3DRender::QAbstractTexture::RG8_UNorm);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkGenerateMipMapsUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setGenerateMipMaps(true);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setGenerateMipMaps(true);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkWidthUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setWidth(1024);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setWidth(1024);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkHeightUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setHeight(256);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setHeight(256);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkDepthUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setDepth(512);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setDepth(512);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkMagnificationFilterUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setMagnificationFilter(Qt3DRender::QAbstractTexture::NearestMipMapLinear);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setMagnificationFilter(Qt3DRender::QAbstractTexture::NearestMipMapLinear);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkMinificationFilterUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setMinificationFilter(Qt3DRender::QAbstractTexture::NearestMipMapLinear);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setMinificationFilter(Qt3DRender::QAbstractTexture::NearestMipMapLinear);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkMaximumAnisotropyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setMaximumAnisotropy(327.0f);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setMaximumAnisotropy(327.0f);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkComparisonFunctionUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setComparisonFunction(Qt3DRender::QAbstractTexture::CompareAlways);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setComparisonFunction(Qt3DRender::QAbstractTexture::CompareAlways);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkComparisonModeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkLayersUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setLayers(64);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setLayers(64);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkSamplesUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.setSamples(16);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            abstractTexture.setSamples(16);

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

    void checkTextureImageAdded()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            FakeTextureImage image;
            abstractTexture.addTextureImage(&image);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }
    }

    void checkTextureImageRemoved()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        FakeTextureImage image;
        abstractTexture.addTextureImage(&image);
        arbiter.setArbiterOnNode(&abstractTexture);

        {
            // WHEN
            abstractTexture.removeTextureImage(&image);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);

            arbiter.dirtyNodes.clear();
        }
    }

    void checkSceneChangedEvent()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTexture abstractTexture;
        arbiter.setArbiterOnNode(&abstractTexture);

        qRegisterMetaType<Qt3DRender::QAbstractTexture::Status>("Status");
        qRegisterMetaType<Qt3DRender::QAbstractTexture::TextureFormat>("TextureFormat");
        qRegisterMetaType<Qt3DRender::QAbstractTexture::HandleType>("HandleType");


        {
            QSignalSpy spy(&abstractTexture, SIGNAL(widthChanged(int)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("width");
            valueChange->setValue(883);
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.width(), 883);

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.width(), 883);
        }

        {
            QSignalSpy spy(&abstractTexture, SIGNAL(heightChanged(int)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("height");
            valueChange->setValue(1584);
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.height(), 1584);

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.height(), 1584);
        }

        {
            QSignalSpy spy(&abstractTexture, SIGNAL(depthChanged(int)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("depth");
            valueChange->setValue(8);
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.depth(), 8);

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.depth(), 8);
        }

        {
            QSignalSpy spy(&abstractTexture, SIGNAL(layersChanged(int)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("layers");
            valueChange->setValue(256);
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.layers(), 256);

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.layers(), 256);
        }

        {
            QSignalSpy spy(&abstractTexture, SIGNAL(formatChanged(TextureFormat)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("format");
            const auto newFormat = Qt3DRender::QAbstractTexture::R8I;
            valueChange->setValue(QVariant::fromValue(newFormat));
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.format(), newFormat);

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.format(), newFormat);
        }

        {
            QSignalSpy spy(&abstractTexture, SIGNAL(statusChanged(Status)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("status");
            const auto newStatus = Qt3DRender::QAbstractTexture::Error;
            valueChange->setValue(QVariant::fromValue(newStatus));
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.status(), newStatus);

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.status(), newStatus);
        }

        {
            QSignalSpy spy(&abstractTexture, SIGNAL(handleTypeChanged(HandleType)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("handleType");
            const auto newType = Qt3DRender::QAbstractTexture::OpenGLTextureId;
            valueChange->setValue(QVariant::fromValue(newType));
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.handleType(), newType);

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.handleType(), newType);
        }

        {
            QSignalSpy spy(&abstractTexture, SIGNAL(handleChanged(QVariant)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("handle");
            valueChange->setValue(QVariant(1));
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 1);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.handle(), QVariant(1));

            // WHEN
            spy.clear();
            abstractTexture.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(abstractTexture.handle(), QVariant(1));
        }
    }

    void checkTextureDataUpdate()
    {
        {
            // GIVEN
            TestArbiter arbiter;
            FakeTexture abstractTexture;
            Qt3DRender::QTextureDataUpdate update;
            arbiter.setArbiterOnNode(&abstractTexture);

            // WHEN
            abstractTexture.updateData(update);
            QCoreApplication::processEvents();

            // THEN (arbiter -> should not be stored in the initial changes but only send as a property change)
            auto d = static_cast<Qt3DRender::QAbstractTexturePrivate*>(Qt3DRender::QAbstractTexturePrivate::get(&abstractTexture));
            QCOMPARE(d->m_pendingDataUpdates.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &abstractTexture);
        }
    }

};

QTEST_MAIN(tst_QAbstractTexture)

#include "tst_qabstracttexture.moc"
