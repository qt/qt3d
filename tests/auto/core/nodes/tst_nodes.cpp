// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qabstractaspect.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <private/qabstractaspect_p.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/vector_helper_p.h>
#include <QSet>
#include <QSignalSpy>
#include <testarbiter.h>
#include <vector>

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
    void checkParentChangeFromExistingBackendParentToNewlyCreatedParent();
    void checkBackendNodesCreatedFromTopDown();   //QTBUG-74106
    void checkBackendNodesCreatedFromTopDownWithReparenting();
    void checkAllBackendCreationDoneInSingleFrame();

    void removingSingleChildNodeFromNode();
    void removingMultipleChildNodesFromNode();

    void appendingChildEntitiesToNode();
    void removingChildEntitiesFromNode();

    void checkConstructionSetParentMix(); // QTBUG-60612
    void checkParentingQEntityToQNode(); // QTBUG-73905
    void checkConstructionWithParent();
    void checkConstructionWithNonRootParent(); // QTBUG-73986
    void checkConstructionAsListElement();
    void checkSceneIsSetOnConstructionWithParent(); // QTBUG-69352
    void checkSubNodePostConstructIsCalledWhenReferincingNodeProperty(); // QTBUG-79350

    void appendingComponentToEntity();
    void appendingParentlessComponentToEntityWithoutScene();
    void appendingParentlessComponentToEntityWithScene();
    void appendingParentlessComponentToNonRootEntity();
    void removingComponentFromEntity();

    void changeCustomProperty();
    void checkDestruction();

    void checkDefaultConstruction();
    void checkPropertyChanges();
    void checkEnabledUpdate();

    void checkNodeRemovedFromDirtyListOnDestruction();

    void checkBookkeepingSingleNode();
    void checkBookkeeping_QVector_OfNode();
    void checkBookkeeping_stdvector_OfNode();
};

class MyQNode : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QString customProperty READ customProperty WRITE setCustomProperty NOTIFY customPropertyChanged)
    Q_PROPERTY(Qt3DCore::QNode *nodeProperty READ nodeProperty WRITE setNodeProperty NOTIFY nodePropertyChanged)

public:
    explicit MyQNode(Qt3DCore::QNode *parent = nullptr)
        : QNode(parent)
        , m_nodeProperty(nullptr)
    {}

    ~MyQNode()
    {
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

    void setArbiterAndScene(Qt3DCore::QChangeArbiter *arbiter,
                            Qt3DCore::QScene *scene = nullptr)
    {
        Q_ASSERT(arbiter);
        if (scene)
            scene->setArbiter(arbiter);
        Qt3DCore::QNodePrivate::get(this)->setScene(scene);
        Qt3DCore::QNodePrivate::get(this)->setArbiter(arbiter);
    }

    void setSimulateBackendCreated(bool created)
    {
        Qt3DCore::QNodePrivate::get(this)->m_hasBackendNode = created;
    }

    Qt3DCore::QNode *nodeProperty() const { return m_nodeProperty; }

    const QList<Qt3DCore::QNode *> &attributes() const { return m_attributes; }

public slots:
    void setNodeProperty(Qt3DCore::QNode *node)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        if (m_nodeProperty == node)
            return;

        if (m_nodeProperty)
            d->unregisterDestructionHelper(m_nodeProperty);

        if (node && !node->parent())
            node->setParent(this);

        m_nodeProperty = node;

        // Ensures proper bookkeeping
        if (m_nodeProperty)
            d->registerDestructionHelper(m_nodeProperty, &MyQNode::setNodeProperty, m_nodeProperty);

        emit nodePropertyChanged(node);
    }

    void addAttribute(Qt3DCore::QNode *attribute)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        if (!m_attributes.contains(attribute)) {
            m_attributes.append(attribute);

            // Ensures proper bookkeeping
            d->registerDestructionHelper(attribute, &MyQNode::removeAttribute, m_attributes);

            // We need to add it as a child of the current node if it has been declared inline
            // Or not previously added as a child of the current node so that
            // 1) The backend gets notified about it's creation
            // 2) When the current node is destroyed, it gets destroyed as well
            if (!attribute->parent())
                attribute->setParent(this);

            d->update();
        }
    }

    void removeAttribute(Qt3DCore::QNode *attribute)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->update();

        m_attributes.removeOne(attribute);
        // Remove bookkeeping connection
        d->unregisterDestructionHelper(attribute);
    }

signals:
    void customPropertyChanged();
    void nodePropertyChanged(Qt3DCore::QNode *node);

protected:
    QString m_customProperty;
    Qt3DCore::QNode *m_nodeProperty;
    QList<Qt3DCore::QNode *> m_attributes;
};


class MyQNodeVec : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    explicit MyQNodeVec(Qt3DCore::QNode *parent = nullptr)
        : QNode(parent)
    {}

    ~MyQNodeVec()
    {
    }

    void setArbiterAndScene(Qt3DCore::QChangeArbiter *arbiter,
                            Qt3DCore::QScene *scene = nullptr)
    {
        Q_ASSERT(arbiter);
        if (scene)
            scene->setArbiter(arbiter);
        Qt3DCore::QNodePrivate::get(this)->setScene(scene);
        Qt3DCore::QNodePrivate::get(this)->setArbiter(arbiter);
    }

    void setSimulateBackendCreated(bool created)
    {
        Qt3DCore::QNodePrivate::get(this)->m_hasBackendNode = created;
    }

    const std::vector<Qt3DCore::QNode *> &attributes() const { return m_attributes; }

