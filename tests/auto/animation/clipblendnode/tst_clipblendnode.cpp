/****************************************************************************
**
** Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
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
#include <Qt3DAnimation/qlerpclipblend.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qabstractclipblendnode_p.h>
#include <Qt3DAnimation/private/clipblendnode_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include "qbackendnodetester.h"

#include <random>
#include <algorithm>

using namespace Qt3DAnimation::Animation;

namespace {

class TestClipBlendNode : public ClipBlendNode
{
public:
    TestClipBlendNode()
        : ClipBlendNode(ClipBlendNode::LerpBlendType)
    {}

    float blend(float , float ) const Q_DECL_FINAL { return 0.0f; }

    QVector<Qt3DCore::QNodeId> dependencyIds() const Q_DECL_FINAL
    {
        return QVector<Qt3DCore::QNodeId>();
    }

    using ClipBlendNode::setClipResults;
};

} // anonymous

Q_DECLARE_METATYPE(TestClipBlendNode *)

class tst_ClipBlendNode : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        TestClipBlendNode backendClipBlendNode;

        // THEN
        QCOMPARE(backendClipBlendNode.isEnabled(), false);
        QVERIFY(backendClipBlendNode.peerId().isNull());
        QCOMPARE(backendClipBlendNode.clipIds(), Qt3DCore::QNodeIdVector());
        QCOMPARE(backendClipBlendNode.parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode.childrenIds(), Qt3DCore::QNodeIdVector());
        QVERIFY(backendClipBlendNode.clipBlendNodeManager() == nullptr);
        QCOMPARE(backendClipBlendNode.blendType(), ClipBlendNode::LerpBlendType);
        QCOMPARE(backendClipBlendNode.clipResults(Qt3DCore::QNodeId()), ClipResults());
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QLerpClipBlend clipBlendNode;
        Qt3DAnimation::QAnimationClipLoader clip;
        clipBlendNode.addClip(&clip);

        QCoreApplication::processEvents();

        {
            // WHEN
            ClipBlendNodeManager manager;
            TestClipBlendNode backendClipBlendNode;
            backendClipBlendNode.setClipBlendNodeManager(&manager);
            simulateInitialization(&clipBlendNode, &backendClipBlendNode);

            // THEN
            QCOMPARE(backendClipBlendNode.isEnabled(), true);
            QCOMPARE(backendClipBlendNode.peerId(), clipBlendNode.id());
            QCOMPARE(backendClipBlendNode.clipIds().size(), 1);
            QCOMPARE(backendClipBlendNode.clipIds().first(), clip.id());
            QCOMPARE(backendClipBlendNode.parentId(), Qt3DCore::QNodeId());
            QCOMPARE(backendClipBlendNode.childrenIds().size(), 0);
            QCOMPARE(backendClipBlendNode.clipBlendNodeManager(), &manager);
            QCOMPARE(backendClipBlendNode.blendType(), ClipBlendNode::LerpBlendType);
            QCOMPARE(backendClipBlendNode.clipResults(Qt3DCore::QNodeId()), ClipResults());
        }
        {
            // WHEN
            ClipBlendNodeManager manager;
            TestClipBlendNode backendClipBlendNode;
            clipBlendNode.setEnabled(false);
            backendClipBlendNode.setClipBlendNodeManager(&manager);
            simulateInitialization(&clipBlendNode, &backendClipBlendNode);

            // THEN
            QCOMPARE(backendClipBlendNode.peerId(), clipBlendNode.id());
            QCOMPARE(backendClipBlendNode.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        TestClipBlendNode backendClipBlendNode;
        ClipBlendNodeManager manager;
        backendClipBlendNode.setClipBlendNodeManager(&manager);
        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendClipBlendNode.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendClipBlendNode.isEnabled(), newValue);
        }
        {
            // WHEN
            Qt3DAnimation::QAnimationClipLoader clip;
            // To geneate the type_info in the QNodePrivate of clip
            Qt3DCore::QNodeCreatedChangeGenerator generator(&clip);

            const auto addedChange = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &clip);
            backendClipBlendNode.sceneChangeEvent(addedChange);

            // THEN
            QCOMPARE(backendClipBlendNode.clipIds().size(), 1);
            QCOMPARE(backendClipBlendNode.clipIds().first(), clip.id());

            // WHEN
            const auto removedChange = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &clip);
            backendClipBlendNode.sceneChangeEvent(removedChange);

            // THEN
            QCOMPARE(backendClipBlendNode.clipIds().size(), 0);
        }
        {
            // WHEN
            Qt3DAnimation::QLerpClipBlend clipBlendChild;
            // Will be destroyed when manager is destroyed
            TestClipBlendNode *backenChildClipBlendNode = new TestClipBlendNode();
            backendClipBlendNode.setClipBlendNodeManager(&manager);
            manager.appendNode(clipBlendChild.id(), backenChildClipBlendNode);

            // To geneate the type_info in the QNodePrivate of clipBlendChild
            Qt3DCore::QNodeCreatedChangeGenerator generator(&clipBlendChild);
            const auto addChange = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &clipBlendChild);
            backendClipBlendNode.sceneChangeEvent(addChange);

            // THEN
            QCOMPARE(backendClipBlendNode.childrenIds().size(), 1);
            QCOMPARE(backendClipBlendNode.childrenIds().first(), clipBlendChild.id());

            // WHEN
            const auto removedChange = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &clipBlendChild);
            backendClipBlendNode.sceneChangeEvent(removedChange);

            // THEN
            QCOMPARE(backendClipBlendNode.childrenIds().size(), 0);
        }
    }

    void checkParentInitialization()
    {
        // GIVEN
        TestClipBlendNode *backendClipBlendNode = new TestClipBlendNode();
        TestClipBlendNode *backendChildClipBlendNode = new TestClipBlendNode();
        Qt3DAnimation::QLerpClipBlend clipBlendParent;
        Qt3DAnimation::QLerpClipBlend childClipBlend(&clipBlendParent);
        ClipBlendNodeManager manager;
        backendClipBlendNode->setClipBlendNodeManager(&manager);
        backendChildClipBlendNode->setClipBlendNodeManager(&manager);

        // THEN
        QCOMPARE(backendClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode->childrenIds().size(), 0);
        QCOMPARE(backendChildClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendChildClipBlendNode->childrenIds().size(), 0);

        // WHEN
        manager.appendNode(clipBlendParent.id(), backendClipBlendNode);
        manager.appendNode(childClipBlend.id(), backendChildClipBlendNode);
        simulateInitialization(&clipBlendParent, backendClipBlendNode);
        simulateInitialization(&childClipBlend, backendChildClipBlendNode);

        // THEN
        QCOMPARE(backendClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode->childrenIds().size(), 1);
        QCOMPARE(backendClipBlendNode->childrenIds().first(), childClipBlend.id());
        QCOMPARE(backendChildClipBlendNode->parentId(), clipBlendParent.id());
        QCOMPARE(backendChildClipBlendNode->childrenIds().size(), 0);

        // WHEN
        // To geneate the type_info in the QNodePrivate of clipBlendChild
        Qt3DCore::QNodeCreatedChangeGenerator generator(&childClipBlend);
        const auto removedChange = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &childClipBlend);
        backendClipBlendNode->sceneChangeEvent(removedChange);

        // THEN
        QCOMPARE(backendClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendClipBlendNode->childrenIds().size(), 0);
        QCOMPARE(backendChildClipBlendNode->parentId(), Qt3DCore::QNodeId());
        QCOMPARE(backendChildClipBlendNode->childrenIds().size(), 0);
    }

    void checkClipResults_data()
    {
        QTest::addColumn<TestClipBlendNode *>("blendNode");
        QTest::addColumn<QVector<int>>("indexes");
        QTest::addColumn<QVector<Qt3DCore::QNodeId>>("animatorIds");
        QTest::addColumn<QVector<ClipResults>>("expectedClipResults");

        // Single entry
        {
            auto blendNode = new TestClipBlendNode;
            QVector<Qt3DCore::QNodeId> animatorIds;
            QVector<ClipResults> expectedClipResults;

            const auto animatorId = Qt3DCore::QNodeId::createId();
            animatorIds.push_back(animatorId);

            ClipResults clipResults = { 0.0f, 1.0f, 2.0f };
            for (int i = 0; i < 3; ++i)
                clipResults.push_back(float(i));
            expectedClipResults.push_back(clipResults);

            // Set data and indexes
            blendNode->setClipResults(animatorId, clipResults);
            QVector<int> indexes = QVector<int>() << 0;

            QTest::newRow("single entry")
                    << blendNode << indexes << animatorIds << expectedClipResults;
        }

        // No data
        {
            auto blendNode = new TestClipBlendNode;
            QVector<Qt3DCore::QNodeId> animatorIds;
            QVector<ClipResults> expectedClipResults;

            auto animatorId = Qt3DCore::QNodeId::createId();
            animatorIds.push_back(animatorId);

            ClipResults clipResults;
            expectedClipResults.push_back(clipResults);

            // Don't set any data
            QVector<int> indexes = QVector<int>() << 0;

            QTest::newRow("no entries")
                    << blendNode << indexes << animatorIds << expectedClipResults;
        }

        // Multiple entries, ordered
        {
            auto blendNode = new TestClipBlendNode;
            QVector<Qt3DCore::QNodeId> animatorIds;
            QVector<ClipResults> expectedClipResults;

            const int animatorCount = 10;
            for (int j = 0; j < animatorCount; ++j) {
                auto animatorId = Qt3DCore::QNodeId::createId();
                animatorIds.push_back(animatorId);

                ClipResults clipResults;
                for (int i = 0; i < j + 5; ++i)
                    clipResults.push_back(float(i + j));
                expectedClipResults.push_back(clipResults);

                blendNode->setClipResults(animatorId, clipResults);
            }

            QVector<int> indexes(animatorCount);
            std::iota(indexes.begin(), indexes.end(), 0);

            QTest::newRow("multiple entries, ordered")
                    << blendNode << indexes << animatorIds << expectedClipResults;
        }

        // Multiple entries, unordered
        {
            auto blendNode = new TestClipBlendNode;
            QVector<Qt3DCore::QNodeId> animatorIds;
            QVector<ClipResults> expectedClipResults;

            const int animatorCount = 10;
            for (int j = 0; j < animatorCount; ++j) {
                auto animatorId = Qt3DCore::QNodeId::createId();
                animatorIds.push_back(animatorId);

                ClipResults clipResults;
                for (int i = 0; i < j + 5; ++i)
                    clipResults.push_back(float(i + j));
                expectedClipResults.push_back(clipResults);

                blendNode->setClipResults(animatorId, clipResults);
            }

            // Shuffle the animatorIds to randomise the lookups
            QVector<int> indexes(animatorCount);
            std::iota(indexes.begin(), indexes.end(), 0);
            std::random_device rd;
            std::mt19937 generator(rd());
            std::shuffle(indexes.begin(), indexes.end(), generator);

            QTest::newRow("multiple entries, unordered")
                    << blendNode << indexes << animatorIds << expectedClipResults;
        }
    }

    void checkClipResults()
    {
        // GIVEN
        QFETCH(TestClipBlendNode *, blendNode);
        QFETCH(QVector<int>, indexes);
        QFETCH(QVector<Qt3DCore::QNodeId>, animatorIds);
        QFETCH(QVector<ClipResults>, expectedClipResults);

        for (int i = 0; i < indexes.size(); ++i) {
            // WHEN
            const int index = indexes[i];
            const ClipResults actualClipResults = blendNode->clipResults(animatorIds[index]);

            // THEN
            QCOMPARE(actualClipResults.size(), expectedClipResults[index].size());
            for (int j = 0; j < actualClipResults.size(); ++j)
                QCOMPARE(actualClipResults[j], expectedClipResults[index][j]);
        }

        delete blendNode;
    }
};

QTEST_MAIN(tst_ClipBlendNode)

#include "tst_clipblendnode.moc"
