/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/qframegraph.h>

#include "testpostmanarbiter.h"

class MyFrameGraphNode : public Qt3DRender::QFrameGraphNode
{
    Q_OBJECT
public:
    explicit MyFrameGraphNode(Qt3DCore::QNode *parent = Q_NULLPTR)
        : QFrameGraphNode(parent)
    {
    }

    ~MyFrameGraphNode()
    {
        QNode::cleanup();
    }

private:
    QT3D_CLONEABLE(MyFrameGraphNode)
};

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QFrameGraph: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QFrameGraph()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QFrameGraph> defaultFrameGraph(new Qt3DRender::QFrameGraph);

        QVERIFY(defaultFrameGraph->activeFrameGraph() == Q_NULLPTR);
    }

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QFrameGraph *>("frameGraph");
        QTest::addColumn<Qt3DRender::QFrameGraphNode *>("frameGraphTree");

        Qt3DRender::QFrameGraph *defaultConstructed = new Qt3DRender::QFrameGraph();
        QTest::newRow("defaultConstructed") << defaultConstructed << static_cast<Qt3DRender::QFrameGraphNode *>(Q_NULLPTR);

        Qt3DRender::QFrameGraph *frameGraphWithActiveNode = new Qt3DRender::QFrameGraph();
        Qt3DRender::QFrameGraphNode *frameGraphTree = new MyFrameGraphNode();
        frameGraphWithActiveNode->setActiveFrameGraph(frameGraphTree);
        QTest::newRow("frameGraphWithActiveTree") << frameGraphWithActiveNode << frameGraphTree;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QFrameGraph*, frameGraph);
        QFETCH(Qt3DRender::QFrameGraphNode *, frameGraphTree);

        // THEN
        QVERIFY(frameGraph->activeFrameGraph() == frameGraphTree);

        // WHEN
        Qt3DRender::QFrameGraph *clone = static_cast<Qt3DRender::QFrameGraph *>(QNode::clone(frameGraph));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(frameGraph->id(), clone->id());

        if (frameGraph->activeFrameGraph() != Q_NULLPTR) {
            QVERIFY(frameGraph->activeFrameGraph()->parent() == frameGraph);
            QVERIFY(clone->activeFrameGraph() != Q_NULLPTR);
            QCOMPARE(clone->activeFrameGraph()->id(), frameGraph->activeFrameGraph()->id());
            QVERIFY(clone->activeFrameGraph()->parent() == clone);
        }

        delete frameGraph;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QFrameGraph> frameGraph(new Qt3DRender::QFrameGraph());
        TestArbiter arbiter(frameGraph.data());

        // WHEN
        MyFrameGraphNode *activeFrameGraph1 = new MyFrameGraphNode();
        frameGraph->setActiveFrameGraph(activeFrameGraph1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "activeFrameGraph");
        QCOMPARE(change->subjectId(), frameGraph->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), activeFrameGraph1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        frameGraph->setActiveFrameGraph(activeFrameGraph1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        MyFrameGraphNode *activeFrameGraph2 = new MyFrameGraphNode();
        frameGraph->setActiveFrameGraph(activeFrameGraph2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "activeFrameGraph");
        QCOMPARE(change->subjectId(), frameGraph->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), activeFrameGraph2->id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        frameGraph->setActiveFrameGraph(Q_NULLPTR);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "activeFrameGraph");
        QCOMPARE(change->subjectId(), frameGraph->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), Qt3DCore::QNodeId());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QFrameGraph)

#include "tst_qframegraph.moc"
