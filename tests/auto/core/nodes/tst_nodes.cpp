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

#include <QtTest/QTest>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <private/qpostman_p.h>

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

    void appendSingleChildNodeToNodeNoSceneExplicitParenting();
    void appendSingleChildNodeToNodeNoSceneImplicitParenting();
    void appendMultipleChildNodesToNodeNoScene();

    void appendSingleChildNodeToNodeSceneExplicitParenting();
    void appendSingleChildNodeToNodeSceneImplicitParenting();
    void appendMultipleChildNodesToNodeScene();

    void checkParentChangeToNull();
    void checkParentChangeToOtherParent();

    void removingSingleChildNodeFromNode();
    void removingMultipleChildNodesFromNode();

    void appendingChildEntitiesToNode();
    void removingChildEntitiesFromNode();

    void appendingComponentToEntity();
    void appendingParentlessComponentToEntity();
    void removingComponentFromEntity();

    void changeCustomProperty();
    void checkDestruction();
    void verifyCopy();
};

class ObserverSpy;
class SimplePostman : public Qt3D::QAbstractPostman
{
public:
    SimplePostman(ObserverSpy *spy)
        : m_spy(spy)
    {}

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &) Q_DECL_FINAL {};
    void setScene(Qt3D::QScene *) Q_DECL_FINAL {};
    void notifyBackend(const Qt3D::QSceneChangePtr &change) Q_DECL_FINAL;

private:
    ObserverSpy *m_spy;
};

class ObserverSpy : public Qt3D::QAbstractArbiter
{
public:
    class ChangeRecord : public QPair<Qt3D::QSceneChangePtr, bool>
    {
    public:
        ChangeRecord(const Qt3D::QSceneChangePtr &event, bool locked)
            : QPair<Qt3D::QSceneChangePtr, bool>(event, locked)
        {}

        Qt3D::QSceneChangePtr change() const { return first; }

        bool wasLocked() const { return second; }
    };

    ObserverSpy(Qt3D::QNode *node)
        : Qt3D::QAbstractArbiter()
        , m_node(node)
        , m_postman(new SimplePostman(this))
    {
        Qt3D::QNodePrivate::get(node)->setArbiter(this);
    }

    ~ObserverSpy()
    {
        Qt3D::QNodePrivate::get(m_node)->setArbiter(Q_NULLPTR);
    }

    void sceneChangeEventWithLock(const Qt3D::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        events << ChangeRecord(e, true);
    }

    void sceneChangeEventWithLock(const Qt3D::QSceneChangeList &e) Q_DECL_OVERRIDE
    {
        for (uint i = 0, m = e.size(); i < m; ++i) {
            events << ChangeRecord(e.at(i), false);
        }
    }

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        events << ChangeRecord(e, false);
    }

    Qt3D::QAbstractPostman *postman() const Q_DECL_FINAL
    {
        return m_postman.data();
    }

    QList<ChangeRecord> events;
    Qt3D::QNode *m_node;
    QScopedPointer<SimplePostman> m_postman;
};

void SimplePostman::notifyBackend(const Qt3D::QSceneChangePtr &change)
{
    m_spy->sceneChangeEventWithLock(change);
}

class MyQNode : public Qt3D::QNode
{
    Q_OBJECT
    Q_PROPERTY(QString customProperty READ customProperty WRITE setCustomProperty NOTIFY customPropertyChanged)
public:
    explicit MyQNode(Qt3D::QNode *parent = 0)
        : QNode(parent)
        , m_scene(Q_NULLPTR)
    {}

    ~MyQNode()
    {
        QNode::cleanup();
        delete m_scene;
    }

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

    void assignScene()
    {
        if (!m_scene)
            m_scene = new Qt3D::QScene();
        Qt3D::QNodePrivate::get(this)->setScene(m_scene);
    }

    void makeCopyOf(QNode *other)
    {
        QNode::copy(other);
    }

signals:
    void customPropertyChanged();

protected:
    QT3D_CLONEABLE(MyQNode)

    QString m_customProperty;
    Qt3D::QScene *m_scene;
};

