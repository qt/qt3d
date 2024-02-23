// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/qclipanimator.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/private/clipanimator_p.h>
#include <Qt3DAnimation/private/channelmapper_p.h>
#include <Qt3DAnimation/private/channelmapping_p.h>
#include <Qt3DAnimation/private/findrunningclipanimatorsjob_p.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testarbiter.h>

using namespace Qt3DAnimation::Animation;

Q_DECLARE_METATYPE(Qt3DAnimation::Animation::Handler*)
Q_DECLARE_METATYPE(QVector<Qt3DAnimation::Animation::HClipAnimator>)

using MappingDataResults = QHash<ClipAnimator *, QVector<Qt3DAnimation::Animation::MappingData>>;
Q_DECLARE_METATYPE(MappingDataResults)

class tst_FindRunningClipAnimatorsJob: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
public:
    ChannelMapping *createChannelMapping(Handler *handler,
                                         const QString &channelName,
                                         const Qt3DCore::QNodeId targetId,
                                         const char *propertyName,
                                         int type,
                                         int componentCount)
    {
        auto channelMappingId = Qt3DCore::QNodeId::createId();
        ChannelMapping *channelMapping = handler->channelMappingManager()->getOrCreateResource(channelMappingId);
        setPeerId(channelMapping, channelMappingId);
        channelMapping->setHandler(handler);
        channelMapping->setTargetId(targetId);
        channelMapping->setPropertyName(propertyName);
        channelMapping->setChannelName(channelName);
        channelMapping->setType(type);
        channelMapping->setComponentCount(componentCount);
        channelMapping->setMappingType(ChannelMapping::ChannelMappingType);
        return channelMapping;
    }

    ChannelMapper *createChannelMapper(Handler *handler,
                                       const QList<Qt3DCore::QNodeId> &mappingIds)
    {
        auto channelMapperId = Qt3DCore::QNodeId::createId();
        ChannelMapper *channelMapper = handler->channelMapperManager()->getOrCreateResource(channelMapperId);
        setPeerId(channelMapper, channelMapperId);
        channelMapper->setHandler(handler);
        channelMapper->setMappingIds(mappingIds);
        return channelMapper;
    }

    AnimationClip *createAnimationClipLoader(Handler *handler,
                                             const QUrl &source)
    {
        auto clipId = Qt3DCore::QNodeId::createId();
        AnimationClip *clip = handler->animationClipLoaderManager()->getOrCreateResource(clipId);
        setPeerId(clip, clipId);
        clip->setHandler(handler);
        clip->setDataType(AnimationClip::File);
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
        animator->setHandler(handler);
        animator->setStartTime(globalStartTimeNS);
        animator->setLoops(loops);
        return animator;
    }

private Q_SLOTS:
    void checkJob_data()
    {
        QTest::addColumn<Handler *>("handler");
        QTest::addColumn<QVector<HClipAnimator>>("dirtyClipAnimators");
        QTest::addColumn<MappingDataResults>("expectedResults");


        {
            Handler *handler;
            AnimationClip *clip;
            ClipAnimator *animator;
            ChannelMapper *channelMapper;
            MappingDataResults expectedResults;
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));

            const qint64 globalStartTimeNS = 0;
            const int loops = 1;
            animator = createClipAnimator(handler, globalStartTimeNS, loops);
            animator->setClipId(clip->peerId());
            const QVector<HClipAnimator> dirtyClipAnimators
                    = { handler->clipAnimatorManager()->getOrAcquireHandle(animator->peerId()) };

            auto channelMapping = createChannelMapping(handler,
                                                       QLatin1String("Location"),
                                                       Qt3DCore::QNodeId::createId(),
                                                       "translation",
                                                       static_cast<int>(QMetaType::QVector3D),
                                                       3);
            channelMapper = createChannelMapper(handler, QList<Qt3DCore::QNodeId> { channelMapping->peerId() });
            animator->setMapperId(channelMapper->peerId());
            animator->setRunning(true); // Has to be marked as running for the job to process it
            animator->setEnabled(true); // Has to be marked as enabled for the job to process it

            const ComponentIndices locationIndices = { 0, 1, 2 };
            MappingData expectedMapping;
            expectedMapping.targetId = channelMapping->targetId();
            expectedMapping.propertyName = channelMapping->propertyName();
            expectedMapping.type = channelMapping->type();
            expectedMapping.channelIndices = locationIndices;
            expectedResults.insert(animator, QVector<MappingData> { expectedMapping });

            QTest::newRow("single mapping")
                            << handler
                            << dirtyClipAnimators
                            << expectedResults;
        }

        {
            Handler *handler;
            AnimationClip *clip;
            ClipAnimator *animator;
            ChannelMapper *channelMapper;
            MappingDataResults expectedResults;
            handler = new Handler();
            clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));

            const qint64 globalStartTimeNS = 0;
            const int loops = 1;
            animator = createClipAnimator(handler, globalStartTimeNS, loops);
            animator->setClipId(clip->peerId());
            const QVector<HClipAnimator> dirtyClipAnimators
                    = { handler->clipAnimatorManager()->getOrAcquireHandle(animator->peerId()) };

            auto channelMapping = createChannelMapping(handler,
                                                       QLatin1String("Location"),
                                                       Qt3DCore::QNodeId::createId(),
                                                       "translation",
                                                       static_cast<int>(QMetaType::QVector3D),
                                                       3);
            channelMapper = createChannelMapper(handler, QList<Qt3DCore::QNodeId> { channelMapping->peerId() });
            animator->setMapperId(channelMapper->peerId());
            animator->setRunning(true); // Has to be marked as running for the job to process it
            animator->setEnabled(false); // Has to be marked as enabled for the job to process it

            QTest::newRow("disabled animator")
                    << handler
                    << dirtyClipAnimators
                    << expectedResults;
        }
    }

    void checkJob()
    {
        // GIVEN
        QFETCH(Handler *, handler);
        QFETCH(QVector<HClipAnimator>, dirtyClipAnimators);
        QFETCH(MappingDataResults, expectedResults);
        FindRunningClipAnimatorsJob job;

        // WHEN
        job.setHandler(handler);
        job.setDirtyClipAnimators(dirtyClipAnimators);
        job.run();

        // THEN - check the resulting MappingData on the animator matches the expected results
        for (const auto &dirtyClipAnimator : dirtyClipAnimators) {
            const auto animator = handler->clipAnimatorManager()->data(dirtyClipAnimator);
            const QVector<MappingData> actualMappingData = animator->mappingData();
            const QVector<MappingData> expectedMappingData = expectedResults[animator];

            QCOMPARE(expectedMappingData.size(), actualMappingData.size());
            for (int i = 0; i < actualMappingData.size(); ++i) {
                QCOMPARE(expectedMappingData[i].targetId, actualMappingData[i].targetId);
                QCOMPARE(expectedMappingData[i].type, actualMappingData[i].type);
                QVERIFY(qstrcmp(expectedMappingData[i].propertyName, actualMappingData[i].propertyName) == 0);
                QCOMPARE(expectedMappingData[i].channelIndices.size(), actualMappingData[i].channelIndices.size());

                for (int j = 0; j < actualMappingData[i].channelIndices.size(); ++j) {
                    QCOMPARE(expectedMappingData[i].channelIndices[j], actualMappingData[i].channelIndices[j]);
                }
            }
        }
    }
};

QTEST_APPLESS_MAIN(tst_FindRunningClipAnimatorsJob)

#include "tst_findrunningclipanimatorsjob.moc"