public slots:

    void addAttribute(Qt3DCore::QNode *attribute)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        if (!Qt3DCore::contains(m_attributes, attribute)) {
            m_attributes.push_back(attribute);

            // Ensures proper bookkeeping
            d->registerDestructionHelper(attribute, &MyQNodeVec::removeAttribute, m_attributes);

            // We need to add it as a child of the current node if it has been declared inline
            // Or not previously added as a child of the current node so that
            // 1) The backend gets notified about it's creation
            // 2) When the current node is destroyed, it gets destroyed as well
            if (!attribute->parent())
                attribute->setParent(this);

            d->update();
        }
    }

    void removeAttribute(Qt3DCore::QNode *attribute)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->update();

        m_attributes.erase(std::remove(m_attributes.begin(),
                                       m_attributes.end(),
                                       attribute), m_attributes.end());
        // Remove bookkeeping connection
        d->unregisterDestructionHelper(attribute);
    }

protected:
    std::vector<Qt3DCore::QNode *> m_attributes;
};

class MyQEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(MyQNode *nodeProperty READ nodeProperty WRITE setNodeProperty NOTIFY nodePropertyChanged)
public:
    explicit MyQEntity(Qt3DCore::QNode *parent = nullptr)
        : QEntity(parent)
    {}

    ~MyQEntity()
    {
    }

    void setArbiterAndScene(Qt3DCore::QChangeArbiter *arbiter,
                            Qt3DCore::QScene *scene = nullptr)
    {
        Q_ASSERT(arbiter);
        if (scene)
            scene->setArbiter(arbiter);
        Qt3DCore::QNodePrivate::get(this)->setScene(scene);
        Qt3DCore::QNodePrivate::get(this)->setArbiter(arbiter);
    }

    void setArbiterAndEngine(Qt3DCore::QChangeArbiter *arbiter,
                            Qt3DCore::QAspectEngine *engine)
    {
        Q_ASSERT(arbiter);
        Q_ASSERT(engine);
        auto scene = Qt3DCore::QAspectEnginePrivate::get(engine)->m_scene;
        Qt3DCore::QNodePrivate::get(this)->setScene(scene);
        Qt3DCore::QNodePrivate::get(this)->setArbiter(arbiter);

        if (scene) {
            scene->setArbiter(arbiter);
            engine->setRootEntity(Qt3DCore::QEntityPtr(this, [](QEntity *) {}));
            scene->setArbiter(arbiter);
            Qt3DCore::QNodePrivate::get(this)->setArbiter(arbiter);
        }
    }

    void setSimulateBackendCreated(bool created)
    {
        Qt3DCore::QNodePrivate::get(this)->m_hasBackendNode = created;
    }

    MyQNode *nodeProperty() const { return m_nodeProperty; }

    void addAttribute(MyQNode *attribute)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        if (!m_attributes.contains(attribute)) {
            m_attributes.append(attribute);

            // Ensures proper bookkeeping
            d->registerDestructionHelper(attribute, &MyQEntity::removeAttribute, m_attributes);

            // We need to add it as a child of the current node if it has been declared inline
            // Or not previously added as a child of the current node so that
            // 1) The backend gets notified about it's creation
            // 2) When the current node is destroyed, it gets destroyed as well
            if (!attribute->parent())
                attribute->setParent(this);

            d->update();
        }
    }

    void removeAttribute(MyQNode *attribute)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->update();

        m_attributes.removeOne(attribute);
        // Remove bookkeeping connection
        d->unregisterDestructionHelper(attribute);
    }

public slots:
    void setNodeProperty(MyQNode *node)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        if (m_nodeProperty == node)
            return;

        if (m_nodeProperty)
            d->unregisterDestructionHelper(m_nodeProperty);

        if (node && !node->parent())
            node->setParent(this);

        m_nodeProperty = node;

        // Ensures proper bookkeeping
        if (m_nodeProperty)
            d->registerDestructionHelper(m_nodeProperty, &MyQEntity::setNodeProperty, m_nodeProperty);

        emit nodePropertyChanged(node);
    }

signals:
    void nodePropertyChanged(MyQNode *node);

private:
    MyQNode *m_nodeProperty;
    QList<MyQNode *> m_attributes;
};

class MyQComponent : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit MyQComponent(Qt3DCore::QNode *parent = nullptr) : QComponent(parent)
    {}
    void setArbiter(Qt3DCore::QChangeArbiter *arbiter)
    {
        Q_ASSERT(arbiter);
        Qt3DCore::QComponentPrivate::get(this)->setArbiter(arbiter);
    }
};

class MyFakeMaterial : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit MyFakeMaterial(Qt3DCore::QNode *parent = nullptr)
      : QComponent(parent)
      , m_effect(new MyQNode(this))
      , m_technique(new MyQNode(m_effect))
      , m_renderPass(new MyQNode(m_technique))
    {
    }

    void setArbiter(Qt3DCore::QChangeArbiter *arbiter)
    {
        Q_ASSERT(arbiter);
        Qt3DCore::QComponentPrivate::get(this)->setArbiter(arbiter);
    }

    MyQNode *m_effect;
    MyQNode *m_technique;
    MyQNode *m_renderPass;
};

