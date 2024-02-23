// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/clipblendnodevisitor_p.h>
#include <Qt3DAnimation/private/lerpclipblend_p.h>
#include <Qt3DAnimation/qlerpclipblend.h>
#include "qbackendnodetester.h"

class tst_ClipBlendNodeVisitor : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkVisitAllNodes()
    {
        // GIVEN
        Qt3DAnimation::QLerpClipBlend rootBlendNode;
        Qt3DAnimation::QLerpClipBlend childBlendNode1(&rootBlendNode);
        Qt3DAnimation::QLerpClipBlend childBlendNode2(&rootBlendNode);
        rootBlendNode.setStartClip(&childBlendNode1);
        rootBlendNode.setEndClip(&childBlendNode2);
        Qt3DAnimation::QLerpClipBlend childBlendNode11(&childBlendNode1);
        Qt3DAnimation::QLerpClipBlend childBlendNode12(&childBlendNode1);
        childBlendNode1.setStartClip(&childBlendNode11);
        childBlendNode1.setEndClip(&childBlendNode12);

        Qt3DAnimation::Animation::LerpClipBlend *backendRootBlendNode = new Qt3DAnimation::Animation::LerpClipBlend();
        Qt3DAnimation::Animation::LerpClipBlend *backendChildBlendNode1 = new Qt3DAnimation::Animation::LerpClipBlend();
        Qt3DAnimation::Animation::LerpClipBlend *backendChildBlendNode2 = new Qt3DAnimation::Animation::LerpClipBlend();
        Qt3DAnimation::Animation::LerpClipBlend *backendChildBlendNode11 = new Qt3DAnimation::Animation::LerpClipBlend();
        Qt3DAnimation::Animation::LerpClipBlend *backendChildBlendNode12 = new Qt3DAnimation::Animation::LerpClipBlend();

        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        backendRootBlendNode->setClipBlendNodeManager(&manager);
        backendChildBlendNode1->setClipBlendNodeManager(&manager);
        backendChildBlendNode2->setClipBlendNodeManager(&manager);
        backendChildBlendNode11->setClipBlendNodeManager(&manager);
        backendChildBlendNode12->setClipBlendNodeManager(&manager);

        manager.appendNode(rootBlendNode.id(), backendRootBlendNode);
        manager.appendNode(childBlendNode1.id(), backendChildBlendNode1);
        manager.appendNode(childBlendNode2.id(), backendChildBlendNode2);
        manager.appendNode(childBlendNode11.id(), backendChildBlendNode11);
        manager.appendNode(childBlendNode12.id(), backendChildBlendNode12);

        // WHEN
        simulateInitializationSync(&rootBlendNode, backendRootBlendNode);
        simulateInitializationSync(&childBlendNode1, backendChildBlendNode1);
        simulateInitializationSync(&childBlendNode2, backendChildBlendNode2);
        simulateInitializationSync(&childBlendNode11, backendChildBlendNode11);
        simulateInitializationSync(&childBlendNode12, backendChildBlendNode12);

        // THEN
        QCOMPARE(backendRootBlendNode->allDependencyIds().size(), 2);
        QCOMPARE(backendChildBlendNode1->allDependencyIds().size(), 2);
        QCOMPARE(backendChildBlendNode2->allDependencyIds().size(), 2);
        QCOMPARE(backendChildBlendNode11->allDependencyIds().size(), 2);
        QCOMPARE(backendChildBlendNode12->allDependencyIds().size(), 2);

        // WHEN
        int i = 0;
        // Note: post-order traversal
        auto childCounter = [&] (Qt3DAnimation::Animation::ClipBlendNode *node) {
            if (i == 0)
                QCOMPARE(node, backendChildBlendNode11);
            else if (i == 1)
                QCOMPARE(node, backendChildBlendNode12);
            else if (i == 2)
                QCOMPARE(node, backendChildBlendNode1);
            else if (i == 3)
                QCOMPARE(node, backendChildBlendNode2);
            else if (i == 4)
                QCOMPARE(node, backendRootBlendNode);
            ++i;
        };

        // THEN -> no comparison failure
        Qt3DAnimation::Animation::ClipBlendNodeVisitor visitor(&manager);
        visitor.traverse(rootBlendNode.id(), childCounter);
    }

    void checkDoesntCrashIfRootNodeIsNotFound()
    {
        // GIVEN
        Qt3DAnimation::QLerpClipBlend rootBlendNode;

        Qt3DAnimation::Animation::LerpClipBlend *backendRootBlendNode = new Qt3DAnimation::Animation::LerpClipBlend();

        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        backendRootBlendNode->setClipBlendNodeManager(&manager);

        // We purposely forgot the to do:  manager.appendNode(rootBlendNode.id(), backendRootBlendNode);

        // WHEN
        simulateInitializationSync(&rootBlendNode, backendRootBlendNode);

        // THEN
        QCOMPARE(backendRootBlendNode->allDependencyIds().size(), 2);

        // WHEN
        auto childCounter = [] (Qt3DAnimation::Animation::ClipBlendNode *) {};

        // THEN -> shouldn't crash
        Qt3DAnimation::Animation::ClipBlendNodeVisitor visitor(&manager);
        visitor.traverse(rootBlendNode.id(), childCounter);
    }

    void checkDoesntCrashIfChildNodeIsNotFound()
    {
        // GIVEN
        Qt3DAnimation::QLerpClipBlend rootBlendNode;
        Qt3DAnimation::QLerpClipBlend childBlendNode1(&rootBlendNode);
        Qt3DAnimation::QLerpClipBlend childBlendNode2(&rootBlendNode);
        rootBlendNode.setStartClip(&childBlendNode1);
        rootBlendNode.setEndClip(&childBlendNode2);

        Qt3DAnimation::Animation::LerpClipBlend *backendRootBlendNode = new Qt3DAnimation::Animation::LerpClipBlend();
        Qt3DAnimation::Animation::LerpClipBlend *backendChildBlendNode1 = new Qt3DAnimation::Animation::LerpClipBlend();
        Qt3DAnimation::Animation::LerpClipBlend *backendChildBlendNode2 = new Qt3DAnimation::Animation::LerpClipBlend();

        Qt3DAnimation::Animation::ClipBlendNodeManager manager;
        backendRootBlendNode->setClipBlendNodeManager(&manager);
        backendChildBlendNode1->setClipBlendNodeManager(&manager);
        backendChildBlendNode2->setClipBlendNodeManager(&manager);

        manager.appendNode(rootBlendNode.id(), backendRootBlendNode);
        // We purposely forgot the to do:
        // manager.appendNode(childBlendNode1.id(), backendChildBlendNode1);
        // manager.appendNode(childBlendNode2.id(), backendChildBlendNode2);


        // WHEN
        simulateInitializationSync(&rootBlendNode, backendRootBlendNode);
        simulateInitializationSync(&childBlendNode1, backendChildBlendNode1);
        simulateInitializationSync(&childBlendNode2, backendChildBlendNode2);

        // THEN
        QCOMPARE(backendRootBlendNode->allDependencyIds().size(), 2);
        QCOMPARE(backendChildBlendNode1->allDependencyIds().size(), 2);
        QCOMPARE(backendChildBlendNode2->allDependencyIds().size(), 2);

        // WHEN
        int i = 0;
        auto childCounter = [&] (Qt3DAnimation::Animation::ClipBlendNode *) {
            ++i;
        };

        // THEN -> shouldn't crash
        Qt3DAnimation::Animation::ClipBlendNodeVisitor visitor(&manager);
        visitor.traverse(rootBlendNode.id(), childCounter);
        QCOMPARE(i, 1);
    }

};

QTEST_MAIN(tst_ClipBlendNodeVisitor)

#include "tst_clipblendnodevisitor.moc"
