// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <Qt3DRender/private/framegraphnode_p.h>
#include <QtTest/QTest>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include "testrenderer.h"
#include "qbackendnodetester.h"

class MyFrameGraphNode : public Qt3DRender::Render::FrameGraphNode
{
public:
    void setEnabled(bool enabled)
    {
        FrameGraphNode::setEnabled(enabled);
    }
};

class MyQFrameGraphNode : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
public:
    MyQFrameGraphNode(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QFrameGraphNode(parent)
    {}
};

class tst_FrameGraphNode : public  Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
public:
    tst_FrameGraphNode(QObject *parent = nullptr)
        : Qt3DCore::QBackendNodeTester(parent)
    {}

    ~tst_FrameGraphNode()
    {}

    void setIdInternal(Qt3DRender::Render::FrameGraphNode *node, Qt3DCore::QNodeId id)
    {
        Qt3DCore::QBackendNodePrivate::get(node)->m_peerId = id;
    }

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        MyFrameGraphNode n;

        // THEN
        QCOMPARE(n.nodeType(), Qt3DRender::Render::FrameGraphNode::InvalidNodeType);
        QVERIFY(!n.isEnabled());
        QVERIFY(n.peerId().isNull());
        QVERIFY(n.manager() == nullptr);
        QVERIFY(n.parentId().isNull());
        QVERIFY(n.childrenIds().empty());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        QScopedPointer<MyFrameGraphNode> n(new MyFrameGraphNode());

        // WHEN
        n->setEnabled(true);
        // THEN
        QCOMPARE(n->isEnabled(), true);

        // WHEN
        QScopedPointer<Qt3DRender::Render::FrameGraphManager> manager(new Qt3DRender::Render::FrameGraphManager());
        n->setFrameGraphManager(manager.data());
        // THEN
        QCOMPARE(n->manager(), manager.data());

        // WHEN
        const Qt3DCore::QNodeId parentId = Qt3DCore::QNodeId::createId();
        // THEN
        QVERIFY(!parentId.isNull());

        // WHEN
        n->setParentId(parentId);

        // THEN
        QCOMPARE(n->parentId(), parentId);
    }

    void checkParentChange()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::Render::FrameGraphManager> manager(new Qt3DRender::Render::FrameGraphManager());
        const Qt3DCore::QNodeId parentId = Qt3DCore::QNodeId::createId();
        const Qt3DCore::QNodeId childId = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::FrameGraphNode *parent1 = new MyFrameGraphNode();
        Qt3DRender::Render::FrameGraphNode *child = new MyFrameGraphNode();

        setIdInternal(parent1, parentId);
        setIdInternal(child, childId);

        manager->appendNode(parentId, parent1);
        manager->appendNode(childId, child);

        parent1->setFrameGraphManager(manager.data());
        child->setFrameGraphManager(manager.data());

        // THEN
        QVERIFY(parent1->childrenIds().isEmpty());
        QVERIFY(child->parentId().isNull());

        // WHEN
        child->setParentId(parentId);

        // THEN
        QCOMPARE(child->parentId(), parentId);
        QCOMPARE(child->parent(), parent1);
        QCOMPARE(parent1->childrenIds().size(), 1);
        QCOMPARE(parent1->childrenIds().first(), childId);
        QCOMPARE(parent1->children().size(), parent1->childrenIds().size());
        QCOMPARE(parent1->children().first(), child);

        // WHEN
        child->setParentId(Qt3DCore::QNodeId());

        // THEN
        QVERIFY(child->parentId().isNull());
        QVERIFY(child->parent() == nullptr);
        QCOMPARE(parent1->childrenIds().size(), 0);
        QCOMPARE(parent1->children().size(), parent1->childrenIds().size());
    }

    void checkSetParent()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::Render::FrameGraphManager> manager(new Qt3DRender::Render::FrameGraphManager());
        const Qt3DCore::QNodeId parent1Id = Qt3DCore::QNodeId::createId();
        const Qt3DCore::QNodeId parent2Id = Qt3DCore::QNodeId::createId();
        const Qt3DCore::QNodeId childId = Qt3DCore::QNodeId::createId();

        Qt3DRender::Render::FrameGraphNode *parent1 = new MyFrameGraphNode();
        Qt3DRender::Render::FrameGraphNode *parent2 = new MyFrameGraphNode();
        Qt3DRender::Render::FrameGraphNode *child = new MyFrameGraphNode();

        setIdInternal(parent1, parent1Id);
        setIdInternal(parent2, parent2Id);
        setIdInternal(child, childId);

        manager->appendNode(parent1Id, parent1);
        manager->appendNode(parent2Id, parent2);
        manager->appendNode(childId, child);

        parent1->setFrameGraphManager(manager.data());
        parent2->setFrameGraphManager(manager.data());
        child->setFrameGraphManager(manager.data());

        // THEN
        QCOMPARE(parent1->peerId(), parent1Id);
        QCOMPARE(parent2->peerId(), parent2Id);
        QCOMPARE(child->peerId(), childId);

        QVERIFY(child->parentId().isNull());
        QCOMPARE(parent1->childrenIds().size(), 0);
        QCOMPARE(parent2->childrenIds().size(), 0);

        // WHEN
        child->setParentId(parent1Id);

        // THEN
        QCOMPARE(child->parentId(), parent1Id);
        QCOMPARE(parent1->childrenIds().size(), 1);
        QCOMPARE(parent2->childrenIds().size(), 0);

        // WHEN
        child->setParentId(parent2Id);

        // THEN
        QCOMPARE(child->parentId(), parent2Id);
        QCOMPARE(parent1->childrenIds().size(), 0);
        QCOMPARE(parent2->childrenIds().size(), 1);

        // WHEN
        child->setParentId(Qt3DCore::QNodeId());

        // THEN
        QVERIFY(child->parentId().isNull());
        QCOMPARE(parent1->childrenIds().size(), 0);
        QCOMPARE(parent2->childrenIds().size(), 0);
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QFrameGraphNode *frontendFGNode = new MyQFrameGraphNode();
        Qt3DRender::QFrameGraphNode *frontendFGChild = new MyQFrameGraphNode();
        Qt3DRender::Render::FrameGraphNode *backendFGNode = new MyFrameGraphNode();
        Qt3DRender::Render::FrameGraphNode *backendFGChild = new MyFrameGraphNode();

        QScopedPointer<Qt3DRender::Render::FrameGraphManager> manager(new Qt3DRender::Render::FrameGraphManager());

        TestRenderer renderer;

        backendFGNode->setRenderer(&renderer);
        backendFGChild->setRenderer(&renderer);

        setIdInternal(backendFGNode, frontendFGNode->id());
        setIdInternal(backendFGChild, frontendFGChild->id());

        manager->appendNode(frontendFGNode->id(), backendFGNode);
        manager->appendNode(frontendFGChild->id(), backendFGChild);

        backendFGNode->setFrameGraphManager(manager.data());
        backendFGChild->setFrameGraphManager(manager.data());

        simulateInitializationSync(frontendFGNode, backendFGNode);
        simulateInitializationSync(frontendFGChild, backendFGChild);

        QCOMPARE(backendFGNode->childrenIds().size(), 0);

        {
            // WHEN
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            frontendFGChild->setParent(frontendFGNode);
            backendFGChild->syncFromFrontEnd(frontendFGChild, false);

            // THEN
            QCOMPARE(backendFGNode->childrenIds().size(), 1);
            QCOMPARE(backendFGChild->parentId(), frontendFGNode->id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
        {
            // WHEN
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            frontendFGChild->setParent(Q_NODE_NULLPTR);
            backendFGChild->syncFromFrontEnd(frontendFGChild, false);

            // THEN
            QCOMPARE(backendFGNode->childrenIds().size(), 0);
            QVERIFY(backendFGChild->parentId().isNull());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
    }

    void checCleanupMarksDirty()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::FrameGraphNode *backendFGNode = new MyFrameGraphNode();

        backendFGNode->setRenderer(&renderer);

        // THEN
        QVERIFY(renderer.dirtyBits() == 0);

        // WHEN
        backendFGNode->cleanup();

        // THEN
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
    }

};

QTEST_MAIN(tst_FrameGraphNode)

#include "tst_framegraphnode.moc"