class TestAspectPrivate;
class TestAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    explicit TestAspect(QObject *parent = nullptr);

    enum ChangeType { Creation, Destruction };

    void clearNodes()
    {
        events.clear();
        allNodes.clear();
    }

    void addEvent(const Qt3DCore::QNodeId &id, ChangeType change)
    {
        events.push_back(Event{ change, id});
    }

    QList<Qt3DCore::QNodeId> filteredEvents(ChangeType change) const
    {
        QList<Qt3DCore::QNodeId> result;
        for (const auto &event : events) {
            if (event.type == change)
                result.push_back(event.nodeId);
        }
        return result;
    }

    struct Event{
        ChangeType type;
        Qt3DCore::QNodeId nodeId;
    };

    mutable QList<Event> events;
    mutable QHash<Qt3DCore::QNodeId, Qt3DCore::QNode *> allNodes;

private:
    Q_DECLARE_PRIVATE(TestAspect)
    explicit TestAspect(TestAspectPrivate &dd, QObject *parent);
};

class TestFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    TestFunctor(TestAspect *aspect) : m_aspect(aspect) {}

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override
    {
        auto node = new Qt3DCore::QBackendNode;
        m_Nodes.insert(id, node);
        m_aspect->addEvent(id, TestAspect::Creation);
        return node;
    }

    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override
    {
        return m_Nodes.value(id, nullptr);
    }

    void destroy(Qt3DCore::QNodeId id) const override
    {
        if (m_Nodes.contains(id)) {
            m_aspect->addEvent(id, TestAspect::Destruction);
            delete m_Nodes.take(id);
        }
    }

private:
    mutable QHash<Qt3DCore::QNodeId, Qt3DCore::QBackendNode *> m_Nodes;
    TestAspect *m_aspect;
};

class TestAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
public:
    TestAspectPrivate() = default;
    void syncDirtyFrontEndNode(Qt3DCore::QNode *node, Qt3DCore::QBackendNode *backend,
                               bool firstTime) const override
    {
        Q_UNUSED(backend);
        auto q = q_func();
        if (firstTime)
            q->allNodes.insert(node->id(), node);
    }

    Q_DECLARE_PUBLIC(TestAspect)
};

TestAspect::TestAspect(QObject *parent) : TestAspect(*new TestAspectPrivate, parent)
{
    registerBackendType<Qt3DCore::QEntity>(QSharedPointer<TestFunctor>::create(this));
    registerBackendType<MyQEntity>(QSharedPointer<TestFunctor>::create(this));
    registerBackendType<MyQNode>(QSharedPointer<TestFunctor>::create(this));
    registerBackendType<Qt3DCore::QNode>(QSharedPointer<TestFunctor>::create(this));
}

TestAspect::TestAspect(TestAspectPrivate &dd, QObject *parent)
    : Qt3DCore::QAbstractAspect(dd, parent)
{
    setObjectName(QStringLiteral("Test Aspect"));
}

namespace {
void verifyChildrenCreatedBeforeParents(Qt3DCore::QNode *root, TestAspect *aspect)
{
    QSet<Qt3DCore::QNodeId> processedNodes;
    processedNodes.insert(root->id());
    for (const auto &nodeId : aspect->filteredEvents(TestAspect::Creation)) {
        const auto node = aspect->allNodes.value(nodeId);
        Q_ASSERT(node);
        const auto parentNode = node->parentNode();
        QVERIFY(parentNode == nullptr || processedNodes.contains(parentNode->id()));
        processedNodes.insert(nodeId);
    }
}
}

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
    TestArbiter arbiter;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&arbiter);

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
    QCOMPARE(node->children().size(), 1);

    // Events are only sent when a scene is set on the root node
    QCOMPARE(arbiter.dirtyNodes().size(), 0);
}

void tst_Nodes::appendSingleChildNodeToNodeNoSceneImplicitParenting()
{
    // Check nodes added when no scene is set
    // GIVEN
    TestArbiter arbiter;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&arbiter);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() == nullptr);
    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode(node.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QCOMPARE(node->children().size(), 1);

    // Events are only sent when a scene is set on the root node
    QCOMPARE(arbiter.dirtyNodes().size(), 0);
}

void tst_Nodes::appendMultipleChildNodesToNodeNoScene()
{
    // Check multiple nodes added with no scene set
    // GIVEN
    TestArbiter arbiter;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&arbiter);

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
    QCOMPARE(node->children().size(), 10);

    // Events are only sent when a scene is set on the root node
    QCOMPARE(arbiter.dirtyNodes().size(), 0);
}

void tst_Nodes::appendSingleChildNodeToNodeSceneExplicitParenting()
{
    // Check nodes added when scene is set
    // GIVEN
    Qt3DCore::QScene scene;
    TestArbiter arbiter;
    QScopedPointer<MyQNode> node(new MyQNode());
    // WHEN
    node->setArbiterAndScene(&arbiter, &scene);
    node->setSimulateBackendCreated(true);

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
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // Child Added
    QCOMPARE(node->children().size(), 1);
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);
}

void tst_Nodes::appendSingleChildNodeToNodeSceneImplicitParenting()
{
    // Check nodes added when scene is set
    // GIVEN
    Qt3DCore::QScene scene;
    TestArbiter arbiter;
    QScopedPointer<MyQNode> node(new MyQNode());

    // WHEN
    node->setArbiterAndScene(&arbiter, &scene);
    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() != nullptr);

    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode(node.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);

    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(node->children().size(), 1);
}