class MyQComponent : public Qt3D::QComponent
{
    Q_OBJECT
public:
    explicit MyQComponent(Qt3D::QNode *parent = 0) : QComponent(parent)
    {}

    ~MyQComponent()
    {
        QNode::cleanup();
    }

    // QNode interface
protected:
    QT3D_CLONEABLE(MyQComponent)
};


void tst_Nodes::defaultNodeConstruction()
{
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());

    // THEN
    QVERIFY(node != Q_NULLPTR);
    QVERIFY(node->children().isEmpty());

    // GIVEN
    MyQNode *node2 = new MyQNode(node.data());

    // THEN
    QVERIFY(node2->parent() == node.data());
    QVERIFY(!node->children().isEmpty());
    QVERIFY(node2->children().isEmpty());
}

void tst_Nodes::defaultComponentConstruction()
{
    // GIVEN
    QScopedPointer<MyQComponent> comp(new MyQComponent());
    MyQComponent *comp2 = new MyQComponent(comp.data());

    // THEN
    QVERIFY(comp != Q_NULLPTR);
    QCOMPARE(comp2->parent(), comp.data());
}

void tst_Nodes::defaultEntityConstrution()
{
    // GIVEN
    QScopedPointer<Qt3D::QEntity> entity(new Qt3D::QEntity());
    Qt3D::QEntity *entity2 = new Qt3D::QEntity(entity.data());

    // THEN
    QVERIFY(entity->components().isEmpty());
    QVERIFY(entity2->components().isEmpty());
    QCOMPARE(entity2->parent(), entity.data());
}

void tst_Nodes::appendSingleChildNodeToNodeNoSceneExplicitParenting()
{
    // Check nodes added when no scene is set
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    ObserverSpy spy(node.data());

    // THEN
    QVERIFY(Qt3D::QNodePrivate::get(node.data())->scene() == Q_NULLPTR);
    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode());

    // THEN
    QVERIFY(child->parent() == Q_NULLPTR);

    // WHEN
    child->setParent(node.data());

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QCOMPARE(node->children().count(), 1);

    // Events are only sent when a scene is set on the root node
    QCOMPARE(spy.events.size(), 0);
}

void tst_Nodes::appendSingleChildNodeToNodeNoSceneImplicitParenting()
{
    // Check nodes added when no scene is set
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    ObserverSpy spy(node.data());

    // THEN
    QVERIFY(Qt3D::QNodePrivate::get(node.data())->scene() == Q_NULLPTR);
    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode(node.data()));

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QCOMPARE(node->children().count(), 1);

    // Events are only sent when a scene is set on the root node
    QCOMPARE(spy.events.size(), 0);
}

void tst_Nodes::appendMultipleChildNodesToNodeNoScene()
{
    // Check multiple nodes added with no scene set
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    ObserverSpy spy(node.data());

    // THEN
    QVERIFY(Qt3D::QNodePrivate::get(node.data())->scene() == Q_NULLPTR);
    // WHEN
    for (int i = 0; i < 10; i++) {
        // WHEN
        Qt3D::QNode *child = Q_NULLPTR;
        if (i % 2 == 0) {
            child = new MyQNode(node.data());
        } else {
            child = new MyQNode();
            child->setParent(node.data());
        }
        // THEN
        QVERIFY(child->parent() == node.data());
    }

    // THEN
    QCOMPARE(node->children().count(), 10);

    // Events are only sent when a scene is set on the root node
    QCOMPARE(spy.events.size(), 0);
}

void tst_Nodes::appendSingleChildNodeToNodeSceneExplicitParenting()
{
    // Check nodes added when scene is set
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    ObserverSpy spy(node.data());
    // WHEN
    node->assignScene();
    // THEN
    QVERIFY(Qt3D::QNodePrivate::get(node.data())->scene() != Q_NULLPTR);

    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode());

    // THEN
    QVERIFY(child->parent() == Q_NULLPTR);

    // WHEN
    child->setParent(node.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    QCOMPARE(node->children().count(), 1);

    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeCreated);
    QCOMPARE(event->propertyName(), "node");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), child->id());
    QCOMPARE(clone->parentNode()->id(), node->id());
}

