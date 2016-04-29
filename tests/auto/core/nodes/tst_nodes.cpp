/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qnodepropertychange.h>
#include <Qt3DCore/qcomponentaddedchange.h>
#include <Qt3DCore/qcomponentremovedchange.h>
#include <Qt3DCore/qnodedestroyedchange.h>
#include <Qt3DCore/qnodeaddedpropertychange.h>
#include <Qt3DCore/qnoderemovedpropertychange.h>
#include <private/qpostman_p.h>

#include <Qt3DCore/private/qlockableobserverinterface_p.h>
#include <Qt3DCore/private/qnode_p.h>

class tst_Nodes : public QObject
{
    Q_OBJECT
public:
    tst_Nodes() : QObject()
    {
        qRegisterMetaType<Qt3DCore::QNode*>();
    }
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
};

class ObserverSpy;
class SimplePostman : public Qt3DCore::QAbstractPostman
{
public:
    SimplePostman(ObserverSpy *spy)
        : m_spy(spy)
    {}

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &) Q_DECL_FINAL {};
    void setScene(Qt3DCore::QScene *) Q_DECL_FINAL {};
    void notifyBackend(const Qt3DCore::QSceneChangePtr &change) Q_DECL_FINAL;

private:
    ObserverSpy *m_spy;
};

class ObserverSpy : public Qt3DCore::QAbstractArbiter
{
public:
    class ChangeRecord : public QPair<Qt3DCore::QSceneChangePtr, bool>
    {
    public:
        ChangeRecord(const Qt3DCore::QSceneChangePtr &event, bool locked)
            : QPair<Qt3DCore::QSceneChangePtr, bool>(event, locked)
        {}

        Qt3DCore::QSceneChangePtr change() const { return first; }

        bool wasLocked() const { return second; }
    };

    ObserverSpy(Qt3DCore::QNode *node)
        : Qt3DCore::QAbstractArbiter()
        , m_node(node)
        , m_postman(new SimplePostman(this))
    {
        Qt3DCore::QNodePrivate::get(node)->setArbiter(this);
    }

    ~ObserverSpy()
    {
        Qt3DCore::QNodePrivate::get(m_node)->setArbiter(nullptr);
    }

    void sceneChangeEventWithLock(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        events << ChangeRecord(e, true);
    }

    void sceneChangeEventWithLock(const Qt3DCore::QSceneChangeList &e) Q_DECL_OVERRIDE
    {
        for (uint i = 0, m = e.size(); i < m; ++i) {
            events << ChangeRecord(e.at(i), false);
        }
    }

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        events << ChangeRecord(e, false);
    }

    Qt3DCore::QAbstractPostman *postman() const Q_DECL_FINAL
    {
        return m_postman.data();
    }

    QList<ChangeRecord> events;
    Qt3DCore::QNode *m_node;
    QScopedPointer<SimplePostman> m_postman;
};

void SimplePostman::notifyBackend(const Qt3DCore::QSceneChangePtr &change)
{
    m_spy->sceneChangeEventWithLock(change);
}

class MyQNode : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QString customProperty READ customProperty WRITE setCustomProperty NOTIFY customPropertyChanged)
public:
    explicit MyQNode(Qt3DCore::QNode *parent = 0)
        : QNode(parent)
        , m_scene(nullptr)
    {}

    ~MyQNode()
    {
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

    void assignScene(Qt3DCore::QAbstractArbiter *arbiter)
    {
        if (!m_scene) {
            m_scene = new Qt3DCore::QScene();
            if (arbiter)
                m_scene->setArbiter(arbiter);
        }
        Qt3DCore::QNodePrivate::get(this)->setScene(m_scene);
    }

signals:
    void customPropertyChanged();

protected:
    QString m_customProperty;
    Qt3DCore::QScene *m_scene;
};

class MyQComponent : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit MyQComponent(Qt3DCore::QNode *parent = 0) : QComponent(parent)
    {}
};