void tst_Nodes::appendMultipleChildNodesToNodeScene()
{
    // Check nodes added when scene is set

    // GIVEN
    Qt3DCore::QScene scene;
    TestArbiter arbiter;
    QScopedPointer<MyQNode> node(new MyQNode());

    // WHEN
    node->setArbiterAndScene(&arbiter, &scene);
    node->setSimulateBackendCreated(true);
    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() != nullptr);

    // WHEN
    const auto childCount = 10;
    for (int i = 0; i < childCount; i++) {
        // WHEN
        Qt3DCore::QNode *child = nullptr;
        if (i % 2 == 0) {
            child = new MyQNode(node.data());
        } else {
            child = new MyQNode();
            child->setParent(node.data());
        }

        // THEN parent and scene should be set synchronously
        QVERIFY(child->parent() == node.data());
        QVERIFY(Qt3DCore::QNodePrivate::get(child)->scene() == Qt3DCore::QNodePrivate::get(node.data())->m_scene);
    }
    // THEN
    QCOMPARE(node->children().size(), 10);

    // WHEN
    QCoreApplication::processEvents();

    // THEN backend is notified after the event loop spins. Only the parent is notified
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
}

void tst_Nodes::checkParentChangeToNull()
{
    // GIVEN
    Qt3DCore::QScene scene;
    TestArbiter arbiter;
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    root->setArbiterAndScene(&arbiter, &scene);
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == root.data());
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(root->children().size(), 1);

    // WHEN
    arbiter.clear();
    child->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(child->parent() == nullptr);
    QCOMPARE(root->children().size(), 0);
    QCOMPARE(arbiter.dirtyNodes().size(), 0);
}

void tst_Nodes::checkParentChangeToOtherParent()
{
    // GIVEN
    Qt3DCore::QScene scene;
    TestArbiter arbiter;
    QScopedPointer<MyQNode> root(new MyQNode());
    root->setArbiterAndScene(&arbiter, &scene);
    QScopedPointer<MyQNode> parent1(new MyQNode(root.data()));
    QScopedPointer<MyQNode> parent2(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // only parent node has changed

    // WHEN
    arbiter.clear();
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode(parent1.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == parent1.data());
    QCOMPARE(parent1->children().size(), 1);
    QCOMPARE(parent2->children().size(), 0);
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // 1 node added to children change

    // WHEN
    arbiter.clear();
    child->setParent(parent2.data());

    // THEN
    QVERIFY(child->parent() == parent2.data());
    QCOMPARE(parent1->children().size(), 0);
    QCOMPARE(parent2->children().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().size(), 1);

    // CHECK event 1 is a Node Removed event
    // this no longer generated because nodes don't have backends yet

    QVERIFY(!Qt3DCore::QNodePrivate::get(child.data())->m_hasBackendNode);
}

void tst_Nodes::checkParentChangeFromExistingBackendParentToNewlyCreatedParent()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    engine.setRunMode(Qt3DCore::QAspectEngine::Manual);
    QScopedPointer<MyQEntity> root(new MyQEntity());
    root->setArbiterAndEngine(&arbiter, &engine);
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);

    MyQNode *child(new MyQNode(root.data()));
    MyQNode *child2(new MyQNode(root.data()));

    QCoreApplication::processEvents();
    engine.processFrame();

    // Due to the way we create root, it has a backend
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->m_hasBackendNode == true);
    QCOMPARE(aspect->events.size(), 2);
    QCOMPARE(aspect->events[0].type, TestAspect::Creation);
    QCOMPARE(aspect->events[0].nodeId, child->id());
    QCOMPARE(aspect->events[1].type, TestAspect::Creation);
    QCOMPARE(aspect->events[1].nodeId, child2->id());

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // only parent node has changed

    // WHEN -> Reparenting child with backend node to new parent with no backend yet
    aspect->clearNodes();
    arbiter.clear();
    QScopedPointer<Qt3DCore::QNode> newParent(new MyQNode(root.data()));
    child->setParent(newParent.data());

    // THEN
    QVERIFY(child->parent() == newParent.data());
    QCOMPARE(newParent->childNodes().size(), 1);
    QCOMPARE(child2->childNodes().size(), 0);
    QCOMPARE(root->childNodes().size(), 2);
    QVERIFY(Qt3DCore::QNodePrivate::get(newParent.data())->m_hasBackendNode == false);
    QVERIFY(Qt3DCore::QNodePrivate::get(child)->m_hasBackendNode == false);
    QVERIFY(Qt3DCore::QNodePrivate::get(newParent.data())->scene() != nullptr);
    QVERIFY(Qt3DCore::QNodePrivate::get(child)->scene() != nullptr);

    // WHEN
    QCoreApplication::processEvents();
    engine.processFrame();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // only parent node has changed
    QVERIFY(Qt3DCore::QNodePrivate::get(newParent.data())->m_hasBackendNode == true);
    QVERIFY(Qt3DCore::QNodePrivate::get(child)->m_hasBackendNode == true);

    // WHEN -> Changing parent to node with existing backend
    aspect->clearNodes();
    child->setParent(child2);

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 3); // old parent, new parent, and child have all changed
    arbiter.clear();

    QVERIFY(Qt3DCore::QNodePrivate::get(child)->m_hasBackendNode == true);
    QCOMPARE(root->childNodes().size(), 2);
    QCOMPARE(child2->childNodes().size(), 1);
    QCOMPARE(newParent->childNodes().size(), 0);

    // WHEN -> Changing to parent which has no backend
    QScopedPointer<Qt3DCore::QNode> newParent2(new MyQNode(root.data()));
    child->setParent(newParent2.data());

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(newParent2.data())->m_hasBackendNode == false);
    QVERIFY(Qt3DCore::QNodePrivate::get(child)->m_hasBackendNode == false);
    QVERIFY(Qt3DCore::QNodePrivate::get(newParent2.data())->scene() != nullptr);
    QVERIFY(Qt3DCore::QNodePrivate::get(child)->scene() != nullptr);

    // WHEN
    QCoreApplication::processEvents();
    engine.processFrame();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 2);
}

