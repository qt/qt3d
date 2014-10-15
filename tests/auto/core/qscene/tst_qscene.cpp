/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DCore/qscene.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qobservableinterface.h>
#include <Qt3DCore/qobserverinterface.h>
#include <Qt3DCore/qchangearbiter.h>
#include <private/qnode_p.h>

class tst_QScene : public QObject
{
    Q_OBJECT
public:
    tst_QScene() : QObject() {}
    ~tst_QScene() {}

private slots:
    void addObservable();
    void addNodeObservable();
    void removeObservable();
    void removeNodeObservable();
    void addChildNode();
    void removeChildNode();
    void addEntityForComponent();
    void removeEntityForComponent();
};

class tst_Observable : public Qt3D::QObservableInterface
{
public:
    void registerObserver(Qt3D::QObserverInterface *observer)
    {
        m_arbiter = dynamic_cast<Qt3D::QChangeArbiter *>(observer);
        QVERIFY(m_arbiter != Q_NULLPTR);
    }

    void unregisterObserver(Qt3D::QObserverInterface *observer)
    {
        QVERIFY(m_arbiter == observer);
    }

protected:
    void notifyObservers(const Qt3D::QSceneChangePtr &) {}

private:
    Qt3D::QChangeArbiter *m_arbiter;
};

class tst_Node : public Qt3D::QNode
{
public:
    tst_Node() : Qt3D::QNode()
    {}
protected:
    QT3D_CLONEABLE(tst_Node)
};

class tst_Component : public Qt3D::QComponent
{
public:
    tst_Component() : Qt3D::QComponent()
    {}
protected:
    QT3D_CLONEABLE(tst_Component)
};

void tst_QScene::addObservable()
{
    Qt3D::QNode *node1 = new tst_Node();
    Qt3D::QNode *node2 = new tst_Node();

    QList<tst_Observable *> observables;

    for (int i = 0; i < 10; i++)
        observables.append(new tst_Observable());

    Qt3D::QScene *scene = new Qt3D::QScene;
    scene->setArbiter(new Qt3D::QChangeArbiter());

    for (int i = 0; i < 5; i++)
        scene->addObservable(observables.at(i), node1->uuid());

    for (int i = 0; i < 5; i++)
        scene->addObservable(observables.at(i + 5), node2->uuid());

    Qt3D::QObservableList obs1 = scene->lookupObservables(node1->uuid());
    Qt3D::QObservableList obs2 = scene->lookupObservables(node2->uuid());

    QCOMPARE(obs1.count(), 5);
    QCOMPARE(obs2.count(), obs1.count());

    Q_FOREACH (Qt3D::QObservableInterface *o, obs1) {
        QVERIFY(scene->nodeIdFromObservable(o) == node1->uuid());
        QVERIFY(scene->lookupNode(node1->uuid()) == Q_NULLPTR);
    }
    Q_FOREACH (Qt3D::QObservableInterface *o, obs2) {
        QVERIFY(scene->nodeIdFromObservable(o) == node2->uuid());
        QVERIFY(scene->lookupNode(node2->uuid()) == Q_NULLPTR);
    }
}

void tst_QScene::addNodeObservable()
{
    QList<Qt3D::QNode *> nodes;

    for (int i = 0; i < 10; i++)
        nodes.append(new tst_Node());

    Qt3D::QScene *scene = new Qt3D::QScene;
    scene->setArbiter(new Qt3D::QChangeArbiter());

    for (int i = 0; i < 10; i++)
        scene->addObservable(nodes.at(i));

    Q_FOREACH (Qt3D::QNode *n, nodes) {
        QVERIFY(n == scene->lookupNode(n->uuid()));
        QVERIFY(scene->lookupObservables(n->uuid()).isEmpty());
    }
}

void tst_QScene::removeObservable()
{
    Qt3D::QNode *node1 = new tst_Node();
    Qt3D::QNode *node2 = new tst_Node();

    QList<tst_Observable *> observables;

    for (int i = 0; i < 10; i++)
        observables.append(new tst_Observable());

    Qt3D::QScene *scene = new Qt3D::QScene;
    scene->setArbiter(new Qt3D::QChangeArbiter());

    for (int i = 0; i < 5; i++)
        scene->addObservable(observables.at(i), node1->uuid());

    for (int i = 0; i < 5; i++)
        scene->addObservable(observables.at(i + 5), node2->uuid());

    Qt3D::QObservableList obs1 = scene->lookupObservables(node1->uuid());
    Qt3D::QObservableList obs2 = scene->lookupObservables(node2->uuid());

    QCOMPARE(obs1.count(), 5);
    QCOMPARE(obs2.count(), obs1.count());

    scene->removeObservable(observables.at(0), node1->uuid());
    QCOMPARE(scene->lookupObservables(node1->uuid()).count(), 4);

    scene->removeObservable(observables.at(0), node1->uuid());
    QCOMPARE(scene->lookupObservables(node1->uuid()).count(), 4);

    scene->removeObservable(observables.at(6), node1->uuid());
    QCOMPARE(scene->lookupObservables(node1->uuid()).count(), 4);
    QCOMPARE(scene->lookupObservables(node2->uuid()).count(), 5);

    scene->removeObservable(observables.at(0), node2->uuid());
    QCOMPARE(scene->lookupObservables(node2->uuid()).count(), 5);

    QVERIFY(scene->nodeIdFromObservable(observables.at(0)) == QUuid());
}

