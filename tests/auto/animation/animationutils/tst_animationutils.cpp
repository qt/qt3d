/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DAnimation/private/animationcliploader_p.h>
#include <Qt3DAnimation/private/animationutils_p.h>
#include <Qt3DAnimation/private/channelmapper_p.h>
#include <Qt3DAnimation/private/channelmapping_p.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qquaternion.h>

#include <qbackendnodetester.h>
#include <testpostmanarbiter.h>

using namespace Qt3DAnimation::Animation;

Q_DECLARE_METATYPE(Qt3DAnimation::Animation::Handler*)
Q_DECLARE_METATYPE(QVector<ChannelMapping *>)
Q_DECLARE_METATYPE(ChannelMapper *)
Q_DECLARE_METATYPE(AnimationClipLoader *)
Q_DECLARE_METATYPE(QVector<MappingData>)
Q_DECLARE_METATYPE(QVector<Qt3DCore::QPropertyUpdatedChangePtr>)

class tst_AnimationUtils : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

public:
    ChannelMapping *createChannelMapping(Handler *handler,
                                         const QString &channelName,
                                         const Qt3DCore::QNodeId targetId,
                                         const QString &property,
                                         const char *propertyName,
                                         int type)
    {
        auto channelMappingId = Qt3DCore::QNodeId::createId();
        ChannelMapping *channelMapping = handler->channelMappingManager()->getOrCreateResource(channelMappingId);
        setPeerId(channelMapping, channelMappingId);
        channelMapping->setTargetId(targetId);
        channelMapping->setProperty(property);
        channelMapping->setPropertyName(propertyName);
        channelMapping->setChannelName(channelName);
        channelMapping->setType(type);
        return channelMapping;
    }

    ChannelMapper *createChannelMapper(Handler *handler,
                                       const QVector<Qt3DCore::QNodeId> &mappingIds)
    {
        auto channelMapperId = Qt3DCore::QNodeId::createId();
        ChannelMapper *channelMapper = handler->channelMapperManager()->getOrCreateResource(channelMapperId);
        setPeerId(channelMapper, channelMapperId);
        channelMapper->setMappingIds(mappingIds);
        return channelMapper;
    }

    AnimationClipLoader *createAnimationClipLoader(Handler *handler,
                                                   const QUrl &source)
    {
        auto clipId = Qt3DCore::QNodeId::createId();
        AnimationClipLoader *clip = handler->animationClipLoaderManager()->getOrCreateResource(clipId);
        setPeerId(clip, clipId);
        clip->setSource(source);
        clip->loadAnimation();
        return clip;
    }