//Test creation changes happen for an entire subtree at once, starting at the top
// so that parents are always created before their children. Even if the front-end
// nodes are constructed in a different order.
void tst_Nodes::checkBackendNodesCreatedFromTopDown()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);
    QScopedPointer<MyQEntity> root(new MyQEntity());

    root->setArbiterAndEngine(&arbiter, &engine);
    QScopedPointer<Qt3DCore::QNode> parentWithBackend(new MyQNode(root.data()));

    // create parent backend node
    QCoreApplication::processEvents();
    QVERIFY(Qt3DCore::QNodePrivate::get(parentWithBackend.get())->m_hasBackendNode);

    // WHEN -> creating 3 nodes and setting one as a property on the other
    //         child2 is set as property on child1, but is created AFTER child1
    arbiter.clear();
    MyQNode *dummyParent(new MyQNode(parentWithBackend.data()));
    MyQNode *child1(new MyQNode(parentWithBackend.data()));
    MyQNode *child2(new MyQNode(dummyParent));
    child1->setNodeProperty(child2);

    // THEN - we should have no events because the new nodes have no backend yet
    QCOMPARE(arbiter.dirtyNodes().size(), 0);

    // WHEN - create the backend nodes
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(dummyParent->parent() == parentWithBackend.data());
    QVERIFY(child1->parent() == parentWithBackend.data());
    QVERIFY(child2->parent() == dummyParent);

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // only parent node has changed

    {
        QVERIFY(Qt3DCore::QNodePrivate::get(dummyParent)->m_hasBackendNode);
        QVERIFY(Qt3DCore::QNodePrivate::get(child1)->m_hasBackendNode);
        QVERIFY(Qt3DCore::QNodePrivate::get(child2)->m_hasBackendNode);

        verifyChildrenCreatedBeforeParents(root.get(), aspect);
    }

}

void tst_Nodes::checkBackendNodesCreatedFromTopDownWithReparenting()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);
    QScopedPointer<MyQEntity> root(new MyQEntity());

    root->setArbiterAndEngine(&arbiter, &engine);
    QScopedPointer<Qt3DCore::QNode> parentWithBackend(new MyQNode(root.data()));

    // create parent backend node
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(Qt3DCore::QNodePrivate::get(parentWithBackend.get())->m_hasBackendNode);

    // WHEN -> creating a node with a parent with backend, then reparenting it to another
    // parent with backend created after it.
    arbiter.clear();
    auto node1 = new MyQNode(parentWithBackend.data());
    auto parent1 = new MyQNode(parentWithBackend.data());
    node1->setParent(parent1);

    QCoreApplication::processEvents();

    // THEN
    QVERIFY(node1->parent() == parent1);
    QVERIFY(parent1->parent() == parentWithBackend.data());
    verifyChildrenCreatedBeforeParents(root.get(), aspect);


    // WHEN -> creating 2 nodes with no parent and reparenting the first to the second
    auto node2 = new MyQNode(nullptr);
    auto parent2 = new MyQNode(nullptr);
    node2->setParent(parent2);
    parent2->setParent(parentWithBackend.get());

    QCoreApplication::processEvents();

    // THEN
    QVERIFY(node2->parent() == parent2);
    QVERIFY(parent2->parent() == parentWithBackend.data());
    verifyChildrenCreatedBeforeParents(root.get(), aspect);
}

void tst_Nodes::removingSingleChildNodeFromNode()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode());

    // WHEN
    root->setArbiterAndEngine(&arbiter, &engine);
    root->setSimulateBackendCreated(true);
    child->setParent(root.data());

    // Clear any creation event
    arbiter.clear();

    // THEN
    QVERIFY(root->children().size() == 1);
    QVERIFY(child->parentNode() == root.data());

    // WHEN
    child->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(child->parent() == nullptr);
    QVERIFY(root->children().size() == 0);

    QCOMPARE(arbiter.dirtyNodes().size(), 1);
}