void tst_Nodes::defaultNodeConstruction()
{
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());

    // THEN
    QVERIFY(node != nullptr);
    QVERIFY(node->children().isEmpty());

    // GIVEN
    MyQNode *node2 = new MyQNode(node.data());
    QCoreApplication::processEvents();

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
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(comp != nullptr);
    QCOMPARE(comp2->parent(), comp.data());
}

void tst_Nodes::defaultEntityConstrution()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity());
    Qt3DCore::QEntity *entity2 = new Qt3DCore::QEntity(entity.data());
    QCoreApplication::processEvents();

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
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() == nullptr);
    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode());

    // THEN
    QVERIFY(child->parent() == nullptr);

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
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() == nullptr);
    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode(node.data()));
    QCoreApplication::processEvents();

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
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() == nullptr);
    // WHEN
    for (int i = 0; i < 10; i++) {
        // WHEN
        Qt3DCore::QNode *child = nullptr;
        if (i % 2 == 0) {
            child = new MyQNode(node.data());
            QCoreApplication::processEvents();
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
    node->assignScene(&spy);
    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() != nullptr);

    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode());

    // THEN
    QVERIFY(child->parent() == nullptr);
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() == nullptr);

    // WHEN
    child->setParent(node.data());

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QCOMPARE(spy.events.size(), 2);
    QCOMPARE(node->children().count(), 1);
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);

    // Creation event
    QVERIFY(spy.events.first().wasLocked());
    Qt3DCore::QNodeCreatedChangeBasePtr creationEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeCreatedChangeBase>();
    QVERIFY(creationEvent);
    QCOMPARE(creationEvent->subjectId(), child->id());
    QCOMPARE(creationEvent->metaObject(), child->metaObject());
    QCOMPARE(creationEvent->isNodeEnabled(), child->isEnabled());
    QCOMPARE(creationEvent->parentId(), child->parentNode()->id());

    // Node Added event
    QVERIFY(spy.events.first().wasLocked());
    Qt3DCore::QNodeAddedPropertyChangePtr additionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeAddedPropertyChange>();
    QVERIFY(additionEvent);
    QCOMPARE(additionEvent->subjectId(), node->id());
    QCOMPARE(additionEvent->addedNodeId(), child->id());
    QCOMPARE(additionEvent->metaObject(), child->metaObject());
}

void tst_Nodes::appendSingleChildNodeToNodeSceneImplicitParenting()
{
    // Check nodes added when scene is set
    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    ObserverSpy spy(node.data());

    // WHEN
    node->assignScene(&spy);
    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() != nullptr);

    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode(node.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);

    QCOMPARE(spy.events.size(), 2);
    QVERIFY(spy.events.first().wasLocked());
    QCOMPARE(node->children().count(), 1);

    // Creation event
    QVERIFY(spy.events.first().wasLocked());
    Qt3DCore::QNodeCreatedChangeBasePtr creationEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeCreatedChangeBase>();
    QVERIFY(creationEvent);
    QCOMPARE(creationEvent->subjectId(), child->id());
    QCOMPARE(creationEvent->metaObject(), child->metaObject());
    QCOMPARE(creationEvent->isNodeEnabled(), child->isEnabled());
    QCOMPARE(creationEvent->parentId(), child->parentNode()->id());

    // Node Added event
    QVERIFY(spy.events.first().wasLocked());
    Qt3DCore::QNodeAddedPropertyChangePtr additionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeAddedPropertyChange>();
    QVERIFY(additionEvent);
    QCOMPARE(additionEvent->subjectId(), node->id());
    QCOMPARE(additionEvent->addedNodeId(), child->id());
    QCOMPARE(additionEvent->metaObject(), child->metaObject());
}