void tst_Nodes::appendSingleChildNodeToNodeSceneImplicitParenting()
{
    // Check nodes added when scene is set
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    ObserverSpy spy(node.data());
    // WHEN
    node->assignScene();
    // THEN
    QVERIFY(Qt3D::QNodePrivate::get(node.data())->scene() != Q_NULLPTR);

    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode(node.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QVERIFY(Qt3D::QNodePrivate::get(child.data())->scene() != Q_NULLPTR);

    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    QCOMPARE(node->children().count(), 1);

    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeCreated);
    QCOMPARE(event->propertyName(), "node");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), child->id());
    QCOMPARE(clone->parentNode()->id(), node->id());
}

void tst_Nodes::appendMultipleChildNodesToNodeScene()
{
    // Check nodes added when scene is set

    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    // WHEN
    node->assignScene();
    ObserverSpy spy(node.data());
    // THEN
    QVERIFY(Qt3D::QNodePrivate::get(node.data())->scene() != Q_NULLPTR);

    // WHEN
    for (int i = 0; i < 10; i++) {
        // WHEN
        Qt3D::QNode *child = Q_NULLPTR;
        if (i % 2 == 0) {
            child = new MyQNode(node.data());
            QCoreApplication::processEvents();
            QCOMPARE(spy.events.size(), i + 1);
        } else {
            child = new MyQNode();
            child->setParent(node.data());
        }
        // THEN
        QVERIFY(child->parent() == node.data());
        QVERIFY(Qt3D::QNodePrivate::get(child)->scene() != Q_NULLPTR);
    }
    // THEN
    QCOMPARE(node->children().count(), 10);

    // THEN
    QCOMPARE(spy.events.size(), 10);
    Q_FOREACH (const ObserverSpy::ChangeRecord &r, spy.events) {

        QVERIFY(r.wasLocked());
        Qt3D::QScenePropertyChangePtr event = r.change().dynamicCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(event->type(), Qt3D::NodeCreated);
        QCOMPARE(event->propertyName(), "node");
        Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();

        bool found = false;
        Q_FOREACH (QObject *c, node->children()) {
            if (clone->id() == qobject_cast<Qt3D::QNode *>(c)->id()) {
                found = true;
                QCOMPARE(clone->parentNode()->id(), node->id());
                break;
            }
        }
        QVERIFY(found);
    }
}

void tst_Nodes::checkParentChangeToNull()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    ObserverSpy spy(root.data());

    // WHEN
    root->assignScene();
    QScopedPointer<Qt3D::QNode> child(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == root.data());
    QCOMPARE(spy.events.size(), 1);
    QCOMPARE(root->children().size(), 1);

    // WHEN
    spy.events.clear();
    child->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(child->parent() == Q_NULLPTR);
    QCOMPARE(root->children().size(), 0);
    QCOMPARE(spy.events.size(), 1);

    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeAboutToBeDeleted);
    QCOMPARE(event->propertyName(), "node");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), child->id());
    QVERIFY(!clone->parentNode());
}

void tst_Nodes::checkParentChangeToOtherParent()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    root->assignScene();
    ObserverSpy spy(root.data());
    QScopedPointer<MyQNode> parent1(new MyQNode(root.data()));
    QScopedPointer<MyQNode> parent2(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(spy.events.size(), 2);

    // WHEN
    ObserverSpy spyParent1(parent1.data());
    ObserverSpy spyParent2(parent2.data());
    QScopedPointer<Qt3D::QNode> child(new MyQNode(parent1.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == parent1.data());
    QCOMPARE(parent1->children().size(), 1);
    QCOMPARE(parent2->children().size(), 0);
    QVERIFY(Qt3D::QNodePrivate::get(child.data())->scene() != Q_NULLPTR);
    QCOMPARE(spyParent1.events.size(), 1);

    // WHEN
    spyParent1.events.clear();
    child->setParent(parent2.data());

    // THEN
    QVERIFY(child->parent() == parent2.data());
    QCOMPARE(parent1->children().size(), 0);
    QCOMPARE(parent2->children().size(), 1);
    QCOMPARE(spyParent1.events.size(), 1);
    QCOMPARE(spyParent2.events.size(), 1);

    // CHECK event 1 is  a Node Deleted event
    QVERIFY(spyParent1.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spyParent1.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeAboutToBeDeleted);
    QCOMPARE(event->propertyName(), "node");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), child->id());
    QVERIFY(!clone->parentNode());

    // CHECK event 2 is a Node Added event
    QVERIFY(spyParent2.events.last().wasLocked());
    event = spyParent2.events.last().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeCreated);
    QCOMPARE(event->propertyName(), "node");
    clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), child->id());
    QVERIFY(clone->parentNode());
}


