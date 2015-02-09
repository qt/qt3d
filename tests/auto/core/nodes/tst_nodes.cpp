/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QtTest/QtTest>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qscene.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <Qt3DCore/private/qlockableobserverinterface_p.h>
#include <Qt3DCore/private/qnode_p.h>

class tst_Nodes : public QObject
{
    Q_OBJECT
public:
    tst_Nodes() : QObject() {}
    ~tst_Nodes() {}

private slots:
    void defaultNodeConstruction();
    void defaultComponentConstruction();
    void defaultEntityConstrution();
    void appendChildNodesToNode();
    void removingChildNodesFromNode();
    void appendingChildEntitiesToNode();
    void removingChildEntitiesFromNode();
    void appendingComponentsToEntity();
    void removingComponentsFromEntity();
    void changeCustomProperty();
    void checkDestruction();
};

class ObserverSpy : public Qt3D::QLockableObserverInterface
{
public:
    class ChangeRecord : public QPair<Qt3D::QSceneChangePtr, bool>
    {
    public:
        ChangeRecord(const Qt3D::QSceneChangePtr &event, bool locked)
            : QPair<Qt3D::QSceneChangePtr, bool>(event, locked) {}
        Qt3D::QSceneChangePtr change() const { return first; }
        bool wasLocked() const { return second; }
    };

    ObserverSpy(Qt3D::QNode *node)
        : Qt3D::QLockableObserverInterface()
    {
        Qt3D::QNodePrivate::get(node)->setArbiter(this);
    }

    void sceneChangeEventWithLock(const Qt3D::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        events << ChangeRecord(e, true);
    }

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        events << ChangeRecord(e, false);
    }

    QList<ChangeRecord> events;
};

class MyQNode : public Qt3D::QNode
{
    Q_OBJECT
    Q_PROPERTY(QString customProperty READ customProperty WRITE setCustomProperty NOTIFY customPropertyChanged)
public:
    explicit MyQNode(Qt3D::QNode *parent = 0) : QNode(parent)
    {}

    void setCustomProperty(const QString &s)
    {
        if (m_customProperty == s)
            return;

        m_customProperty = s;
        emit customPropertyChanged();
    }

    QString customProperty() const
    {
        return m_customProperty;
    }

signals:
    void customPropertyChanged();

protected:
    QT3D_CLONEABLE(MyQNode)

    QString m_customProperty;

};

class MyQComponent : public Qt3D::QComponent
{
    Q_OBJECT
public:
    explicit MyQComponent(Qt3D::QNode *parent = 0) : QComponent(parent)
    {}

    // QNode interface
protected:
    QT3D_CLONEABLE(MyQComponent)
};


void tst_Nodes::defaultNodeConstruction()
{
    MyQNode *node = new MyQNode();

    QVERIFY(node != Q_NULLPTR);
    QVERIFY(node->children().isEmpty());
    MyQNode *node2 = new MyQNode(node);
    QVERIFY(node2->parent() == node);
    QVERIFY(!node->children().isEmpty());
    QVERIFY(node2->children().isEmpty());

    delete node;
}

void tst_Nodes::defaultComponentConstruction()
{
    MyQComponent *comp = new MyQComponent();
    MyQComponent *comp2 = new MyQComponent(comp);

    QVERIFY(comp != Q_NULLPTR);
    QCOMPARE(comp2->parent(), comp);

    delete comp;
}

void tst_Nodes::defaultEntityConstrution()
{
    Qt3D::QEntity *entity = new Qt3D::QEntity();
    Qt3D::QEntity *entity2 = new Qt3D::QEntity(entity);

    QVERIFY(entity->components().isEmpty());
    QVERIFY(entity2->components().isEmpty());
    QCOMPARE(entity2->parent(), entity);

    delete entity;
}