void tst_Nodes::appendMultipleChildNodesToNodeScene()
{
    // Check nodes added when scene is set

    // GIVEN
    QScopedPointer<MyQNode> node(new MyQNode());
    // WHEN
    ObserverSpy spy(node.data());
    node->assignScene(&spy);
    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() != nullptr);

    // WHEN
    for (int i = 0; i < 10; i++) {
        // WHEN
        Qt3DCore::QNode *child = nullptr;
        if (i % 2 == 0) {
            child = new MyQNode(node.data());
            QCoreApplication::processEvents();
            QCOMPARE(spy.events.size(), 2 * (i + 1));
        } else {
            child = new MyQNode();
            child->setParent(node.data());
        }
        // THEN
        QVERIFY(child->parent() == node.data());
        QVERIFY(Qt3DCore::QNodePrivate::get(child)->scene() != nullptr);
    }
    // THEN
    QCOMPARE(node->children().count(), 10);

    // THEN
    QCOMPARE(spy.events.size(), 10 * 2);
    int i = 0;
    Q_FOREACH (const ObserverSpy::ChangeRecord &r, spy.events) {
        QVERIFY(r.wasLocked());
        Qt3DCore::QNode *child = node->childNodes().at(i / 2);
        if (i % 2 == 0) {
            const Qt3DCore::QNodeCreatedChangeBasePtr event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeCreatedChangeBase>();
            QCOMPARE(event->subjectId(), child->id());
            QCOMPARE(event->metaObject(), child->metaObject());
            QCOMPARE(event->isNodeEnabled(), child->isEnabled());
        } else {
            Qt3DCore::QNodeAddedPropertyChangePtr additionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeAddedPropertyChange>();
            QCOMPARE(additionEvent->subjectId(), node->id());
            QCOMPARE(additionEvent->addedNodeId(), child->id());
            QCOMPARE(additionEvent->metaObject(), child->metaObject());
        }
        ++i;
    }
}

void tst_Nodes::checkParentChangeToNull()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    ObserverSpy spy(root.data());

    // WHEN
    root->assignScene(&spy);
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == root.data());
    QCOMPARE(spy.events.size(), 2);
    QCOMPARE(root->children().size(), 1);

    // WHEN
    spy.events.clear();
    child->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(child->parent() == nullptr);
    QCOMPARE(root->children().size(), 0);
    QCOMPARE(spy.events.size(), 2);

    QVERIFY(spy.events.first().wasLocked());
    const Qt3DCore::QNodeRemovedPropertyChangePtr removalEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeRemovedPropertyChange>();
    QVERIFY(removalEvent);
    QCOMPARE(removalEvent->subjectId(), root->id());
    QCOMPARE(removalEvent->removedNodeId(), child->id());
    QCOMPARE(removalEvent->metaObject(), child->metaObject());

    QVERIFY(spy.events.first().wasLocked());
    const Qt3DCore::QNodeDestroyedChangePtr destructionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeDestroyedChange>();
    QVERIFY(destructionEvent);
    QCOMPARE(destructionEvent->type(), Qt3DCore::NodeDeleted);
    QCOMPARE(destructionEvent->subjectId(), child->id());
    QCOMPARE(destructionEvent->subtreeIdsAndTypes().size(), 1);
    QCOMPARE(destructionEvent->subtreeIdsAndTypes().first().id, child->id());
    QCOMPARE(destructionEvent->subtreeIdsAndTypes().first().type, child->metaObject());
}