void tst_Nodes::removingSingleChildNodeFromNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    QScopedPointer<Qt3D::QNode> child(new MyQNode());

    // WHEN
    child->setParent(root.data());

    // THEN
    QVERIFY(root->children().count() == 1);
    QVERIFY(child->parentNode() == root.data());

    // WHEN
    ObserverSpy spy(root.data());
    child->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(child->parent() == Q_NULLPTR);
    QVERIFY(root->children().count() == 0);

    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeAboutToBeDeleted);
    QCOMPARE(event->propertyName(), "node");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), child->id());
    QVERIFY(!clone->parentNode());
}

void tst_Nodes::removingMultipleChildNodesFromNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    root->assignScene();
    ObserverSpy spy(root.data());

    // THEN
    QVERIFY(Qt3D::QNodePrivate::get(root.data())->scene() != Q_NULLPTR);

    // WHEN
    for (int i = 0; i < 10; i++)
        (void) new MyQNode(root.data());

    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(root->children().count(), 10);
    QCOMPARE(spy.events.size(), 10);

    // WHEN
    spy.events.clear();
    Q_FOREACH (QObject *c, root->children())
        delete c;

    // THEN
    QVERIFY(root->children().count() == 0);
    QCOMPARE(spy.events.size(), 10);
    Q_FOREACH (const ObserverSpy::ChangeRecord &r, spy.events) {
        QVERIFY(r.wasLocked());
        Qt3D::QScenePropertyChangePtr event = r.change().dynamicCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(event->type(), Qt3D::NodeAboutToBeDeleted);
        QCOMPARE(event->propertyName(), "node");
        Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
        QVERIFY(!clone->parentNode());
    }
}

void tst_Nodes::appendingChildEntitiesToNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    Qt3D::QEntity *childEntity = new Qt3D::QEntity(root.data());

    // THEN
    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == Q_NULLPTR);
    QVERIFY(childEntity->parentNode() == root.data());
}

void tst_Nodes::removingChildEntitiesFromNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    Qt3D::QEntity *childEntity = new Qt3D::QEntity(root.data());

    // THEN
    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == Q_NULLPTR);
    QVERIFY(childEntity->parentNode() == root.data());

    // WHEN
    childEntity->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(root->children().isEmpty());
    QVERIFY(childEntity->parentNode() == Q_NULLPTR);
    QVERIFY(childEntity->parent() == Q_NULLPTR);
}

void tst_Nodes::appendingParentlessComponentToEntity()
{
    // GIVEN
    QScopedPointer<Qt3D::QEntity> entity(new Qt3D::QEntity());
    MyQComponent *comp = new MyQComponent();

    // THEN
    QVERIFY(entity->parentNode() == Q_NULLPTR);
    QVERIFY(entity->children().count() == 0);
    QVERIFY(entity->components().empty());
    QVERIFY(comp->parentNode() == Q_NULLPTR);

    // WHEN
    ObserverSpy spy(entity.data());
    entity->addComponent(comp);

    // THEN
    QVERIFY(entity->components().count() == 1);
    QVERIFY(entity->components().first() == comp);
    QVERIFY(comp->parentNode() == entity.data());
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());

    // Note: since QEntity has no scene in this test case, we only have the
    // ComponentAdded event In theory we should also get the NodeCreated event
    // when setting the parent but that doesn't happen since no scene is
    // actually set on the entity and that QNodePrivate::_q_addChild will
    // return early in such a case.

    // Check that we received ComponentAdded
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentAdded);
    QCOMPARE(event->propertyName(), "component");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), comp->id());
    QVERIFY(!clone->parentNode());
}