private Q_SLOTS:
    void checkBuildPropertyMappings_data()
    {
        QTest::addColumn<Handler *>("handler");
        QTest::addColumn<QVector<ChannelMapping *>>("channelMappings");
        QTest::addColumn<ChannelMapper *>("channelMapper");
        QTest::addColumn<AnimationClipLoader *>("clip");
        QTest::addColumn<QVector<MappingData>>("expectedMappingData");

        auto handler = new Handler;
        auto channelMapping = createChannelMapping(handler,
                                                   QLatin1String("Location"),
                                                   Qt3DCore::QNodeId::createId(),
                                                   QLatin1String("translation"),
                                                   "translation",
                                                   static_cast<int>(QVariant::Vector3D));
        QVector<ChannelMapping *> channelMappings;
        channelMappings.push_back(channelMapping);

        // ... a channel mapper...
        auto channelMapper = createChannelMapper(handler, QVector<Qt3DCore::QNodeId>() << channelMapping->peerId());

        // ...and an animation clip
        auto clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));

        QVector<MappingData> mappingData;
        MappingData mapping;
        mapping.targetId = channelMapping->targetId();
        mapping.propertyName = channelMapping->propertyName(); // Location
        mapping.type = channelMapping->type();
        mapping.channelIndices = QVector<int>() << 0 << 1 << 2; // Location X, Y, Z
        mappingData.push_back(mapping);

        QTest::newRow("clip1.json") << handler
                                    << channelMappings
                                    << channelMapper
                                    << clip
                                    << mappingData;
    }

    void checkBuildPropertyMappings()
    {
        // GIVEN
        QFETCH(Handler *, handler);
        QFETCH(QVector<ChannelMapping *>, channelMappings);
        QFETCH(ChannelMapper *, channelMapper);
        QFETCH(AnimationClipLoader *, clip);
        QFETCH(QVector<MappingData>, expectedMappingData);

        // WHEN
        // Build the mapping data for the above configuration
        QVector<MappingData> mappingData = buildPropertyMappings(handler, clip, channelMapper);

        // THEN
        QCOMPARE(mappingData.size(), expectedMappingData.size());
        for (int i = 0; i < mappingData.size(); ++i) {
            const auto mapping = mappingData[i];
            const auto expectedMapping = expectedMappingData[i];

            QCOMPARE(mapping.targetId, expectedMapping.targetId);
            QCOMPARE(mapping.propertyName, expectedMapping.propertyName);
            QCOMPARE(mapping.type, expectedMapping.type);
            QCOMPARE(mapping.channelIndices.size(), expectedMapping.channelIndices.size());
            for (int j = 0; j < mapping.channelIndices.size(); ++j) {
                QCOMPARE(mapping.channelIndices[j], expectedMapping.channelIndices[j]);
            }
        }

        // Cleanup
        delete handler;
    }

    void checkLocalTimeFromGlobalTime_data()
    {
        QTest::addColumn<double>("globalTime");
        QTest::addColumn<double>("globalStartTime");
        QTest::addColumn<double>("playbackRate");
        QTest::addColumn<double>("duration");
        QTest::addColumn<int>("loopCount");
        QTest::addColumn<double>("expectedLocalTime");
        QTest::addColumn<int>("expectedCurrentLoop");

        double globalTime;
        double globalStartTime;
        double playbackRate;
        double duration;
        int loopCount;
        double expectedLocalTime;
        int expectedCurrentLoop;

        globalTime = 0.0;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 1;
        expectedLocalTime = 0.0;
        expectedCurrentLoop = 0;
        QTest::newRow("simple, t_global = 0")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 0.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 1;
        expectedLocalTime = 0.5;
        expectedCurrentLoop = 0;
        QTest::newRow("simple, t_global = 0.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 1.0;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 1;
        expectedLocalTime = 1.0;
        expectedCurrentLoop = 0;
        QTest::newRow("simple, t_global = 1.0")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = -0.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 1;
        expectedLocalTime = 0.0;
        expectedCurrentLoop = 0;
        QTest::newRow("simple, t_global = -0.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 1.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 1;
        expectedLocalTime = 1.0;
        expectedCurrentLoop = 0;
        QTest::newRow("simple, t_global = 1.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 0.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 2;
        expectedLocalTime = 0.5;
        expectedCurrentLoop = 0;
        QTest::newRow("simple, loopCount = 2, t_global = 0.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 1.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 2;
        expectedLocalTime = 0.5;
        expectedCurrentLoop = 1;
        QTest::newRow("simple, loopCount = 2, t_global = 1.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 3.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 2.0;
        loopCount = 2;
        expectedLocalTime = 1.5;
        expectedCurrentLoop = 1;
        QTest::newRow("duration = 2, loopCount = 2, t_global = 3.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 4.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 2.0;
        loopCount = 2;
        expectedLocalTime = 2.0;
        expectedCurrentLoop = 1;
        QTest::newRow("duration = 2, loopCount = 2, t_global = 4.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 1.5;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 0;
        expectedLocalTime = 0.5;
        expectedCurrentLoop = 1;
        QTest::newRow("simple, loopCount = inf, t_global = 1.5")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;

        globalTime = 10.2;
        globalStartTime = 0.0;
        playbackRate = 1.0;
        duration = 1.0;
        loopCount = 0;
        expectedLocalTime = 0.2;
        expectedCurrentLoop = 10;
        QTest::newRow("simple, loopCount = inf, t_global = 10.2")
                << globalTime << globalStartTime << playbackRate << duration << loopCount
                << expectedLocalTime << expectedCurrentLoop;
    }

    void checkLocalTimeFromGlobalTime()
    {
        // GIVEN
        QFETCH(double, globalTime);
        QFETCH(double, globalStartTime);
        QFETCH(double, playbackRate);
        QFETCH(double, duration);
        QFETCH(int, loopCount);
        QFETCH(double, expectedLocalTime);
        QFETCH(int, expectedCurrentLoop);

        // WHEN
        int actualCurrentLoop = 0;
        double actualLocalTime = localTimeFromGlobalTime(globalTime,
                                                         globalStartTime,
                                                         playbackRate,
                                                         duration,
                                                         loopCount,
                                                         actualCurrentLoop);

        // THEN
        QCOMPARE(actualCurrentLoop, expectedCurrentLoop);
        QCOMPARE(actualLocalTime, expectedLocalTime);
    }

    void checkPreparePropertyChanges_data()
    {
        QTest::addColumn<Qt3DCore::QNodeId>("animatorId");
        QTest::addColumn<QVector<MappingData>>("mappingData");
        QTest::addColumn<QVector<float>>("channelResults");
        QTest::addColumn<bool>("finalFrame");
        QTest::addColumn<QVector<Qt3DCore::QPropertyUpdatedChangePtr>>("expectedChanges");

        Qt3DCore::QNodeId animatorId;
        QVector<MappingData> mappingData;
        QVector<float> channelResults;
        bool finalFrame;
        QVector<Qt3DCore::QPropertyUpdatedChangePtr> expectedChanges;

        // Single property, vec3
        {
            animatorId = Qt3DCore::QNodeId::createId();
            MappingData mapping;
            mapping.targetId = Qt3DCore::QNodeId::createId();
            mapping.propertyName = "translation";
            mapping.type = static_cast<int>(QVariant::Vector3D);
            mapping.channelIndices = QVector<int>() << 0 << 1 << 2;
            mappingData.push_back(mapping);
            channelResults = QVector<float>() << 1.0f << 2.0f << 3.0f;
            finalFrame = false;

            auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(mapping.targetId);
            change->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            change->setPropertyName(mapping.propertyName);
            change->setValue(QVariant::fromValue(QVector3D(1.0f, 2.0f, 3.0f)));
            expectedChanges.push_back(change);

            QTest::newRow("vec3 translation, final = false")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            finalFrame = true;
            auto animatorChange = Qt3DCore::QPropertyUpdatedChangePtr::create(animatorId);
            animatorChange->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            animatorChange->setPropertyName("running");
            animatorChange->setValue(false);
            expectedChanges.push_back(animatorChange);

            QTest::newRow("vec3 translation, final = true")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            mappingData.clear();
            channelResults.clear();
            expectedChanges.clear();
        }

        // Multiple properties, all vec3
        {
            animatorId = Qt3DCore::QNodeId::createId();
            MappingData translationMapping;
            translationMapping.targetId = Qt3DCore::QNodeId::createId();
            translationMapping.propertyName = "translation";
            translationMapping.type = static_cast<int>(QVariant::Vector3D);
            translationMapping.channelIndices = QVector<int>() << 0 << 1 << 2;
            mappingData.push_back(translationMapping);

            MappingData scaleMapping;
            scaleMapping.targetId = Qt3DCore::QNodeId::createId();
            scaleMapping.propertyName = "scale";
            scaleMapping.type = static_cast<int>(QVariant::Vector3D);
            scaleMapping.channelIndices = QVector<int>() << 3 << 4 << 5;
            mappingData.push_back(scaleMapping);

            channelResults = QVector<float>() << 1.0f << 2.0f << 3.0f
                                              << 4.0f << 5.0f << 6.0f;
            finalFrame = false;

            auto translationChange = Qt3DCore::QPropertyUpdatedChangePtr::create(translationMapping.targetId);
            translationChange->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            translationChange->setPropertyName(translationMapping.propertyName);
            translationChange->setValue(QVariant::fromValue(QVector3D(1.0f, 2.0f, 3.0f)));
            expectedChanges.push_back(translationChange);

            auto scaleChange = Qt3DCore::QPropertyUpdatedChangePtr::create(scaleMapping.targetId);
            scaleChange->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            scaleChange->setPropertyName(scaleMapping.propertyName);
            scaleChange->setValue(QVariant::fromValue(QVector3D(4.0f, 5.0f, 6.0f)));
            expectedChanges.push_back(scaleChange);

            QTest::newRow("vec3 translation, vec3 scale, final = false")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            finalFrame = true;
            auto animatorChange = Qt3DCore::QPropertyUpdatedChangePtr::create(animatorId);
            animatorChange->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            animatorChange->setPropertyName("running");
            animatorChange->setValue(false);
            expectedChanges.push_back(animatorChange);

            QTest::newRow("vec3 translation, vec3 scale, final = true")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            mappingData.clear();
            channelResults.clear();
            expectedChanges.clear();
        }

        // Single property, double
        {
            animatorId = Qt3DCore::QNodeId::createId();
            MappingData mapping;
            mapping.targetId = Qt3DCore::QNodeId::createId();
            mapping.propertyName = "mass";
            mapping.type = static_cast<int>(QVariant::Double);
            mapping.channelIndices = QVector<int>() << 0;
            mappingData.push_back(mapping);
            channelResults = QVector<float>() << 3.5f;
            finalFrame = false;

            auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(mapping.targetId);
            change->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            change->setPropertyName(mapping.propertyName);
            change->setValue(QVariant::fromValue(3.5f));
            expectedChanges.push_back(change);

            QTest::newRow("double mass")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            mappingData.clear();
            channelResults.clear();
            expectedChanges.clear();
        }

        // Single property, vec2
        {
            animatorId = Qt3DCore::QNodeId::createId();
            MappingData mapping;
            mapping.targetId = Qt3DCore::QNodeId::createId();
            mapping.propertyName = "pos";
            mapping.type = static_cast<int>(QVariant::Vector2D);
            mapping.channelIndices = QVector<int>() << 0 << 1;
            mappingData.push_back(mapping);
            channelResults = QVector<float>() << 2.0f << 1.0f;
            finalFrame = false;

            auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(mapping.targetId);
            change->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            change->setPropertyName(mapping.propertyName);
            change->setValue(QVariant::fromValue(QVector2D(2.0f, 1.0f)));
            expectedChanges.push_back(change);

            QTest::newRow("vec2 pos")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            mappingData.clear();
            channelResults.clear();
            expectedChanges.clear();
        }

        // Single property, vec4
        {
            animatorId = Qt3DCore::QNodeId::createId();
            MappingData mapping;
            mapping.targetId = Qt3DCore::QNodeId::createId();
            mapping.propertyName = "foo";
            mapping.type = static_cast<int>(QVariant::Vector2D);
            mapping.channelIndices = QVector<int>() << 0 << 1 << 2 << 3;
            mappingData.push_back(mapping);
            channelResults = QVector<float>() << 4.0f << 3.0f << 2.0f << 1.0f;
            finalFrame = false;

            auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(mapping.targetId);
            change->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            change->setPropertyName(mapping.propertyName);
            change->setValue(QVariant::fromValue(QVector4D(4.0f, 3.0f, 2.0f, 1.0f)));
            expectedChanges.push_back(change);

            QTest::newRow("vec4 foo")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            mappingData.clear();
            channelResults.clear();
            expectedChanges.clear();
        }

        // Single property, quaternion
        {
            animatorId = Qt3DCore::QNodeId::createId();
            MappingData mapping;
            mapping.targetId = Qt3DCore::QNodeId::createId();
            mapping.propertyName = "rotation";
            mapping.type = static_cast<int>(QVariant::Quaternion);
            mapping.channelIndices = QVector<int>() << 0 << 1 << 2 << 3;
            mappingData.push_back(mapping);
            channelResults = QVector<float>() << 1.0f << 0.0f << 0.0f << 1.0f;
            finalFrame = false;

            auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(mapping.targetId);
            change->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            change->setPropertyName(mapping.propertyName);
            change->setValue(QVariant::fromValue(QQuaternion(1.0f, 0.0f, 0.0f, 1.0f)));
            expectedChanges.push_back(change);

            QTest::newRow("quaternion rotation")
                    << animatorId << mappingData << channelResults << finalFrame
                    << expectedChanges;

            mappingData.clear();
            channelResults.clear();
            expectedChanges.clear();
        }
    }

    void checkPreparePropertyChanges()
    {
        // GIVEN
        QFETCH(Qt3DCore::QNodeId, animatorId);
        QFETCH(QVector<MappingData>, mappingData);
        QFETCH(QVector<float>, channelResults);
        QFETCH(bool, finalFrame);
        QFETCH(QVector<Qt3DCore::QPropertyUpdatedChangePtr>, expectedChanges);

        // WHEN
        QVector<Qt3DCore::QSceneChangePtr> actualChanges
                = preparePropertyChanges(animatorId, mappingData, channelResults, finalFrame);

        // THEN
        QCOMPARE(actualChanges.size(), expectedChanges.size());
        for (int i = 0; i < actualChanges.size(); ++i) {
            auto expectedChange = expectedChanges[i];
            auto actualChange
                    = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(expectedChanges[i]);

            QCOMPARE(actualChange->subjectId(), expectedChange->subjectId());
            QCOMPARE(actualChange->deliveryFlags(), expectedChange->deliveryFlags());
            QCOMPARE(actualChange->propertyName(), expectedChange->propertyName());
            QCOMPARE(actualChange->value(), expectedChange->value());
        }
    }
};

QTEST_MAIN(tst_AnimationUtils)

#include "tst_animationutils.moc"