void tst_Nodes::checkParentChangeToOtherParent()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    ObserverSpy spy(root.data());
    root->assignScene(&spy);
    QScopedPointer<MyQNode> parent1(new MyQNode(root.data()));
    QScopedPointer<MyQNode> parent2(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(spy.events.size(), 4);

    // WHEN
    ObserverSpy spyParent1(parent1.data());
    ObserverSpy spyParent2(parent2.data());
    parent1->assignScene(&spyParent1);
    parent2->assignScene(&spyParent2);

    QScopedPointer<Qt3DCore::QNode> child(new MyQNode(parent1.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == parent1.data());
    QCOMPARE(parent1->children().size(), 1);
    QCOMPARE(parent2->children().size(), 0);
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);
    QCOMPARE(spyParent1.events.size(), 2);

    // WHEN
    spyParent1.events.clear();
    child->setParent(parent2.data());

    // THEN
    QVERIFY(child->parent() == parent2.data());
    QCOMPARE(parent1->children().size(), 0);
    QCOMPARE(parent2->children().size(), 1);
    QCOMPARE(spyParent1.events.size(), 1);
    QCOMPARE(spyParent2.events.size(), 1);

    // CHECK event 1 is a Node Removed event
    QVERIFY(spyParent1.events.first().wasLocked());
    const Qt3DCore::QNodeRemovedPropertyChangePtr event = spyParent1.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeRemovedPropertyChange>();
    QCOMPARE(event->type(), Qt3DCore::NodeRemoved);
    QCOMPARE(event->subjectId(), parent1->id());
    QCOMPARE(event->removedNodeId(), child->id());
    QCOMPARE(event->metaObject(), child->metaObject());

    // CHECK event 2 is a Node Added event
    QVERIFY(spyParent2.events.last().wasLocked());
    const Qt3DCore::QNodeAddedPropertyChangePtr event2 = spyParent2.events.last().change().dynamicCast<Qt3DCore::QNodeAddedPropertyChange>();
    QCOMPARE(event2->type(), Qt3DCore::NodeAdded);
    QCOMPARE(event2->subjectId(), parent2->id());
    QCOMPARE(event2->addedNodeId(), child->id());
    QCOMPARE(event2->metaObject(), child->metaObject());
}

void tst_Nodes::removingSingleChildNodeFromNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode());
    ObserverSpy spy(root.data());

    // WHEN
    root->assignScene(&spy);
    child->setParent(root.data());

    // Clear any creation event
    spy.events.clear();

    // THEN
    QVERIFY(root->children().count() == 1);
    QVERIFY(child->parentNode() == root.data());

    // WHEN
    child->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(child->parent() == nullptr);
    QVERIFY(root->children().count() == 0);

    QCOMPARE(spy.events.size(), 2);

    QVERIFY(spy.events.first().wasLocked());
    const Qt3DCore::QNodeRemovedPropertyChangePtr removalEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeRemovedPropertyChange>();
    QCOMPARE(removalEvent->subjectId(), root->id());
    QCOMPARE(removalEvent->removedNodeId(), child->id());
    QCOMPARE(removalEvent->metaObject(), child->metaObject());

    QVERIFY(spy.events.first().wasLocked());
    const Qt3DCore::QNodeDestroyedChangePtr destructionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeDestroyedChange>();
    QCOMPARE(destructionEvent->type(), Qt3DCore::NodeDeleted);
    QCOMPARE(destructionEvent->subjectId(), child->id());
    QCOMPARE(destructionEvent->subtreeIdsAndTypes().size(), 1);
    QCOMPARE(destructionEvent->subtreeIdsAndTypes().first().id, child->id());
    QCOMPARE(destructionEvent->subtreeIdsAndTypes().first().type, child->metaObject());
}