void tst_Nodes::appendingComponentToEntity()
{
    // GIVEN
    QScopedPointer<Qt3D::QEntity> entity(new Qt3D::QEntity());
    MyQComponent *comp = new MyQComponent(entity.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(entity->parentNode() == Q_NULLPTR);
    QVERIFY(entity->children().count() == 1);
    QVERIFY(entity->components().empty());
    QVERIFY(comp->parentNode() == entity.data());

    // WHEN
    ObserverSpy spy(entity.data());
    entity->addComponent(comp);

    // THEN
    QVERIFY(entity->components().count() == 1);
    QVERIFY(entity->components().first() == comp);
    QVERIFY(comp->parentNode() == entity.data());
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentAdded);
    QCOMPARE(event->propertyName(), "component");
    Qt3D::QNodePtr clone = event->value().value<Qt3D::QNodePtr>();
    QCOMPARE(clone->id(), comp->id());
    QVERIFY(!clone->parentNode());
}

void tst_Nodes::removingComponentFromEntity()
{
    // GIVEN
    QScopedPointer<Qt3D::QEntity> entity(new Qt3D::QEntity());
    MyQComponent *comp = new MyQComponent();

    // WHEN
    entity->addComponent(comp);

    // THEN
    QVERIFY(entity->components().count() == 1);
    QCOMPARE(entity->children().count(), 1);
    QVERIFY(comp->parent() == entity.data());

    ObserverSpy spy(entity.data());
    // WHEN
    entity->removeComponent(comp);

    // THEN
    QVERIFY(entity->components().count() == 0);
    QVERIFY(comp->parent() == entity.data());
    QVERIFY(entity->children().count() == 1);
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::ComponentRemoved);
    QCOMPARE(event->propertyName(), "componentId");
    Qt3D::QNodeId nodeId = event->value().value<Qt3D::QNodeId>();
    QCOMPARE(nodeId, comp->id());
}

void tst_Nodes::changeCustomProperty()
{
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    ObserverSpy spy(node.data());
    // WHEN
    node->setCustomProperty(QStringLiteral("foo"));
    // THEN
    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    Qt3D::QScenePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3D::QScenePropertyChange>();
    QCOMPARE(event->type(), Qt3D::NodeUpdated);
    QCOMPARE(event->propertyName(), "customProperty");
    QCOMPARE(event->value().toString(), QString("foo"));
}

void tst_Nodes::checkDestruction()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    Qt3D::QEntity *entity = new Qt3D::QEntity(root.data());

    MyQComponent *comp1 = new MyQComponent();
    MyQComponent *comp2 = new MyQComponent();
    MyQComponent *comp3 = new MyQComponent();

    entity->addComponent(comp1);
    entity->addComponent(comp2);
    entity->addComponent(comp3);

    // THEN
    QVERIFY(!root->children().isEmpty());

    // WHEN
    delete entity;

    // THEN
    QVERIFY(root->children().isEmpty());
}

void tst_Nodes::verifyCopy()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    MyQNode *other1 = new MyQNode();
    MyQNode *other2 = new MyQNode();

    // THEN
    QVERIFY(root->id() != other1->id());
    QVERIFY(root->id() != other2->id());
    QVERIFY(other1->id() != other2->id());

    // WHEN
    other1->makeCopyOf(root.data());

    // THEN
    QVERIFY(root->id() == other1->id());
    QVERIFY(root->id() != other2->id());

    // WHEN
    other2->makeCopyOf(other1);

    // THEN
    QVERIFY(root->id() == other1->id() && root->id() == other2->id());
}

QTEST_MAIN(tst_Nodes)

#include "tst_nodes.moc"