void tst_QScene::removeNodeObservable()
{
    Qt3D::QNode *node1 = new tst_Node();
    Qt3D::QNode *node2 = new tst_Node();

    QList<tst_Observable *> observables;

    for (int i = 0; i < 10; i++)
        observables.append(new tst_Observable());

    Qt3D::QScene *scene = new Qt3D::QScene;
    scene->setArbiter(new Qt3D::QChangeArbiter());

    scene->addObservable(node1);
    scene->addObservable(node2);

    for (int i = 0; i < 5; i++)
        scene->addObservable(observables.at(i), node1->uuid());

    for (int i = 0; i < 5; i++)
        scene->addObservable(observables.at(i + 5), node2->uuid());

    Qt3D::QObservableList obs1 = scene->lookupObservables(node1->uuid());
    Qt3D::QObservableList obs2 = scene->lookupObservables(node2->uuid());

    QCOMPARE(obs1.count(), 5);
    QCOMPARE(obs2.count(), obs1.count());

    scene->removeObservable(node1);

    QVERIFY(scene->lookupNode(node1->uuid()) == Q_NULLPTR);
    QVERIFY(scene->lookupObservables(node1->uuid()).empty());
    QVERIFY(scene->nodeIdFromObservable(observables.at(0)) == QUuid());

    QVERIFY(scene->lookupNode(node2->uuid()) == node2);
    QCOMPARE(scene->lookupObservables(node2->uuid()).count(), 5);
    QVERIFY(scene->nodeIdFromObservable(observables.at(9)) == node2->uuid());
}

void tst_QScene::addChildNode()
{
    Qt3D::QScene *scene = new Qt3D::QScene;

    QList<Qt3D::QNode *> nodes;

    Qt3D::QNode *root = new tst_Node();
    Qt3D::QNodePrivate::get(root)->setScene(scene);
    scene->addObservable(root);
    for (int i = 0; i < 10; i++) {
        Qt3D::QNode *child = new tst_Node();
        if (nodes.isEmpty())
            child->setParent(root);
        else
            child->setParent(nodes.last());
        nodes.append(child);
    }

    QVERIFY(scene->lookupNode(root->uuid()) == root);
    QCoreApplication::processEvents();

    Q_FOREACH (Qt3D::QNode *n, nodes) {
        QVERIFY(scene->lookupNode(n->uuid()) == n);
    }
}

void tst_QScene::removeChildNode()
{
    Qt3D::QScene *scene = new Qt3D::QScene;

    QList<Qt3D::QNode *> nodes;

    Qt3D::QNode *root = new tst_Node;
    Qt3D::QNodePrivate::get(root)->setScene(scene);
    scene->addObservable(root);
    for (int i = 0; i < 10; i++) {
        Qt3D::QNode *child = new tst_Node;
        if (nodes.isEmpty())
            child->setParent(root);
        else
            child->setParent(nodes.last());
        nodes.append(child);
    }

    while (!nodes.isEmpty()) {
        Qt3D::QNode *lst = nodes.takeLast();
        QVERIFY(scene->lookupNode(lst->uuid()) == lst);
        if (lst->parentNode() != Q_NULLPTR) {
            lst->setParent(Q_NULLPTR);
            QCoreApplication::processEvents();
            QVERIFY(scene->lookupNode(lst->uuid()) == Q_NULLPTR);
        }
    }
}

void tst_QScene::addEntityForComponent()
{
    Qt3D::QScene *scene = new Qt3D::QScene;

    QList<Qt3D::QEntity *> entities;
    QList<Qt3D::QComponent *> components;

    for (int i = 0; i < 10; i++) {
        Qt3D::QEntity *entity = new Qt3D::QEntity();
        Qt3D::QComponent *comp = new tst_Component();

        Qt3D::QNodePrivate::get(entity)->setScene(scene);
        Qt3D::QNodePrivate::get(comp)->setScene(scene);
        entities << entity;
        components << comp;
    }

    for (int i = 0; i < 10; i++) {
        Qt3D::QEntity *e = entities.at(i);
        for (int j = 0; j < 10; j++) {
            e->addComponent(components.at(j));
        }
    }

    for (int i = 0; i < 10; i++) {
        QList<QUuid> uuids = scene->entitiesForComponent(components.at(i)->uuid());
        QCOMPARE(uuids.count(), 10);
    }
}

void tst_QScene::removeEntityForComponent()
{
    Qt3D::QScene *scene = new Qt3D::QScene;

    QList<Qt3D::QEntity *> entities;
    QList<Qt3D::QComponent *> components;

    for (int i = 0; i < 10; i++) {
        Qt3D::QEntity *entity = new Qt3D::QEntity();
        Qt3D::QComponent *comp = new tst_Component();

        Qt3D::QNodePrivate::get(entity)->setScene(scene);
        Qt3D::QNodePrivate::get(comp)->setScene(scene);
        entities << entity;
        components << comp;
    }

    for (int i = 0; i < 10; i++) {
        Qt3D::QEntity *e = entities.at(i);
        for (int j = 0; j < 10; j++) {
            e->addComponent(components.at(j));
        }
    }

    for (int i = 0; i < 10; i++) {
        Qt3D::QEntity *e = entities.at(i);
        for (int j = 0; j < 10; j++) {
            e->removeComponent(components.at(j));
            QCOMPARE(scene->entitiesForComponent(components.at(j)->uuid()).count(), 10 - (i + 1));
        }
    }
}

QTEST_MAIN(tst_QScene)

#include "tst_qscene.moc"