void tst_Nodes::checkAllBackendCreationDoneInSingleFrame()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    engine.setRunMode(Qt3DCore::QAspectEngine::Manual);
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);

    QScopedPointer<MyQEntity> root(new MyQEntity());
    root->setArbiterAndEngine(&arbiter, &engine);

    QCoreApplication::processEvents();

    // THEN
    // Due to the way we create root, it has a backend
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->m_hasBackendNode == true);
    QCOMPARE(aspect->events.size(), 1);
    QCOMPARE(aspect->events[0].type, TestAspect::Creation);
    QCOMPARE(aspect->events[0].nodeId, root->id());

    // WHEN -> create 2 children:
    //  1. a child with parent with backend node
    //  2. a child with no parent that is then reparented to a parent with backend node
    aspect->clearNodes();
    auto child1 = new MyQNode(root.data());
    auto child2 = new MyQNode;
    child2->setParent(root.data());

    // THEN - reparented child should have a backend node, but other child should
    //        still be waiting
    QCOMPARE(child1->parent(), root.data());
    QCOMPARE(child2->parent(), root.data());
    QVERIFY(Qt3DCore::QNodePrivate::get(child1)->m_hasBackendNode == false);
    QVERIFY(Qt3DCore::QNodePrivate::get(child2)->m_hasBackendNode == true);

    // WHEN
    QCoreApplication::processEvents();
    engine.processFrame();

    // THEN - both children have their backend nodes actually created.
    QCOMPARE(aspect->events.size(), 2);
    QCOMPARE(aspect->events[0].type, TestAspect::Creation);
    QCOMPARE(aspect->events[0].nodeId, child2->id());
    QCOMPARE(aspect->events[1].type, TestAspect::Creation);
    QCOMPARE(aspect->events[1].nodeId, child1->id());
}

void tst_Nodes::removingMultipleChildNodesFromNode()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&arbiter, &engine);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    Qt3DCore::QNodeIdVector childIds;
    for (int i = 0; i < 10; i++) {
        auto child = new MyQNode(root.data());
        childIds.push_back(child->id());
    }

    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(root->children().size(), 10);
    QCOMPARE(arbiter.dirtyNodes().size(), 1);

    // WHEN
    arbiter.clear();
    auto cl = root->children();
    qDeleteAll(cl);

    // THEN
    QVERIFY(root->children().size() == 0);
    QCOMPARE(arbiter.dirtyNodes().size(), 0);   // since all nodes are deleted, there's no backend to notify
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

void tst_Nodes::checkConstructionSetParentMix()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&arbiter, &engine);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    Qt3DCore::QEntity *subTreeRoot = new Qt3DCore::QEntity(root.data());

    for (int i = 0; i < 100; ++i) {
        Qt3DCore::QEntity *child = new Qt3DCore::QEntity();
        child->setParent(subTreeRoot);
    }

    // THEN
    QCoreApplication::processEvents();
    QCOMPARE(root->children().size(), 1);
    QCOMPARE(subTreeRoot->children().size(), 100);
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // 1 child added (subTree to root)

    // Ensure first event is subTreeRoot
    verifyChildrenCreatedBeforeParents(root.data(), aspect);
}

void tst_Nodes::checkParentingQEntityToQNode()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&arbiter, &engine);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    auto subTreeRoot = new Qt3DCore::QEntity(root.data());
    auto childEntity = new Qt3DCore::QEntity(subTreeRoot);
    auto childNode = new Qt3DCore::QNode(subTreeRoot);

    // THEN
    QCoreApplication::processEvents();
    QCOMPARE(arbiter.dirtyNodes().size(), 1);

    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(subTreeRoot)->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(childEntity)->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(childNode)->m_hasBackendNode);

    // WHEN we reparent the childEntity to the childNode (QNode)
    arbiter.clear();
    childEntity->setParent(childNode);
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 3);
}

void tst_Nodes::checkConstructionWithParent()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&arbiter, &engine);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN we create a child and then set it as a Node* property
    auto *node = new MyQNode(root.data());
    root->setNodeProperty(node);

    // THEN we should get one child added change
    QCoreApplication::processEvents();
    QCOMPARE(root->children().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // 1 child added change
    QCOMPARE(arbiter.dirtyNodes().front(), root.data());
}

void tst_Nodes::checkConstructionWithNonRootParent()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&arbiter, &engine);
    root->setSimulateBackendCreated(true);
    QScopedPointer<MyQNode> parent(new MyQNode(root.data()));

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);
    QVERIFY(Qt3DCore::QNodePrivate::get(parent.data())->scene() != nullptr);

    // WHEN we create a child and then set it as a Node* property
    auto *child = new MyQNode(parent.data());
    root->setNodeProperty(child);

    // THEN we should get
    // - one creation change for parent,
    // - one creation change for child,
    // - one child added change for root->parent,
    // - and one property change event,
    // in that order.
    QCoreApplication::processEvents();
    QCOMPARE(root->children().size(), 1);
    QCOMPARE(parent->children().size(), 1);

    QCOMPARE(arbiter.dirtyNodes().size(), 1); // 1 child added changes
    QCOMPARE(arbiter.dirtyNodes().front(), root.data());
}

void tst_Nodes::checkConstructionAsListElement()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&arbiter, &engine);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN we create a child and then set it as a Node* property
    auto *node = new MyQNode(root.data());
    root->addAttribute(node);

    // THEN we should get one creation change, one child added change
    // and one property change event, in that order.
    QCoreApplication::processEvents();

    QCOMPARE(root->children().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // 1 property change
}

