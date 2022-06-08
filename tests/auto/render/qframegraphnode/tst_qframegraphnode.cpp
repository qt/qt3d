// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/private/qframegraphnode_p.h>

#include "testarbiter.h"

class MyFrameGraphNode : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
public:
    explicit MyFrameGraphNode(Qt3DCore::QNode *parent = nullptr)
        : QFrameGraphNode(parent)
    {
    }
};

class tst_QFrameGraphNode: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QFrameGraphNode> defaultFrameGraphNode(new MyFrameGraphNode);

        QVERIFY(defaultFrameGraphNode->isEnabled());
        QVERIFY(defaultFrameGraphNode->parentFrameGraphNode() == nullptr);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QFrameGraphNode> frameGraphNode(new MyFrameGraphNode());
        arbiter.setArbiterOnNode(frameGraphNode.data());

        // WHEN
        frameGraphNode->setEnabled(false);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), frameGraphNode.data());

        arbiter.clear();

        // WHEN
        frameGraphNode->setEnabled(false);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        frameGraphNode->setEnabled(true);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), frameGraphNode.data());

        arbiter.clear();
    }

    void checkParentFrameNodeRetrieval()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QFrameGraphNode> root(new MyFrameGraphNode);

        Qt3DRender::QFrameGraphNode *child1 = new MyFrameGraphNode(root.data());
        Qt3DRender::QFrameGraphNode *child2 = new MyFrameGraphNode(root.data());
        Qt3DCore::QEntity *child3 = new Qt3DCore::QEntity(root.data());

        Qt3DRender::QFrameGraphNode *child11 = new MyFrameGraphNode(child1);
        Qt3DCore::QEntity *child21 = new Qt3DCore::QEntity(child2);
        Qt3DRender::QFrameGraphNode *child31 = new MyFrameGraphNode(child3);

        Qt3DRender::QFrameGraphNode *child211 = new MyFrameGraphNode(child21);

        // WHEN
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(child1->parent() == root.data());
        QVERIFY(child1->parentFrameGraphNode() == root.data());

        QVERIFY(child2->parent() == root.data());
        QVERIFY(child2->parentFrameGraphNode() == root.data());

        QVERIFY(child3->parent() == root.data());


        QVERIFY(child11->parent() == child1);
        QVERIFY(child11->parentFrameGraphNode() == child1);

        QVERIFY(child21->parent() == child2);

        QVERIFY(child31->parent() == child3);
        QVERIFY(child31->parentFrameGraphNode() == root.data());


        QVERIFY(child211->parent() == child21);
        QVERIFY(child211->parentFrameGraphNode() == child2);
    }
};

QTEST_MAIN(tst_QFrameGraphNode)

#include "tst_qframegraphnode.moc"
