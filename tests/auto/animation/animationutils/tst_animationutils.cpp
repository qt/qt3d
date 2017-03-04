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
Q_DECLARE_METATYPE(Channel)
Q_DECLARE_METATYPE(AnimatorEvaluationData)
Q_DECLARE_METATYPE(ClipEvaluationData)
Q_DECLARE_METATYPE(ClipAnimator *)

bool fuzzyCompare(float x1, float x2)
{
    if (qFuzzyIsNull(x1) && qFuzzyIsNull(x2)) {
        return true;
    } else if (qFuzzyIsNull(x1) && !qFuzzyIsNull(x2) ||
               !qFuzzyIsNull(x1) && qFuzzyIsNull(x2)) {
        return false;
    } else {
        return qFuzzyCompare(x1, x2);
    }
}

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

    ClipAnimator *createClipAnimator(Handler *handler,
                                     qint64 globalStartTimeNS,
                                     int loops)
    {
        auto animatorId = Qt3DCore::QNodeId::createId();
        ClipAnimator *animator = handler->clipAnimatorManager()->getOrCreateResource(animatorId);
        setPeerId(animator, animatorId);
        animator->setStartTime(globalStartTimeNS);
        animator->setLoops(loops);
        return animator;
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

    void checkEvaluateClipAtLocalTime_data()
    {
        QTest::addColumn<Handler *>("handler");
        QTest::addColumn<AnimationClipLoader *>("clip");
        QTest::addColumn<float>("localTime");
        QTest::addColumn<ClipResults>("expectedResults");

        Handler *handler;
        AnimationClipLoader *clip;
        float localTime;
        ClipResults expectedResults;

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));
            localTime = 0.0f;
            expectedResults = QVector<float>() << 0.0f << 0.0f << 0.0f;

            QTest::newRow("clip1.json, t = 0.0")
                    << handler << clip << localTime << expectedResults;
            expectedResults.clear();
        }

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));
            localTime = clip->duration();
            expectedResults = QVector<float>() << 5.0f << 0.0f << 0.0f;

            QTest::newRow("clip1.json, t = duration")
                    << handler << clip << localTime << expectedResults;
            expectedResults.clear();
        }

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));
            localTime = clip->duration() / 2.0f;
            expectedResults = QVector<float>() << 2.5f << 0.0f << 0.0f;

            QTest::newRow("clip1.json, t = duration/2")
                    << handler << clip << localTime << expectedResults;
            expectedResults.clear();
        }

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip2.json"));
            localTime = 0.0f;
            expectedResults = QVector<float>()
                    << 0.0f << 0.0f << 0.0f             // Translation
                    << 1.0f << 0.0f << 0.0f << 0.0f;    // Rotation

            QTest::newRow("clip2.json, t = 0.0")
                    << handler << clip << localTime << expectedResults;
            expectedResults.clear();
        }
        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip2.json"));
            localTime = clip->duration();
            expectedResults = QVector<float>()
                    << 5.0f << 0.0f << 0.0f             // Translation
                    << 0.0f << 0.0f << -1.0f << 0.0f;   // Rotation

            QTest::newRow("clip2.json, t = duration")
                    << handler << clip << localTime << expectedResults;
            expectedResults.clear();
        }
        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip2.json"));
            localTime = clip->duration() / 2.0f;
            expectedResults = QVector<float>()
                    << 2.5f << 0.0f << 0.0f             // Translation
                    << 0.5f << 0.0f << -0.5f << 0.0f;   // Rotation

            QTest::newRow("clip2.json, t = duration/2")
                    << handler << clip << localTime << expectedResults;
            expectedResults.clear();
        }
    }

    void checkEvaluateClipAtLocalTime()
    {
        // GIVEN
        QFETCH(Handler *, handler);
        QFETCH(AnimationClipLoader *, clip);
        QFETCH(float, localTime);
        QFETCH(ClipResults, expectedResults);

        // WHEN
        ClipResults actualResults = evaluateClipAtLocalTime(clip, localTime);

        // THEN
        QCOMPARE(actualResults.size(), expectedResults.size());
        for (int i = 0; i < actualResults.size(); ++i) {
            auto actual = actualResults[i];
            auto expected = expectedResults[i];

            QVERIFY(fuzzyCompare(actual, expected) == true);
        }

        // Cleanup
        delete handler;
    }

    void checkChannelComponentsToIndicesHelper_data()
    {
        QTest::addColumn<Channel>("channel");
        QTest::addColumn<int>("dataType");
        QTest::addColumn<int>("offset");
        QTest::addColumn<QVector<char>>("suffixes");
        QTest::addColumn<QVector<int>>("expectedResults");

        Channel channel;
        int dataType;
        int offset;
        QVector<char> suffixes;
        QVector<int> expectedResults;

        // vec3 with and without offset
        {
            channel = Channel();
            channel.name = QLatin1String("Location");
            channel.channelComponents.resize(3);
            channel.channelComponents[0].name = QLatin1String("Location X");
            channel.channelComponents[1].name = QLatin1String("Location Y");
            channel.channelComponents[2].name = QLatin1String("Location Z");

            dataType = static_cast<int>(QVariant::Vector3D);
            offset = 0;
            suffixes = (QVector<char>() << 'X' << 'Y' << 'Z' << 'W');
            expectedResults = (QVector<int>() << 0 << 1 << 2);

            QTest::newRow("vec3 location, offset = 0")
                    << channel << dataType << offset << suffixes << expectedResults;

            expectedResults.clear();

            offset = 4;
            expectedResults = (QVector<int>() << 4 << 5 << 6);
            QTest::newRow("vec3 location, offset = 4")
                    << channel << dataType << offset << suffixes << expectedResults;

            suffixes.clear();
            expectedResults.clear();
        }

        // vec2 with and without offset
        {
            channel = Channel();
            channel.name = QLatin1String("pos");
            channel.channelComponents.resize(2);
            channel.channelComponents[0].name = QLatin1String("pos X");
            channel.channelComponents[1].name = QLatin1String("pos Y");

            dataType = static_cast<int>(QVariant::Vector2D);
            offset = 0;
            suffixes = (QVector<char>() << 'X' << 'Y' << 'Z' << 'W');
            expectedResults = (QVector<int>() << 0 << 1);

            QTest::newRow("vec2 pos, offset = 0")
                    << channel << dataType << offset << suffixes << expectedResults;

            expectedResults.clear();

            offset = 2;
            expectedResults = (QVector<int>() << 2 << 3);
            QTest::newRow("vec2 pos, offset = 2")
                    << channel << dataType << offset << suffixes << expectedResults;

            suffixes.clear();
            expectedResults.clear();
        }

        // vec4 with and without offset
        {
            channel = Channel();
            channel.name = QLatin1String("foo");
            channel.channelComponents.resize(4);
            channel.channelComponents[0].name = QLatin1String("foo X");
            channel.channelComponents[1].name = QLatin1String("foo Y");
            channel.channelComponents[2].name = QLatin1String("foo Z");
            channel.channelComponents[3].name = QLatin1String("foo W");

            dataType = static_cast<int>(QVariant::Vector4D);
            offset = 0;
            suffixes = (QVector<char>() << 'X' << 'Y' << 'Z' << 'W');
            expectedResults = (QVector<int>() << 0 << 1 << 2 << 3);

            QTest::newRow("vec4 foo, offset = 0")
                    << channel << dataType << offset << suffixes << expectedResults;

            expectedResults.clear();

            offset = 10;
            expectedResults = (QVector<int>() << 10 << 11 << 12 << 13);
            QTest::newRow("vec4 foo, offset = 10")
                    << channel << dataType << offset << suffixes << expectedResults;

            suffixes.clear();
            expectedResults.clear();
        }

        // double with and without offset
        {
            channel = Channel();
            channel.name = QLatin1String("foo");
            channel.channelComponents.resize(1);
            channel.channelComponents[0].name = QLatin1String("Mass X");

            dataType = static_cast<int>(QVariant::Double);
            offset = 0;
            suffixes = (QVector<char>() << 'X' << 'Y' << 'Z' << 'W');
            expectedResults = (QVector<int>() << 0);

            QTest::newRow("double Mass, offset = 0")
                    << channel << dataType << offset << suffixes << expectedResults;

            expectedResults.clear();

            offset = 5;
            expectedResults = (QVector<int>() << 5);
            QTest::newRow("double Mass, offset = 5")
                    << channel << dataType << offset << suffixes << expectedResults;

            suffixes.clear();
            expectedResults.clear();
        }

        // quaternion with and without offset
        {
            channel = Channel();
            channel.name = QLatin1String("Rotation");
            channel.channelComponents.resize(4);
            channel.channelComponents[0].name = QLatin1String("Rotation W");
            channel.channelComponents[1].name = QLatin1String("Rotation X");
            channel.channelComponents[2].name = QLatin1String("Rotation Y");
            channel.channelComponents[3].name = QLatin1String("Rotation Z");

            dataType = static_cast<int>(QVariant::Quaternion);
            offset = 0;
            suffixes = (QVector<char>() << 'W' << 'X' << 'Y' << 'Z');
            expectedResults = (QVector<int>() << 0 << 1 << 2 << 3);

            QTest::newRow("quaternion Rotation, offset = 0")
                    << channel << dataType << offset << suffixes << expectedResults;

            expectedResults.clear();

            offset = 10;
            expectedResults = (QVector<int>() << 10 << 11 << 12 << 13);
            QTest::newRow("quaternion Rotation, offset = 10")
                    << channel << dataType << offset << suffixes << expectedResults;

            suffixes.clear();
            expectedResults.clear();
        }

        // quaternion with and without offset, randomized
        {
            channel = Channel();
            channel.name = QLatin1String("Rotation");
            channel.channelComponents.resize(4);
            channel.channelComponents[0].name = QLatin1String("Rotation X");
            channel.channelComponents[1].name = QLatin1String("Rotation W");
            channel.channelComponents[2].name = QLatin1String("Rotation Z");
            channel.channelComponents[3].name = QLatin1String("Rotation Y");

            dataType = static_cast<int>(QVariant::Quaternion);
            offset = 0;
            suffixes = (QVector<char>() << 'W' << 'X' << 'Y' << 'Z');
            expectedResults = (QVector<int>() << 1 << 0 << 3 << 2);

            QTest::newRow("quaternion Rotation, offset = 0, randomized")
                    << channel << dataType << offset << suffixes << expectedResults;

            expectedResults.clear();

            offset = 10;
            expectedResults = (QVector<int>() << 11 << 10 << 13 << 12);
            QTest::newRow("quaternion Rotation, offset = 10, randomized")
                    << channel << dataType << offset << suffixes << expectedResults;

            suffixes.clear();
            expectedResults.clear();
        }
    }

    void checkChannelComponentsToIndicesHelper()
    {
        // GIVEN
        QFETCH(Channel, channel);
        QFETCH(int, dataType);
        QFETCH(int, offset);
        QFETCH(QVector<char>, suffixes);
        QFETCH(QVector<int>, expectedResults);

        // WHEN
        QVector<int> actualResults
                = channelComponentsToIndicesHelper(channel, dataType, offset, suffixes);

        // THEN
        QCOMPARE(actualResults.size(), expectedResults.size());
        for (int i = 0; i < actualResults.size(); ++i) {
            QCOMPARE(actualResults[i], expectedResults[i]);
        }
    }

    void checkChannelComponentsToIndices_data()
    {
        QTest::addColumn<Channel>("channel");
        QTest::addColumn<int>("dataType");
        QTest::addColumn<int>("offset");
        QTest::addColumn<QVector<int>>("expectedResults");

        Channel channel;
        int dataType;
        int offset;
        QVector<int> expectedResults;

        // Quaternion
        {
            channel = Channel();
            channel.name = QLatin1String("Rotation");
            channel.channelComponents.resize(4);
            channel.channelComponents[0].name = QLatin1String("Rotation W");
            channel.channelComponents[1].name = QLatin1String("Rotation X");
            channel.channelComponents[2].name = QLatin1String("Rotation Y");
            channel.channelComponents[3].name = QLatin1String("Rotation Z");

            dataType = static_cast<int>(QVariant::Quaternion);
            offset = 0;
            expectedResults = (QVector<int>() << 0 << 1 << 2 << 3);

            QTest::newRow("quaternion Rotation, offset = 0")
                    << channel << dataType << offset << expectedResults;

            expectedResults.clear();

            offset = 10;
            expectedResults = (QVector<int>() << 10 << 11 << 12 << 13);
            QTest::newRow("quaternion Rotation, offset = 10")
                    << channel << dataType << offset << expectedResults;

            expectedResults.clear();
        }

        // vec3 with and without offset
        {
            channel = Channel();
            channel.name = QLatin1String("Location");
            channel.channelComponents.resize(3);
            channel.channelComponents[0].name = QLatin1String("Location X");
            channel.channelComponents[1].name = QLatin1String("Location Y");
            channel.channelComponents[2].name = QLatin1String("Location Z");

            dataType = static_cast<int>(QVariant::Vector3D);
            offset = 0;
            expectedResults = (QVector<int>() << 0 << 1 << 2);

            QTest::newRow("vec3 location, offset = 0")
                    << channel << dataType << offset << expectedResults;

            expectedResults.clear();

            offset = 4;
            expectedResults = (QVector<int>() << 4 << 5 << 6);
            QTest::newRow("vec3 location, offset = 4")
                    << channel << dataType << offset << expectedResults;

            expectedResults.clear();
        }
    }

    void checkChannelComponentsToIndices()
    {
        QFETCH(Channel, channel);
        QFETCH(int, dataType);
        QFETCH(int, offset);
        QFETCH(QVector<int>, expectedResults);

        // WHEN
        QVector<int> actualResults
                = channelComponentsToIndices(channel, dataType, offset);

        // THEN
        QCOMPARE(actualResults.size(), expectedResults.size());
        for (int i = 0; i < actualResults.size(); ++i) {
            QCOMPARE(actualResults[i], expectedResults[i]);
        }
    }

    void checkEvaluationDataForClip_data()
    {
        QTest::addColumn<Handler *>("handler");
        QTest::addColumn<AnimationClipLoader *>("clip");
        QTest::addColumn<AnimatorEvaluationData>("animatorData");
        QTest::addColumn<ClipEvaluationData>("expectedClipData");

        Handler *handler;
        AnimationClipLoader *clip;
        AnimatorEvaluationData animatorData;
        ClipEvaluationData clipData;

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));
            const qint64 globalStartTimeNS = 0;
            const int loops = 1;
            auto animator = createClipAnimator(handler, globalStartTimeNS, loops);
            const qint64 globalTimeNS = 0;
            animatorData = evaluationDataForAnimator(animator, globalTimeNS); // Tested elsewhere

            clipData.localTime = localTimeFromGlobalTime(animatorData.globalTime,
                                                         animatorData.startTime,
                                                         animatorData.playbackRate,
                                                         clip->duration(),
                                                         animatorData.loopCount,
                                                         clipData.currentLoop); // Tested elsewhere
            clipData.isFinalFrame = false;

            QTest::newRow("clip1.json, globalTime = 0")
                    << handler << clip << animatorData << clipData;
        }

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));
            const qint64 globalStartTimeNS = 0;
            const int loops = 1;
            auto animator = createClipAnimator(handler, globalStartTimeNS, loops);
            const qint64 globalTimeNS = (clip->duration() + 1.0) * 1.0e9; // +1 to ensure beyond end of clip
            animatorData = evaluationDataForAnimator(animator, globalTimeNS); // Tested elsewhere

            clipData.localTime = localTimeFromGlobalTime(animatorData.globalTime,
                                                         animatorData.startTime,
                                                         animatorData.playbackRate,
                                                         clip->duration(),
                                                         animatorData.loopCount,
                                                         clipData.currentLoop); // Tested elsewhere
            clipData.isFinalFrame = true;

            QTest::newRow("clip1.json, globalTime = duration")
                    << handler << clip << animatorData << clipData;
        }

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));
            const qint64 globalStartTimeNS = 0;
            const int loops = 0; // Infinite loops
            auto animator = createClipAnimator(handler, globalStartTimeNS, loops);
            const qint64 globalTimeNS = 2.0 * clip->duration() * 1.0e9;
            animatorData = evaluationDataForAnimator(animator, globalTimeNS); // Tested elsewhere

            clipData.localTime = localTimeFromGlobalTime(animatorData.globalTime,
                                                         animatorData.startTime,
                                                         animatorData.playbackRate,
                                                         clip->duration(),
                                                         animatorData.loopCount,
                                                         clipData.currentLoop); // Tested elsewhere
            clipData.isFinalFrame = false;

            QTest::newRow("clip1.json, globalTime = 2 * duration, loops = infinite")
                    << handler << clip << animatorData << clipData;
        }

        {
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));
            const qint64 globalStartTimeNS = 0;
            const int loops = 2;
            auto animator = createClipAnimator(handler, globalStartTimeNS, loops);
            const qint64 globalTimeNS = (2.0 * clip->duration() + 1.0) * 1.0e9; // +1 to ensure beyond end of clip
            animatorData = evaluationDataForAnimator(animator, globalTimeNS); // Tested elsewhere

            clipData.localTime = localTimeFromGlobalTime(animatorData.globalTime,
                                                         animatorData.startTime,
                                                         animatorData.playbackRate,
                                                         clip->duration(),
                                                         animatorData.loopCount,
                                                         clipData.currentLoop); // Tested elsewhere
            clipData.isFinalFrame = true;

            QTest::newRow("clip1.json, globalTime = 2 * duration + 1, loops = 2")
                    << handler << clip << animatorData << clipData;
        }
    }

    void checkEvaluationDataForClip()
    {
        // GIVEN
        QFETCH(Handler *, handler);
        QFETCH(AnimationClipLoader *, clip);
        QFETCH(AnimatorEvaluationData, animatorData);
        QFETCH(ClipEvaluationData, expectedClipData);

        // WHEN
        ClipEvaluationData actualClipData = evaluationDataForClip(clip, animatorData);

        // THEN
        QCOMPARE(actualClipData.currentLoop, expectedClipData.currentLoop);
        QVERIFY(fuzzyCompare(actualClipData.localTime, expectedClipData.localTime) == true);
        QCOMPARE(actualClipData.isFinalFrame, expectedClipData.isFinalFrame);

        // Cleanup
        delete handler;
    }

    void checkEvaluationDataForAnimator_data()
    {
        QTest::addColumn<Handler *>("handler");
        QTest::addColumn<ClipAnimator *>("animator");
        QTest::addColumn<qint64>("globalTimeNS");
        QTest::addColumn<AnimatorEvaluationData>("expectedAnimatorData");

        Handler *handler;
        ClipAnimator *animator;
        qint64 globalTimeNS;
        AnimatorEvaluationData expectedAnimatorData;

        {
            handler = new Handler();
            const qint64 globalStartTimeNS = 0;
            const int loops = 1;
            animator = createClipAnimator(handler, globalStartTimeNS, loops);
            globalTimeNS = 0;

            expectedAnimatorData.loopCount = loops;
            expectedAnimatorData.playbackRate = 1.0; // hard-wired for now
            expectedAnimatorData.startTime = 0.0;
            expectedAnimatorData.globalTime = 0.0;

            QTest::newRow("globalStartTime = 0, globalTime = 0, loops = 1")
                    << handler << animator << globalTimeNS << expectedAnimatorData;
        }

        {
            handler = new Handler();
            const qint64 globalStartTimeNS = 0;
            const int loops = 5;
            animator = createClipAnimator(handler, globalStartTimeNS, loops);
            globalTimeNS = 0;

            expectedAnimatorData.loopCount = loops;
            expectedAnimatorData.playbackRate = 1.0; // hard-wired for now
            expectedAnimatorData.startTime = 0.0;
            expectedAnimatorData.globalTime = 0.0;

            QTest::newRow("globalStartTime = 0, globalTime = 0, loops = 5")
                    << handler << animator << globalTimeNS << expectedAnimatorData;
        }

        {
            handler = new Handler();
            const qint64 globalStartTimeNS = 0;
            const int loops = 1;
            animator = createClipAnimator(handler, globalStartTimeNS, loops);
            globalTimeNS = 5000000000;

            expectedAnimatorData.loopCount = loops;
            expectedAnimatorData.playbackRate = 1.0; // hard-wired for now
            expectedAnimatorData.startTime = 0.0;
            expectedAnimatorData.globalTime = 5.0;

            QTest::newRow("globalStartTime = 0, globalTime = 5, loops = 1")
                    << handler << animator << globalTimeNS << expectedAnimatorData;
        }

        {
            handler = new Handler();
            const qint64 globalStartTimeNS = 3000000000;
            const int loops = 1;
            animator = createClipAnimator(handler, globalStartTimeNS, loops);
            globalTimeNS = 5000000000;

            expectedAnimatorData.loopCount = loops;
            expectedAnimatorData.playbackRate = 1.0; // hard-wired for now
            expectedAnimatorData.startTime = 3.0;
            expectedAnimatorData.globalTime = 5.0;

            QTest::newRow("globalStartTime = 3, globalTime = 5, loops = 1")
                    << handler << animator << globalTimeNS << expectedAnimatorData;
        }
    }

    void checkEvaluationDataForAnimator()
    {
        // GIVEN
        QFETCH(Handler *, handler);
        QFETCH(ClipAnimator *, animator);
        QFETCH(qint64, globalTimeNS);
        QFETCH(AnimatorEvaluationData, expectedAnimatorData);

        // WHEN
        AnimatorEvaluationData actualAnimatorData = evaluationDataForAnimator(animator, globalTimeNS);

        // THEN
        QCOMPARE(actualAnimatorData.loopCount, expectedAnimatorData.loopCount);
        QVERIFY(fuzzyCompare(actualAnimatorData.playbackRate, expectedAnimatorData.playbackRate) == true);
        QVERIFY(fuzzyCompare(actualAnimatorData.startTime, expectedAnimatorData.startTime) == true);
        QVERIFY(fuzzyCompare(actualAnimatorData.globalTime, expectedAnimatorData.globalTime) == true);

        // Cleanup
        delete handler;
    }
};

QTEST_MAIN(tst_AnimationUtils)

#include "tst_animationutils.moc"