void tst_Nodes::removingMultipleChildNodesFromNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    ObserverSpy spy(root.data());
    root->assignScene(&spy);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    Qt3DCore::QNodeIdVector childIds(10);
    for (int i = 0; i < 10; i++) {
        auto child = new MyQNode(root.data());
        childIds[i] = child->id();
    }

    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(root->children().count(), 10);
    QCOMPARE(spy.events.size(), 20);

    // WHEN
    spy.events.clear();
    Q_FOREACH (QObject *c, root->children())
        delete c;

    // THEN
    QVERIFY(root->children().count() == 0);
    QCOMPARE(spy.events.size(), 20);
    int i = 0;
    Q_FOREACH (const ObserverSpy::ChangeRecord &r, spy.events) {
        QVERIFY(r.wasLocked());
        const Qt3DCore::QNodeId childId = childIds.at(i / 2);
        if (i % 2 == 0) {
            Qt3DCore::QNodeRemovedPropertyChangePtr additionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeRemovedPropertyChange>();
            QCOMPARE(additionEvent->subjectId(), root->id());
            QCOMPARE(additionEvent->removedNodeId(), childId);
            QCOMPARE(additionEvent->metaObject(), &MyQNode::staticMetaObject);
        } else {
            const Qt3DCore::QNodeDestroyedChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodeDestroyedChange>();
            QCOMPARE(event->subjectId(), childId);
            QCOMPARE(event->subtreeIdsAndTypes().count(), 1);
            QCOMPARE(event->subtreeIdsAndTypes().first().id, childId);
            QCOMPARE(event->subtreeIdsAndTypes().first().type, &MyQNode::staticMetaObject);
            QCOMPARE(event->type(), Qt3DCore::NodeDeleted);
        }
        ++i;
    }
}

void tst_Nodes::appendingChildEntitiesToNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    Qt3DCore::QEntity *childEntity = new Qt3DCore::QEntity(root.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == nullptr);
    QVERIFY(childEntity->parentNode() == root.data());
}

void tst_Nodes::removingChildEntitiesFromNode()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    Qt3DCore::QEntity *childEntity = new Qt3DCore::QEntity(root.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == nullptr);
    QVERIFY(childEntity->parentNode() == root.data());

    // WHEN
    childEntity->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(root->children().isEmpty());
    QVERIFY(childEntity->parentNode() == nullptr);
    QVERIFY(childEntity->parent() == nullptr);
}

void tst_Nodes::appendingParentlessComponentToEntity()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity());
    MyQComponent *comp = new MyQComponent();

    // THEN
    QVERIFY(entity->parentNode() == nullptr);
    QVERIFY(entity->children().count() == 0);
    QVERIFY(entity->components().empty());
    QVERIFY(comp->parentNode() == nullptr);

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
    const auto event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QComponentAddedChange>();
    QCOMPARE(event->type(), Qt3DCore::ComponentAdded);
    QCOMPARE(event->subjectId(), entity->id());
    QCOMPARE(event->entityId(), entity->id());
    QCOMPARE(event->componentId(), comp->id());
    QCOMPARE(event->componentMetaObject(), comp->metaObject());
}

void tst_Nodes::appendingComponentToEntity()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity());
    MyQComponent *comp = new MyQComponent(entity.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(entity->parentNode() == nullptr);
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
    const auto event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QComponentAddedChange>();
    QCOMPARE(event->type(), Qt3DCore::ComponentAdded);
    QCOMPARE(event->subjectId(), entity->id());
    QCOMPARE(event->entityId(), entity->id());
    QCOMPARE(event->componentId(), comp->id());
    QCOMPARE(event->componentMetaObject(), comp->metaObject());
}

void tst_Nodes::removingComponentFromEntity()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity());
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
    const auto event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QComponentRemovedChange>();
    QCOMPARE(event->type(), Qt3DCore::ComponentRemoved);
    QCOMPARE(event->subjectId(), entity->id());
    QCOMPARE(event->entityId(), entity->id());
    QCOMPARE(event->componentId(), comp->id());
    QCOMPARE(event->componentMetaObject(), comp->metaObject());
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
    Qt3DCore::QNodePropertyChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QNodePropertyChange>();
    QCOMPARE(event->type(), Qt3DCore::NodeUpdated);
    QCOMPARE(event->propertyName(), "customProperty");
    QCOMPARE(event->value().toString(), QString("foo"));
}

void tst_Nodes::checkDestruction()
{
    // GIVEN
    QScopedPointer<MyQNode> root(new MyQNode());
    Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(root.data());

    QCoreApplication::processEvents();

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

QTEST_MAIN(tst_Nodes)

#include "tst_nodes.moc"