void tst_Nodes::appendChildNodesToNode()
{
    MyQNode *node = new MyQNode();
    ObserverSpy spy(node);

    for (int i = 0; i < 10; i++) {
        MyQNode *child = new MyQNode();
        QVERIFY(child->parent() == Q_NULLPTR);
        child->setParent(node);
        QVERIFY(child->parent() == node);
        QVERIFY(child->parentNode() == node);
        QCOMPARE(spy.events.size(), 1);
        QVERIFY(spy.events.first().wasLocked());
        Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(event->type(), Qt3D::NodeCreated);
        QCOMPARE(event->propertyName(), "node");
        Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
        QCOMPARE(clone->id(), child->id());
        QCOMPARE(clone->parentNode()->id(), node->id());
    }
    QVERIFY(node->children().count() == 10);
    for (int i = 0; i < 10; i++) {
        MyQNode *child = new MyQNode();
        child->setParent(node);
        QVERIFY(child->parent() == node);
        QVERIFY(child->parentNode() == node);
        QCOMPARE(spy.events.size(), 1);
        QVERIFY(spy.events.first().wasLocked());
        Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(event->type(), Qt3D::NodeCreated);
        QCOMPARE(event->propertyName(), "node");
        Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
        QCOMPARE(clone->id(), child->id());
        QCOMPARE(clone->parentNode()->id(), node->id());
    }
    QVERIFY(node->children().count() == 20);
    Qt3D::QNode *child = qobject_cast<Qt3D::QNode *>(node->children().first());
    QScopedPointer<ObserverSpy> childSpy(new ObserverSpy(child));
    Qt3D::QNode *parent = node;
    for (int i = 0; i < 10; i++) {
        QVERIFY(child->parent() == parent);
        QVERIFY(child->parentNode() == parent);
        (void) new MyQNode(child);
        parent = child;
        child = qobject_cast<Qt3D::QNode *>(child->children().first());

        QVERIFY(childSpy->events.isEmpty());
        childSpy.reset(new ObserverSpy(child));
    }
    QVERIFY(node->children().count() == 20);
    child = qobject_cast<Qt3D::QNode *>(node->children().first());
    parent = node;
    for (int i = 0; i < 10; i++) {
        QVERIFY(child->parent() == parent);
        QVERIFY(child->parentNode() == parent);
        QVERIFY(child->children().count() == 1);
        parent = child;
        child = qobject_cast<Qt3D::QNode *>(child->children().first());
    }
    QVERIFY(child->children().count() == 0);
}

void tst_Nodes::removingChildNodesFromNode()
{
    MyQNode *root = new MyQNode();
    Qt3D::QNode *child = new MyQNode();

    child->setParent(root);
    QVERIFY(root->children().count() == 1);
    child->setParent(Q_NULLPTR);
    QVERIFY(root->children().count() == 0);
    QVERIFY(child->parent() == Q_NULLPTR);

    for (int i = 0; i < 10; i++) {
        (void) new MyQNode(root);
    }
    QVERIFY(root->children().count() == 10);
    Q_FOREACH (QObject *c, root->children())
        c->setParent(Q_NULLPTR);
    QVERIFY(root->children().count() == 0);

    Qt3D::QNode *firstChild = child;
    for (int i = 0; i < 10; i++) {
        (void) new MyQNode(child);
        child = qobject_cast<Qt3D::QNode *>(child->children().first());
    }
    QVERIFY(root->children().count() == 0);
    firstChild->setParent(root);
    QVERIFY(root->children().count() == 1);

    Qt3D::QNode *parent = child->parentNode();
    for (int i = 0; i < 10; i++) {
        QVERIFY(parent->children().count() == 1);
        QVERIFY(child->parentNode() == parent);

        ObserverSpy spy(parent);
        child->setParent(Q_NULLPTR);

        QVERIFY(child->parent() == Q_NULLPTR);
        QVERIFY(parent->children().count() == 0);

        QCOMPARE(spy.events.size(), 1);
        QVERIFY(spy.events.first().wasLocked());
        Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(event->type(), Qt3D::NodeAboutToBeDeleted);
        QCOMPARE(event->propertyName(), "node");
        Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
        QCOMPARE(clone->id(), child->id());
        QVERIFY(!clone->parentNode());

        child = parent;
        parent = parent->parentNode();
    }
    QVERIFY(parent == root);
    QVERIFY(parent->children().first() == firstChild);
    QVERIFY(firstChild->children().isEmpty());
}

void tst_Nodes::appendingChildEntitiesToNode()
{
    MyQNode *root = new MyQNode();

    Qt3D::QEntity *childEntity = new Qt3D::QEntity(root);

    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == Q_NULLPTR);
    QVERIFY(childEntity->parentNode() == root);
}

void tst_Nodes::removingChildEntitiesFromNode()
{
    MyQNode *root = new MyQNode();

    Qt3D::QEntity *childEntity = new Qt3D::QEntity(root);

    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == Q_NULLPTR);
    QVERIFY(childEntity->parentNode() == root);

    childEntity->setParent(Q_NULLPTR);

    QVERIFY(root->children().isEmpty());
    QVERIFY(childEntity->parentNode() == Q_NULLPTR);
    QVERIFY(childEntity->parent() == Q_NULLPTR);
}

