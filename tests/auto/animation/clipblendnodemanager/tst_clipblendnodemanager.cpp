// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/clipblendnode_p.h>

namespace {

int deadCount = 0;

class TestClipBlendNode : public Qt3DAnimation::Animation::ClipBlendNode
{
public:
    TestClipBlendNode()
        : Qt3DAnimation::Animation::ClipBlendNode(Qt3DAnimation::Animation::ClipBlendNode::NoneBlendType)
    {}

    ~TestClipBlendNode()
    {
        deadCount += 1;
    }

    inline QList<Qt3DCore::QNodeId> allDependencyIds() const override
    {
        return currentDependencyIds();
    }

    QList<Qt3DCore::QNodeId> currentDependencyIds() const final
    {
        return {};
    }

    double duration() const final { return 0.0f; }

protected:
    Qt3DAnimation::Animation::ClipResults doBlend(const QList<Qt3DAnimation::Animation::ClipResults> &) const final
    {
        return Qt3DAnimation::Animation::ClipResults();
    }
};

} // anonymous

class tst_ClipBlendNodeManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkCointainsNode()
    {
        // GIVEN
        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        const Qt3DCore::QNodeId nodeId = Qt3DCore::QNodeId::createId();

        // THEN
        QVERIFY(manager.containsNode(nodeId) == false);

        // WHEN
        TestClipBlendNode *node = new TestClipBlendNode();
        manager.appendNode(nodeId, node);

        // THEN
        QVERIFY(manager.containsNode(nodeId));
    }

    void checkAppendNode()
    {
        // GIVEN
        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        const Qt3DCore::QNodeId nodeId = Qt3DCore::QNodeId::createId();
        const Qt3DCore::QNodeId nodeId2 = Qt3DCore::QNodeId::createId();
        TestClipBlendNode *node = new TestClipBlendNode();
        TestClipBlendNode *node2 = new TestClipBlendNode();

        // WHEN
        manager.appendNode(nodeId, node);
        manager.appendNode(nodeId2, node2);

        // THEN
        QVERIFY(manager.containsNode(nodeId));
        QVERIFY(manager.containsNode(nodeId2));
    }

    void checkLookupNode()
    {
        // GIVEN
        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        const Qt3DCore::QNodeId nodeId = Qt3DCore::QNodeId::createId();
        const Qt3DCore::QNodeId nodeId2 = Qt3DCore::QNodeId::createId();
        TestClipBlendNode *node = new TestClipBlendNode();
        TestClipBlendNode *node2 = new TestClipBlendNode();

        // WHEN
        manager.appendNode(nodeId, node);
        manager.appendNode(nodeId2, node2);
        Qt3DAnimation::Animation::ClipBlendNode *lookedUpNode = manager.lookupNode(nodeId);

        // THEN
        QCOMPARE(lookedUpNode, node);

        // WHEN
        lookedUpNode = manager.lookupNode(nodeId2);

        // THEN
        QCOMPARE(lookedUpNode, node2);

        // WHEN
        lookedUpNode = manager.lookupNode(Qt3DCore::QNodeId::createId());

        // THEN
        QVERIFY(lookedUpNode == nullptr);
    }

    void checkReleaseNode()
    {
        // GIVEN
        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        const Qt3DCore::QNodeId nodeId = Qt3DCore::QNodeId::createId();
        TestClipBlendNode *node = new TestClipBlendNode();
        deadCount = 0;

        // WHEN
        manager.appendNode(nodeId, node);

        // THEN
        QCOMPARE(deadCount, 0);

        // WHEN
        manager.releaseNode(nodeId);

        // THEN
        QCOMPARE(deadCount, 1);
    }

    void checkDestruction()
    {
        // GIVEN
        const Qt3DCore::QNodeId nodeId = Qt3DCore::QNodeId::createId();
        const Qt3DCore::QNodeId nodeId2 = Qt3DCore::QNodeId::createId();
        TestClipBlendNode *node = new TestClipBlendNode();
        TestClipBlendNode *node2 = new TestClipBlendNode();

        // WHEN
        {
            Qt3DAnimation::Animation::ClipBlendNodeManager manager;
            deadCount = 0;

            // WHEN
            manager.appendNode(nodeId, node);
            manager.appendNode(nodeId2, node2);

            // THEN
            QCOMPARE(deadCount, 0);
        }

        // THEN
        QCOMPARE(deadCount, 2);
    }
};

QTEST_MAIN(tst_ClipBlendNodeManager)

#include "tst_clipblendnodemanager.moc"
