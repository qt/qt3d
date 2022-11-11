// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <private/qnode_p.h>
#include <testarbiter.h>

class tst_QScene : public QObject
{
    Q_OBJECT
public:
    tst_QScene() : QObject() {}
    ~tst_QScene() {}

private slots:
    void addNodeObservable();
    void removeNodeObservable();
    void addChildNode();
    void deleteChildNode();
    void removeChildNode();
    void addEntityForComponent();
    void removeEntityForComponent();
    void hasEntityForComponent();
};

class tst_Node : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_Node() : Qt3DCore::QNode()
    {}
};

class tst_Component : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    tst_Component() : Qt3DCore::QComponent()
    {}
};

void tst_QScene::addNodeObservable()
{
    // GIBEN
    QList<Qt3DCore::QNode *> nodes;

    for (int i = 0; i < 10; i++)
        nodes.append(new tst_Node());

    Qt3DCore::QScene *scene = new Qt3DCore::QScene;
    scene->setArbiter(new TestArbiter);

    // WHEN
    for (int i = 0; i < 10; i++)
        scene->addObservable(nodes.at(i));

    // THEN
    for (Qt3DCore::QNode *n : std::as_const(nodes)) {
        QVERIFY(n == scene->lookupNode(n->id()));
    }
}

void tst_QScene::removeNodeObservable()
{
    // GIVEN
    Qt3DCore::QNode *node1 = new tst_Node();
    Qt3DCore::QNode *node2 = new tst_Node();

    Qt3DCore::QScene *scene = new Qt3DCore::QScene;
    scene->setArbiter(new TestArbiter);

    // WHEN
    scene->addObservable(node1);
    scene->addObservable(node2);

    // WHEN
    scene->removeObservable(node1);

    // THEN
    QVERIFY(scene->lookupNode(node1->id()) == nullptr);
    QVERIFY(scene->lookupNode(node2->id()) == node2);
}

void tst_QScene::addChildNode()
{
    // GIVEN
    Qt3DCore::QScene *scene = new Qt3DCore::QScene;

    QList<Qt3DCore::QNode *> nodes;

    Qt3DCore::QNode *root = new tst_Node();
    Qt3DCore::QNodePrivate::get(root)->setScene(scene);

    // WHEN
    scene->addObservable(root);
    // THEN
    QVERIFY(scene->lookupNode(root->id()) == root);

    // WHEN
    for (int i = 0; i < 10; i++) {
        Qt3DCore::QNode *child = new tst_Node();
        if (nodes.isEmpty())
            child->setParent(root);
        else
            child->setParent(nodes.last());
        nodes.append(child);
    }
    QCoreApplication::processEvents();

    // THEN
    for (Qt3DCore::QNode *n : std::as_const(nodes)) {
        QVERIFY(scene->lookupNode(n->id()) == n);
    }
}

void tst_QScene::deleteChildNode()
{
    // GIVEN
    Qt3DCore::QScene *scene = new Qt3DCore::QScene;

    QList<Qt3DCore::QNode *> nodes1, nodes2;

    Qt3DCore::QNode *root1 = new tst_Node();
    Qt3DCore::QNode *root2 = new tst_Node();
    Qt3DCore::QNodePrivate::get(root1)->setScene(scene);
    Qt3DCore::QNodePrivate::get(root2)->setScene(scene);
    Qt3DCore::QNodePrivate::get(root1)->m_hasBackendNode = true;
    Qt3DCore::QNodePrivate::get(root2)->m_hasBackendNode = true;

    // WHEN
    scene->addObservable(root1);
    scene->addObservable(root2);
    // THEN
    QVERIFY(scene->lookupNode(root1->id()) == root1);
    QVERIFY(scene->lookupNode(root2->id()) == root2);

    // WHEN
    for (int i = 0; i < 10; i++) {
        Qt3DCore::QNode *child1 = new tst_Node();
        child1->setParent(nodes1.isEmpty() ? root1 : nodes1.last());
        Qt3DCore::QNodePrivate::get(child1)->m_hasBackendNode = true;
        nodes1.append(child1);

        Qt3DCore::QNode *child2 = new tst_Node();
        child2->setParent(nodes2.isEmpty() ? root2 : nodes2.last());
        Qt3DCore::QNodePrivate::get(child2)->m_hasBackendNode = true;
        nodes2.append(child2);
    }
    QCoreApplication::processEvents();

    // THEN
    for (Qt3DCore::QNode *n : std::as_const(nodes1)) {
        QVERIFY(scene->lookupNode(n->id()) == n);
    }
    for (Qt3DCore::QNode *n : std::as_const(nodes2)) {
        QVERIFY(scene->lookupNode(n->id()) == n);
    }

    // gather node IDs
    Qt3DCore::QNodeIdVector root1ChildIds;
    for (Qt3DCore::QNode *n : std::as_const(nodes1))
        root1ChildIds << n->id();

    // WHEN
    delete root1;
    QCoreApplication::processEvents();

    // THEN
    for (Qt3DCore::QNodeId id : std::as_const(root1ChildIds)) {
        QVERIFY(scene->lookupNode(id) == nullptr);
    }

    // WHEN
    nodes2.first()->setParent(static_cast<Qt3DCore::QNode*>(nullptr));
    QCoreApplication::processEvents();

    // THEN
    for (Qt3DCore::QNode *n : std::as_const(nodes2)) {
        QVERIFY(scene->lookupNode(n->id()) == nullptr);
    }
}