void tst_Nodes::checkSceneIsSetOnConstructionWithParent()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QScene scene;
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    root->setArbiterAndScene(&arbiter, &scene);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    Qt3DCore::QEntity *subTreeRoot = new Qt3DCore::QEntity(root.data());

    QList<Qt3DCore::QEntity *> children;
    QList<Qt3DCore::QComponent *> cmps;
    children.reserve(5);
    cmps.reserve(5);
    for (int i = 0; i < 5; ++i) {
        const auto cmp = new MyQComponent(subTreeRoot);
        cmps << cmp;
        children << new Qt3DCore::QEntity(cmp);

        // When cmp is full created, it will also send the creation change for its child entity
    }
    QCoreApplication::processEvents();
    QCOMPARE(root->children().size(), 1);
    QCOMPARE(subTreeRoot->children().size(), 5);
    QCOMPARE(arbiter.dirtyNodes().size(), 1); // 1 child added (subTree to root)

    arbiter.clear();

    // Add component in a separate loop to ensure components are added after
    // entities have been fully created
    for (int i = 0; i < 5; ++i) {
        children.at(i)->addComponent(cmps.at(i));
    }

    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 5);
}

void tst_Nodes::checkSubNodePostConstructIsCalledWhenReferincingNodeProperty()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;
    Qt3DCore::QScene scene(&engine);
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    root->setArbiterAndScene(&arbiter, &scene);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    Qt3DCore::QEntity *subTreeRoot = new Qt3DCore::QEntity(root.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(subTreeRoot)->m_hasBackendNode);

    // WHEN
    MyFakeMaterial *material = new MyFakeMaterial(subTreeRoot);
    subTreeRoot->addComponent(material);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(material)->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(material->m_effect)->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(material->m_technique)->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(material->m_renderPass)->m_hasBackendNode);
}

void tst_Nodes::appendingParentlessComponentToEntityWithoutScene()
{
    // GIVEN
    TestArbiter arbiter;
    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndScene(&arbiter);
        entity->setSimulateBackendCreated(true);

        MyQComponent *comp = new MyQComponent();
        comp->setArbiter(&arbiter);

        // THEN
        QVERIFY(entity->parentNode() == nullptr);
        QVERIFY(entity->children().size() == 0);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == nullptr);

        // WHEN
        entity->addComponent(comp);

        // THEN
        QVERIFY(entity->components().size() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());
        QCOMPARE(arbiter.dirtyNodes().size(), 1);

        // Note: since QEntity has no scene in this test case, we only have the
        // ComponentAdded event In theory we should also get the NodeCreated event
        // when setting the parent but that doesn't happen since no scene is
        // actually set on the entity and that QNodePrivate::_q_addChild will
        // return early in such a case.
    }
}

void tst_Nodes::appendingParentlessComponentToNonRootEntity()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;

    {
        QScopedPointer<MyQEntity> root(new MyQEntity());
        root->setArbiterAndEngine(&arbiter, &engine);
        root->setSimulateBackendCreated(true);

        QCoreApplication::processEvents();

        QScopedPointer<MyQEntity> entity(new MyQEntity(root.data()));
        MyQComponent *comp = new MyQComponent();

        // THEN
        QVERIFY(root->parentNode() == nullptr);
        QVERIFY(root->children().size() == 1);
        QVERIFY(root->components().empty());
        QVERIFY(entity->parentNode() == root.data());
        QVERIFY(entity->children().size() == 0);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == nullptr);

        // WHEN
        entity->addComponent(comp);
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(entity->components().size() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());

        QCOMPARE(arbiter.dirtyNodes().size(), 2);
        QCOMPARE(arbiter.dirtyNodes().at(0), root.data());
        QCOMPARE(arbiter.dirtyNodes().at(1), entity.data());

        QVERIFY(Qt3DCore::QNodePrivate::get(entity.data())->m_hasBackendNode);
        QVERIFY(Qt3DCore::QNodePrivate::get(comp)->m_hasBackendNode);
    }
}

void tst_Nodes::appendingParentlessComponentToEntityWithScene()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QAspectEngine engine;

    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndEngine(&arbiter, &engine);
        entity->setSimulateBackendCreated(true);

        QCoreApplication::processEvents();

        MyQComponent *comp = new MyQComponent();

        // THEN
        QVERIFY(entity->parentNode() == nullptr);
        QVERIFY(entity->children().size() == 0);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == nullptr);

        // WHEN
        entity->addComponent(comp);
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(entity->components().size() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());

        QCOMPARE(arbiter.dirtyNodes().size(), 1);
    }
}


void tst_Nodes::appendingComponentToEntity()
{
    // GIVEN
    TestArbiter arbiter;
    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndScene(&arbiter);
        MyQComponent *comp = new MyQComponent(entity.data());
        comp->setArbiter(&arbiter);
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(entity->parentNode() == nullptr);
        QVERIFY(entity->children().size() == 1);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == entity.data());

        // WHEN
        entity->addComponent(comp);

        // THEN
        QVERIFY(entity->components().size() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().first(), entity.data());
    }
}

void tst_Nodes::removingComponentFromEntity()
{
    // GIVEN
    TestArbiter arbiter;
    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndScene(&arbiter);
        MyQComponent *comp = new MyQComponent();
        comp->setArbiter(&arbiter);

        // WHEN
        entity->addComponent(comp);

        // THEN
        QVERIFY(entity->components().size() == 1);
        QCOMPARE(entity->children().size(), 1);
        QVERIFY(comp->parent() == entity.data());

        // WHEN
        arbiter.clear();
        entity->removeComponent(comp);

        // THEN
        QVERIFY(entity->components().size() == 0);
        QVERIFY(comp->parent() == entity.data());
        QVERIFY(entity->children().size() == 1);
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
    }
}

