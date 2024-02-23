// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DAnimation/qclipblendvalue.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qclipblendvalue_p.h>
#include <Qt3DAnimation/private/clipblendvalue_p.h>
#include "qbackendnodetester.h"

#include <random>
#include <algorithm>

using namespace Qt3DAnimation::Animation;

Q_DECLARE_METATYPE(ClipBlendValue *)
Q_DECLARE_METATYPE(QList<ClipFormat>)

class tst_ClipBlendValue : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
public:
    AnimationClip *createAnimationClipLoader(Handler *handler,
                                                   double duration)
    {
        auto clipId = Qt3DCore::QNodeId::createId();
        AnimationClip *clip = handler->animationClipLoaderManager()->getOrCreateResource(clipId);
        setPeerId(clip, clipId);
        clip->setDuration(duration);
        return clip;
    }

private Q_SLOTS:
    void checkInitialState()
    {
        // GIVEN
        ClipBlendValue backendClipBlendValue;

        // THEN
        QCOMPARE(backendClipBlendValue.isEnabled(), false);
        QVERIFY(backendClipBlendValue.peerId().isNull());
        QCOMPARE(backendClipBlendValue.clipId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendValue.blendType(), ClipBlendNode::ValueType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QClipBlendValue clipBlendValue;
        Qt3DAnimation::QAnimationClipLoader clip;
        clipBlendValue.setClip(&clip);

        {
            // WHEN
            ClipBlendValue backendClipBlendValue;
            simulateInitializationSync(&clipBlendValue, &backendClipBlendValue);

            // THEN
            QCOMPARE(backendClipBlendValue.isEnabled(), true);
            QCOMPARE(backendClipBlendValue.peerId(), clipBlendValue.id());
            QCOMPARE(backendClipBlendValue.clipId(), clip.id());
        }
        {
            // WHEN
            ClipBlendValue backendClipBlendValue;
            clipBlendValue.setEnabled(false);
            simulateInitializationSync(&clipBlendValue, &backendClipBlendValue);

            // THEN
            QCOMPARE(backendClipBlendValue.peerId(), clipBlendValue.id());
            QCOMPARE(backendClipBlendValue.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DAnimation::QClipBlendValue clipBlendValue;
        ClipBlendValue backendClipBlendValue;
        simulateInitializationSync(&clipBlendValue, &backendClipBlendValue);
        {
            // WHEN
            const bool newValue = false;
            clipBlendValue.setEnabled(newValue);
            backendClipBlendValue.syncFromFrontEnd(&clipBlendValue, false);

            // THEN
            QCOMPARE(backendClipBlendValue.isEnabled(), newValue);
        }
        {
            // WHEN
            Qt3DAnimation::QAnimationClipLoader newValue;
            clipBlendValue.setClip(&newValue);
            backendClipBlendValue.syncFromFrontEnd(&clipBlendValue, false);

            // THEN
            QCOMPARE(backendClipBlendValue.clipId(), newValue.id());
        }
    }

    void checkDependencyIds()
    {
        // GIVEN
        ClipBlendValue clipNode;
        auto clipId = Qt3DCore::QNodeId::createId();

        // WHEN
        clipNode.setClipId(clipId);
        QList<Qt3DCore::QNodeId> actualIds = clipNode.currentDependencyIds();

        // THEN
        QCOMPARE(actualIds.size(), 0);

        // WHEN
        auto anotherClipId = Qt3DCore::QNodeId::createId();
        clipNode.setClipId(anotherClipId);
        actualIds = clipNode.currentDependencyIds();

        // THEN
        QCOMPARE(actualIds.size(), 0);
    }

    void checkDuration()
    {
        // GIVEN
        auto handler = new Handler();
        const double expectedDuration = 123.5;
        auto clip = createAnimationClipLoader(handler, expectedDuration);
        ClipBlendValue clipNode;
        clipNode.setHandler(handler);
        clipNode.setClipBlendNodeManager(handler->clipBlendNodeManager());
        clipNode.setClipId(clip->peerId());

        // WHEN
        double actualDuration = clipNode.duration();

        // THEN
        QCOMPARE(actualDuration, expectedDuration);
    }

    void checkFormatIndices_data()
    {
        QTest::addColumn<ClipBlendValue *>("blendNode");
        QTest::addColumn<QList<int>>("indexes");
        QTest::addColumn<QList<Qt3DCore::QNodeId>>("animatorIds");
        QTest::addColumn<QList<ClipFormat>>("expectedClipFormat");

        // Single entry
        {
            auto blendNode = new ClipBlendValue;
            QList<Qt3DCore::QNodeId> animatorIds;
            QList<ClipFormat> expectedClipFormat;

            const auto animatorId = Qt3DCore::QNodeId::createId();
            animatorIds.push_back(animatorId);

            ClipFormat clipFormat;
            clipFormat.sourceClipIndices = { 0, 1, 2 };
            expectedClipFormat.push_back(clipFormat);

            // Set data and indexes
            blendNode->setClipFormat(animatorId, clipFormat);
            QList<int> indexes = { 0 };

            QTest::newRow("single entry")
                    << blendNode << indexes << animatorIds << expectedClipFormat;
        }

        // Multiple entries, ordered
        {
            auto blendNode = new ClipBlendValue;
            QList<Qt3DCore::QNodeId> animatorIds;
            QList<ClipFormat> expectedClipFormat;

            const int animatorCount = 10;
            for (int j = 0; j < animatorCount; ++j) {
                auto animatorId = Qt3DCore::QNodeId::createId();
                animatorIds.push_back(animatorId);

                ClipFormat clipFormat;
                for (int i = 0; i < j + 5; ++i)
                    clipFormat.sourceClipIndices.push_back(i + j);
                expectedClipFormat.push_back(clipFormat);

                blendNode->setClipFormat(animatorId, clipFormat);
            }

            QVector<int> indexes(animatorCount);
            std::iota(indexes.begin(), indexes.end(), 0);

            QTest::newRow("multiple entries, ordered")
                    << blendNode << indexes << animatorIds << expectedClipFormat;
        }

        // Multiple entries, unordered
        {
            auto blendNode = new ClipBlendValue;
            QList<Qt3DCore::QNodeId> animatorIds;
            QList<ClipFormat> expectedClipFormat;

            const int animatorCount = 10;
            for (int j = 0; j < animatorCount; ++j) {
                auto animatorId = Qt3DCore::QNodeId::createId();
                animatorIds.push_back(animatorId);

                ClipFormat clipFormat;
                for (int i = 0; i < j + 5; ++i)
                    clipFormat.sourceClipIndices.push_back(i + j);
                expectedClipFormat.push_back(clipFormat);

                blendNode->setClipFormat(animatorId, clipFormat);
            }

            // Shuffle the animatorIds to randomise the lookups
            QVector<int> indexes(animatorCount);
            std::iota(indexes.begin(), indexes.end(), 0);
            std::random_device rd;
            std::mt19937 generator(rd());
            std::shuffle(indexes.begin(), indexes.end(), generator);

            QTest::newRow("multiple entries, unordered")
                    << blendNode << indexes << animatorIds << expectedClipFormat;
        }
    }

    void checkFormatIndices()
    {
        // GIVEN
        QFETCH(ClipBlendValue *, blendNode);
        QFETCH(QList<int>, indexes);
        QFETCH(QList<Qt3DCore::QNodeId>, animatorIds);
        QFETCH(QList<ClipFormat>, expectedClipFormat);

        for (int i = 0; i < indexes.size(); ++i) {
            // WHEN
            const int index = indexes[i];
            const ClipFormat actualClipFormat = blendNode->clipFormat(animatorIds[index]);

            // THEN
            QCOMPARE(actualClipFormat.sourceClipIndices.size(), expectedClipFormat[index].sourceClipIndices.size());
            for (int j = 0; j < actualClipFormat.sourceClipIndices.size(); ++j)
                QCOMPARE(actualClipFormat.sourceClipIndices[j], expectedClipFormat[index].sourceClipIndices[j]);
        }

        delete blendNode;
    }
};

QTEST_MAIN(tst_ClipBlendValue)

#include "tst_clipblendvalue.moc"