void tst_QScene::removeChildNode()
{
    // GIVEN
    Qt3DCore::QScene *scene = new Qt3DCore::QScene;

    QList<Qt3DCore::QNode *> nodes;

    Qt3DCore::QNode *root = new tst_Node;
    Qt3DCore::QNodePrivate::get(root)->setScene(scene);
    Qt3DCore::QNodePrivate::get(root)->m_hasBackendNode = true;
    scene->addObservable(root);

    // WHEN
    for (int i = 0; i < 10; i++) {
        Qt3DCore::QNode *child = new tst_Node;
        if (nodes.isEmpty())
            child->setParent(root);
        else
            child->setParent(nodes.last());
        Qt3DCore::QNodePrivate::get(child)->m_hasBackendNode = true;
        nodes.append(child);
    }

    // THEN
    while (!nodes.isEmpty()) {
        Qt3DCore::QNode *lst = nodes.takeLast();
        QVERIFY(scene->lookupNode(lst->id()) == lst);
        if (lst->parentNode() != nullptr) {
            lst->setParent(Q_NODE_NULLPTR);
            QCoreApplication::processEvents();
            QVERIFY(scene->lookupNode(lst->id()) == nullptr);
        }
    }
}

void tst_QScene::addEntityForComponent()
{
    // GIVEN
    Qt3DCore::QScene *scene = new Qt3DCore::QScene;

    QList<Qt3DCore::QEntity *> entities;
    QList<Qt3DCore::QComponent *> components;

    for (int i = 0; i < 10; i++) {
        Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
        Qt3DCore::QComponent *comp = new tst_Component();

        Qt3DCore::QNodePrivate::get(entity)->setScene(scene);
        Qt3DCore::QNodePrivate::get(comp)->setScene(scene);
        entities << entity;
        components << comp;
    }

    // WHEN
    for (int i = 0; i < 10; i++) {
        Qt3DCore::QEntity *e = entities.at(i);
        for (int j = 0; j < 10; j++) {
            e->addComponent(components.at(j));
        }
    }

    // THEN
    for (int i = 0; i < 10; i++) {
        const QList<Qt3DCore::QNodeId> ids = scene->entitiesForComponent(components.at(i)->id());
        QCOMPARE(ids.size(), 10);
    }
}

void tst_QScene::removeEntityForComponent()
{
    // GIVEN
    Qt3DCore::QScene *scene = new Qt3DCore::QScene;

    QList<Qt3DCore::QEntity *> entities;
    QList<Qt3DCore::QComponent *> components;

    for (int i = 0; i < 10; i++) {
        Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
        Qt3DCore::QComponent *comp = new tst_Component();

        Qt3DCore::QNodePrivate::get(entity)->setScene(scene);
        Qt3DCore::QNodePrivate::get(comp)->setScene(scene);
        entities << entity;
        components << comp;
    }

    // WHEN
    for (int i = 0; i < 10; i++) {
        Qt3DCore::QEntity *e = entities.at(i);
        for (int j = 0; j < 10; j++) {
            e->addComponent(components.at(j));
        }
    }

    // THEN
    for (int i = 0; i < 10; i++) {
        Qt3DCore::QEntity *e = entities.at(i);
        for (int j = 0; j < 10; j++) {
            e->removeComponent(components.at(j));
            QCOMPARE(scene->entitiesForComponent(components.at(j)->id()).size(), 10 - (i + 1));
        }
    }
}

void tst_QScene::hasEntityForComponent()
{
    // GIVEN
    Qt3DCore::QScene *scene = new Qt3DCore::QScene;

    QList<Qt3DCore::QEntity *> entities;
    QList<Qt3DCore::QComponent *> components;

    for (int i = 0; i < 10; i++) {
        Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
        Qt3DCore::QComponent *comp = new tst_Component();

        Qt3DCore::QNodePrivate::get(entity)->setScene(scene);
        Qt3DCore::QNodePrivate::get(comp)->setScene(scene);
        entities << entity;
        components << comp;
    }

    // WHEN
    for (int i = 0; i < 10; i++) {
        Qt3DCore::QEntity *e = entities.at(i);
        for (int j = 0; j < 10; j++) {
            e->addComponent(components.at(j));
        }
    }

    // THEN
    for (int i = 0; i < 10; i++)
        QVERIFY(scene->hasEntityForComponent(components.at(i)->id(), entities.at(i)->id()));
}

QTEST_MAIN(tst_QScene)

#include "tst_qscene.moc"