void tst_Nodes::changeCustomProperty()
{
    // GIVEN
    TestArbiter arbiter;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&arbiter);
    // WHEN
    node->setCustomProperty(QStringLiteral("foo"));
    // THEN
    QCOMPARE(arbiter.dirtyNodes().size(), 1);
    QCOMPARE(arbiter.dirtyNodes().front(), node.data());
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

void tst_Nodes::checkDefaultConstruction()
{
    // GIVEN
    Qt3DCore::QNode node;

    // THEN
    QCOMPARE(node.parentNode(), nullptr);
    QCOMPARE(node.isEnabled(), true);
}

void tst_Nodes::checkPropertyChanges()
{
    // GIVEN
    Qt3DCore::QNode parentNode;
    Qt3DCore::QNode node;

    {
        // WHEN
        QSignalSpy spy(&node, SIGNAL(parentChanged(QObject*)));
        Qt3DCore::QNode *newValue = &parentNode;
        node.setParent(newValue);

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(node.parentNode(), newValue);
        QCOMPARE(spy.size(), 1);

        // WHEN
        spy.clear();
        node.setParent(newValue);

        // THEN
        QCOMPARE(node.parentNode(), newValue);
        QCOMPARE(spy.size(), 0);
    }
    {
        // WHEN
        QSignalSpy spy(&node, SIGNAL(enabledChanged(bool)));
        const bool newValue = false;
        node.setEnabled(newValue);

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(node.isEnabled(), newValue);
        QCOMPARE(spy.size(), 1);

        // WHEN
        spy.clear();
        node.setEnabled(newValue);

        // THEN
        QCOMPARE(node.isEnabled(), newValue);
        QCOMPARE(spy.size(), 0);
    }
}

void tst_Nodes::checkEnabledUpdate()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QNode node;
    arbiter.setArbiterOnNode(&node);

    {
        // WHEN
        node.setEnabled(false);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), &node);

        arbiter.clear();
    }

    {
        // WHEN
        node.setEnabled(false);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
    }

}

void tst_Nodes::checkNodeRemovedFromDirtyListOnDestruction()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QScene scene;

    {
        // GIVEN
        QScopedPointer<MyQNode> node(new MyQNode());
        node->setArbiterAndScene(&arbiter, &scene);

        // WHEN
        node->setEnabled(false);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), node.data());

        // WHEN
        // scene should be unset and node removed from arbiter dirtyList
        node.reset();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
    }
    {
        // GIVEN
        QScopedPointer<MyQNode> node(new MyQNode());
        node->setArbiterAndScene(&arbiter, &scene);


        Qt3DCore::QNode *child = new Qt3DCore::QNode(node.data());
        // Wait for deferred initialization of child node
        QCoreApplication::processEvents();

        // WHEN
        child->setEnabled(false);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 2);
        QCOMPARE(arbiter.dirtyNodes().at(0), node.data());
        QCOMPARE(arbiter.dirtyNodes().at(1), child);

        // WHEN
        // scene should be unset and child node removed from arbiter dirtyList
        node.reset();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
    }
}

void tst_Nodes::checkBookkeepingSingleNode()
{
    // GIVEN
    MyQNode root;

    // THEN
    QVERIFY(root.nodeProperty() == nullptr);

    {
        // WHEN
        MyQNode node;
        root.setNodeProperty(&node);

        // THEN
        QCOMPARE(root.nodeProperty(), &node);
    }
    // WHEN -> node destroyed

    // THEN
    QVERIFY(root.nodeProperty() == nullptr);
}


void tst_Nodes::checkBookkeeping_QVector_OfNode()
{
    // GIVEN
    MyQNode root;

    // THEN
    QVERIFY(root.attributes().size() == 0);

    {
        // WHEN
        MyQNode node1;
        root.addAttribute(&node1);

        // THEN
        QVERIFY(root.attributes().size() == 1);

        // WHEN
        {
            MyQNode node2;
            root.addAttribute(&node2);

            // THEN
            QVERIFY(root.attributes().size() == 2);
            QCOMPARE(root.attributes().first(), &node1);
            QCOMPARE(root.attributes().last(), &node2);
        }

        // THEN
        QVERIFY(root.attributes().size() == 1);
        QCOMPARE(root.attributes().first(), &node1);
    }

    // THEN
    QVERIFY(root.attributes().size() == 0);
}

void tst_Nodes::checkBookkeeping_stdvector_OfNode()
{
    // GIVEN
    MyQNodeVec root;

    // THEN
    QVERIFY(root.attributes().size() == 0U);

    {
        // WHEN
        MyQNode node1;
        root.addAttribute(&node1);

        // THEN
        QVERIFY(root.attributes().size() == 1U);

        // WHEN
        {
            MyQNode node2;
            root.addAttribute(&node2);

            // THEN
            QVERIFY(root.attributes().size() == 2U);
            QCOMPARE(root.attributes().front(), &node1);
            QCOMPARE(root.attributes().back(), &node2);
        }

        // THEN
        QVERIFY(root.attributes().size() == 1U);
        QCOMPARE(root.attributes().front(), &node1);
    }

    // THEN
    QVERIFY(root.attributes().size() == 0U);
}


QTEST_MAIN(tst_Nodes)

#include "tst_nodes.moc"