void tst_Nodes::appendingComponentsToEntity()
{
    MyQNode *root = new MyQNode();

    Qt3D::QEntity *entity = new Qt3D::QEntity(root);

    MyQComponent *comp1 = new MyQComponent(root);

    MyQComponent *comp2 = new MyQComponent(entity);
    MyQComponent *comp3 = new MyQComponent();

    QVERIFY(entity->parentNode() == root);
    QVERIFY(root->children().count() == 2);
    QVERIFY(entity->children().count() == 1);
    QVERIFY(entity->components().empty());
    QVERIFY(comp3->parentNode() == Q_NULLPTR);

    ObserverSpy spy(entity);

    entity->addComponent(comp1);
    QVERIFY(entity->components().count() == 1);
    QVERIFY(entity->components().first() == comp1);
    QVERIFY(comp1->parentNode() == root);
    QVERIFY(comp2->parentNode() == entity);
    QVERIFY(comp3->parentNode() == Q_NULLPTR);
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentAdded);
    QCOMPARE(event->propertyName(), "component");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), comp1->id());
    QVERIFY(!clone->parentNode());

    entity->addComponent(comp2);
    QVERIFY(entity->components().count() == 2);
    QVERIFY(entity->components().first() == comp1);
    QVERIFY(entity->components().last() == comp2);
    QVERIFY(comp1->parentNode() == root);
    QVERIFY(comp2->parentNode() == entity);
    QVERIFY(comp3->parentNode() == Q_NULLPTR);
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentAdded);
    QCOMPARE(event->propertyName(), "component");
    clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), comp2->id());
    QVERIFY(!clone->parentNode());

    entity->addComponent(comp3);
    QVERIFY(entity->components().count() == 3);
    QVERIFY(entity->components().first() == comp1);
    QVERIFY(entity->components().last() == comp3);
    QVERIFY(comp1->parentNode() == root);
    QVERIFY(comp2->parentNode() == entity);
    QVERIFY(comp3->parentNode() == entity);
    QCOMPARE(entity->children().count(), 2);

    QCOMPARE(spy.events.size(), 2);

    QVERIFY(spy.events.first().wasLocked());
    event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeCreated);
    QCOMPARE(event->propertyName(), "node");
    clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), comp3->id());
    QCOMPARE(clone->parentNode()->id(), entity->id());

    QVERIFY(spy.events.first().wasLocked());
    event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentAdded);
    QCOMPARE(event->propertyName(), "component");
    clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), comp3->id());
    QVERIFY(!clone->parentNode());
}

void tst_Nodes::removingComponentsFromEntity()
{
    MyQNode *root = new MyQNode();
    Qt3D::QEntity *entity = new Qt3D::QEntity(root);

    MyQComponent *comp1 = new MyQComponent(root);
    MyQComponent *comp2 = new MyQComponent(entity);
    MyQComponent *comp3 = new MyQComponent();


    entity->addComponent(comp1);
    entity->addComponent(comp2);
    entity->addComponent(comp3);
    QVERIFY(entity->components().count() == 3);
    QCOMPARE(entity->children().count(), 2);

    ObserverSpy spy(entity);

    entity->removeComponent(comp2);
    QVERIFY(entity->components().count() == 2);
    QVERIFY(comp2->parent() == entity);
    QVERIFY(entity->children().count() == 2);
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentRemoved);
    QCOMPARE(event->propertyName(), "componentId");
    Qt3D::QNodeId nodeId = event->value().value<Qt3D::QNodeId>();
    QCOMPARE(nodeId, comp2->id());

    entity->removeComponent(comp1);
    QVERIFY(entity->components().count() == 1);
    QVERIFY(comp1->parentNode() == root);
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentRemoved);
    QCOMPARE(event->propertyName(), "componentId");
    nodeId = event->value().value<Qt3D::QNodeId>();
    QCOMPARE(nodeId, comp1->id());

    entity->removeComponent(comp3);
    QVERIFY(entity->components().count() == 0);
    QVERIFY(comp3->parentNode() == entity);
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentRemoved);
    QCOMPARE(event->propertyName(), "componentId");
    nodeId = event->value().value<Qt3D::QNodeId>();
    QCOMPARE(nodeId, comp3->id());

    entity->addComponent(comp1);
    entity->addComponent(comp2);
    entity->addComponent(comp3);
    QVERIFY(entity->components().count() == 3);
    QCOMPARE(entity->children().count(), 2);

    Q_FOREACH (QObject *c, entity->children())
        c->setParent(Q_NULLPTR);
    QCOMPARE(entity->components().count(), 3);
    QCOMPARE(entity->children().count(), 0);
}

void tst_Nodes::changeCustomProperty()
{
    MyQNode *node = new MyQNode();
    ObserverSpy spy(node);
    node->setCustomProperty(QStringLiteral("foo"));
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeUpdated);
    QCOMPARE(event->propertyName(), "customProperty");
    QCOMPARE(event->value().toString(), QString("foo"));
}

void tst_Nodes::checkDestruction()
{
    MyQNode *root = new MyQNode();
    Qt3D::QEntity *entity = new Qt3D::QEntity(root);

    MyQComponent *comp1 = new MyQComponent();
    MyQComponent *comp2 = new MyQComponent();
    MyQComponent *comp3 = new MyQComponent();


    entity->addComponent(comp1);
    entity->addComponent(comp2);
    entity->addComponent(comp3);

    delete root;
}

QTEST_GUILESS_MAIN(tst_Nodes)

#include "tst_nodes.moc"
