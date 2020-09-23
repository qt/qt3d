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

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qabstractaspect.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qcomponentaddedchange.h>
#include <Qt3DCore/qcomponentremovedchange.h>
#include <Qt3DCore/qnodedestroyedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <private/qabstractaspect_p.h>
#include <private/qpostman_p.h>

#include <Qt3DCore/private/qlockableobserverinterface_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <QSignalSpy>
#include "testpostmanarbiter.h"
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
    void initTestCase();
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
    void checkCreationData();
    void checkEnabledUpdate();
    void checkPropertyTrackModeUpdate();
    void checkTrackedPropertyNamesUpdate();

    void checkNodeRemovedFromDirtyListOnDestruction();

    void checkBookkeepingSingleNode();
    void checkBookkeeping_QVector_OfNode();
    void checkBookkeeping_stdvector_OfNode();
};

class ObserverSpy;
class SimplePostman : public Qt3DCore::QAbstractPostman
{
public:
    SimplePostman(ObserverSpy *spy)
        : m_spy(spy)
    {}

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &) final {}
    void setScene(Qt3DCore::QScene *) final {}
    void notifyBackend(const Qt3DCore::QSceneChangePtr &change) final;
    bool shouldNotifyFrontend(const Qt3DCore::QSceneChangePtr &changee) final { Q_UNUSED(changee); return false; }

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

    ObserverSpy()
        : Qt3DCore::QAbstractArbiter()
        , m_postman(new SimplePostman(this))
    {
    }

    ~ObserverSpy();

    void sceneChangeEventWithLock(const Qt3DCore::QSceneChangePtr &e) override
    {
        events << ChangeRecord(e, true);
    }

    void sceneChangeEventWithLock(const Qt3DCore::QSceneChangeList &e) override
    {
        for (size_t i = 0, m = e.size(); i < m; ++i) {
            events << ChangeRecord(e.at(i), false);
        }
    }

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) override
    {
        events << ChangeRecord(e, false);
    }

    Qt3DCore::QAbstractPostman *postman() const final
    {
        return m_postman.data();
    }

    void addDirtyFrontEndNode(Qt3DCore::QNode *node) final {
        if (!dirtyNodes.contains(node))
            dirtyNodes << node;
    }

    void addDirtyFrontEndNode(Qt3DCore::QNode *node, Qt3DCore::QNode *subNode, const char *property, Qt3DCore::ChangeFlag change) final {
        if (!dirtyNodes.contains(node))
            dirtyNodes << node;
        dirtySubNodes.push_back({node, subNode, change, property});
    }

    void removeDirtyFrontEndNode(Qt3DCore::QNode *node) final {
        dirtyNodes.removeOne(node);
    }

    QVector<Qt3DCore::QNode *> dirtyNodes;
    QVector<Qt3DCore::NodeRelationshipChange> dirtySubNodes;
    QList<ChangeRecord> events;
    QScopedPointer<SimplePostman> m_postman;
};

ObserverSpy::~ObserverSpy()
{
}

void SimplePostman::notifyBackend(const Qt3DCore::QSceneChangePtr &change)
{
    m_spy->sceneChangeEventWithLock(change);
}



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

    void setArbiterAndScene(Qt3DCore::QAbstractArbiter *arbiter,
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

            d->updateNode(attribute, "attribute", Qt3DCore::PropertyValueAdded);
        }
    }

    void removeAttribute(Qt3DCore::QNode *attribute)
    {
        Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(this);
        d->updateNode(attribute, "attribute", Qt3DCore::PropertyValueRemoved);

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
        if (std::find(std::begin(m_attributes), std::end(m_attributes), attribute) == std::end(m_attributes)) {
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

    void setArbiterAndScene(Qt3DCore::QAbstractArbiter *arbiter,
                            Qt3DCore::QScene *scene = nullptr)
    {
        Q_ASSERT(arbiter);
        if (scene)
            scene->setArbiter(arbiter);
        Qt3DCore::QNodePrivate::get(this)->setScene(scene);
        Qt3DCore::QNodePrivate::get(this)->setArbiter(arbiter);
    }

    void setArbiterAndEngine(Qt3DCore::QAbstractArbiter *arbiter,
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

            d->updateNode(attribute, "attribute", Qt3DCore::PropertyValueRemoved);
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
    QVector<MyQNode *> m_attributes;
};

class MyQComponent : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit MyQComponent(Qt3DCore::QNode *parent = nullptr) : QComponent(parent)
    {}
    void setArbiter(Qt3DCore::QAbstractArbiter *arbiter)
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

    void setArbiter(Qt3DCore::QAbstractArbiter *arbiter)
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

    QVector<Qt3DCore::QNodeId> filteredEvents(ChangeType change) const
    {
        QVector<Qt3DCore::QNodeId> result;
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

    mutable QVector<Event> events;
    mutable QHash<Qt3DCore::QNodeId, Qt3DCore::QNode *> allNodes;

private:
    Q_DECLARE_PRIVATE(TestAspect)
    explicit TestAspect(TestAspectPrivate &dd, QObject *parent);
};

class TestFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    TestFunctor(TestAspect *aspect) : m_aspect(aspect) {}

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override
    {
        auto node = new Qt3DCore::QBackendNode;
        m_Nodes.insert(change->subjectId(), node);
        m_aspect->addEvent(change->subjectId(), TestAspect::Creation);
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
    registerBackendType<Qt3DCore::QEntity, true>(QSharedPointer<TestFunctor>::create(this));
    registerBackendType<MyQEntity, true>(QSharedPointer<TestFunctor>::create(this));
    registerBackendType<MyQNode, true>(QSharedPointer<TestFunctor>::create(this));
    registerBackendType<Qt3DCore::QNode, true>(QSharedPointer<TestFunctor>::create(this));
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

void tst_Nodes::initTestCase()
{
    qRegisterMetaType<Qt3DCore::QNode::PropertyTrackingMode>("PropertyTrackingMode");
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
    ObserverSpy spy;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&spy);

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
    ObserverSpy spy;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&spy);

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
    ObserverSpy spy;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&spy);

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
    Qt3DCore::QScene scene;
    ObserverSpy spy;
    QScopedPointer<MyQNode> node(new MyQNode());
    // WHEN
    node->setArbiterAndScene(&spy, &scene);
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
    QCOMPARE(spy.events.size(), 1); // Child Added
    QCOMPARE(node->children().count(), 1);
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);

    // Node Added event
    QVERIFY(spy.events.first().wasLocked());
    Qt3DCore::QPropertyNodeAddedChangePtr additionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
    QVERIFY(additionEvent);
    QCOMPARE(additionEvent->subjectId(), node->id());
    QCOMPARE(additionEvent->addedNodeId(), child->id());
    QCOMPARE(additionEvent->metaObject(), child->metaObject());
}

void tst_Nodes::appendSingleChildNodeToNodeSceneImplicitParenting()
{
    // Check nodes added when scene is set
    // GIVEN
    Qt3DCore::QScene scene;
    ObserverSpy spy;
    QScopedPointer<MyQNode> node(new MyQNode());

    // WHEN
    node->setArbiterAndScene(&spy, &scene);
    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(node.data())->scene() != nullptr);

    // WHEN
    QScopedPointer<MyQNode> child(new MyQNode(node.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == node.data());
    QVERIFY(child->parentNode() == node.data());
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);

    QCOMPARE(spy.events.size(), 1);
    QVERIFY(spy.events.first().wasLocked());
    QCOMPARE(node->children().count(), 1);

    // Node Added event
    QVERIFY(spy.events.first().wasLocked());
    Qt3DCore::QPropertyNodeAddedChangePtr additionEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
    QVERIFY(additionEvent);
    QCOMPARE(additionEvent->subjectId(), node->id());
    QCOMPARE(additionEvent->addedNodeId(), child->id());
    QCOMPARE(additionEvent->metaObject(), child->metaObject());
}

void tst_Nodes::appendMultipleChildNodesToNodeScene()
{
    // Check nodes added when scene is set

    // GIVEN
    Qt3DCore::QScene scene;
    ObserverSpy spy;
    QScopedPointer<MyQNode> node(new MyQNode());

    // WHEN
    node->setArbiterAndScene(&spy, &scene);
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
    QCOMPARE(node->children().count(), 10);

    // WHEN
    QCoreApplication::processEvents();

    // THEN backend is notified after the event loop spins. The recorded events are a little
    // tricky to understand and differs for children with the parent being set at construction
    // time (even children and ids) and the children being created without a parent and then
    // explicitly calling setParent() after (odd children and ids).
    //
    // Even children:
    //   child constructed
    //   notifications to backend scheduled via the event loop as object is not yet fully constructed
    //
    // Odd children:
    //   child constructed
    //   parent set
    //   notifications to backend sent immediately as object is fully constructed
    //
    // With this in mind, the recorded events should show:
    //
    // for each odd child:
    //   child addition to parent of odd child
    //
    // followed by:
    //
    // for each even child:
    //   child addition to parent of even child
    //
    const auto expectedEventCount = childCount;
    QCOMPARE(spy.events.size(), 10);

    for (auto i = 0; i < expectedEventCount; ++i) {
        const auto childIndex = i;
        Qt3DCore::QNode *child = node->childNodes().at(childIndex);

        const auto recordIndex = (i % 2 == 0) ? expectedEventCount / 2 + i / 2 : i / 2;
        const auto additionRecord = spy.events.at(recordIndex);
        Qt3DCore::QPropertyNodeAddedChangePtr additionEvent = additionRecord.change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
        QCOMPARE(additionEvent->subjectId(), node->id());
        QCOMPARE(additionEvent->addedNodeId(), child->id());
        QCOMPARE(additionEvent->metaObject(), child->metaObject());
    }
}

void tst_Nodes::checkParentChangeToNull()
{
    // GIVEN
    Qt3DCore::QScene scene;
    ObserverSpy spy;
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    root->setArbiterAndScene(&spy, &scene);
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == root.data());
    QCOMPARE(spy.events.size(), 1);
    QCOMPARE(root->children().size(), 1);

    // WHEN
    spy.events.clear();
    child->setParent(Q_NODE_NULLPTR);

    // THEN
    QVERIFY(child->parent() == nullptr);
    QCOMPARE(root->children().size(), 0);
    QCOMPARE(spy.events.size(), 0);
}

void tst_Nodes::checkParentChangeToOtherParent()
{
    // GIVEN
    Qt3DCore::QScene scene;
    ObserverSpy spy;
    QScopedPointer<MyQNode> root(new MyQNode());
    root->setArbiterAndScene(&spy, &scene);
    QScopedPointer<MyQNode> parent1(new MyQNode(root.data()));
    QScopedPointer<MyQNode> parent2(new MyQNode(root.data()));
    QCoreApplication::processEvents();

    // THEN
    QCOMPARE(spy.events.size(), 2); // 2 x (1 node added to children change)

    // WHEN
    spy.events.clear();
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode(parent1.data()));
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(child->parent() == parent1.data());
    QCOMPARE(parent1->children().size(), 1);
    QCOMPARE(parent2->children().size(), 0);
    QVERIFY(Qt3DCore::QNodePrivate::get(child.data())->scene() != nullptr);
    QCOMPARE(spy.events.size(), 1); // 1 node added to children change

    // WHEN
    spy.events.clear();
    child->setParent(parent2.data());

    // THEN
    QVERIFY(child->parent() == parent2.data());
    QCOMPARE(parent1->children().size(), 0);
    QCOMPARE(parent2->children().size(), 1);
    QCOMPARE(spy.events.size(), 1);

    // CHECK event 1 is a Node Removed event
    // this no longer generated because nodes don't have backends yet

    QVERIFY(!Qt3DCore::QNodePrivate::get(child.data())->m_hasBackendNode);


//    QVERIFY(spy.events.first().wasLocked());
//    const Qt3DCore::QPropertyNodeRemovedChangePtr event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeRemovedChange>();
//    QCOMPARE(event->type(), Qt3DCore::PropertyValueRemoved);
//    QCOMPARE(event->subjectId(), parent1->id());
//    QCOMPARE(event->removedNodeId(), child->id());
//    QCOMPARE(event->metaObject(), child->metaObject());

    // CHECK event 2 is a Node Added event
    QVERIFY(spy.events.last().wasLocked());
    const Qt3DCore::QPropertyNodeAddedChangePtr event2 = spy.events.last().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
    QCOMPARE(event2->type(), Qt3DCore::PropertyValueAdded);
    QCOMPARE(event2->subjectId(), parent2->id());
    QCOMPARE(event2->addedNodeId(), child->id());
    QCOMPARE(event2->metaObject(), child->metaObject());
}

void tst_Nodes::checkParentChangeFromExistingBackendParentToNewlyCreatedParent()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    engine.setRunMode(Qt3DCore::QAspectEngine::Manual);
    QScopedPointer<MyQEntity> root(new MyQEntity());
    root->setArbiterAndEngine(&spy, &engine);
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);

    MyQNode *child(new MyQNode(root.data()));
    MyQNode *child2(new MyQNode(root.data()));

    QCoreApplication::processEvents();
    engine.processFrame();

    // Due to the way we create root, it has a backend
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->m_hasBackendNode == true);
    QCOMPARE(aspect->events.count(), 2);
    QCOMPARE(aspect->events[0].type, TestAspect::Creation);
    QCOMPARE(aspect->events[0].nodeId, child->id());
    QCOMPARE(aspect->events[1].type, TestAspect::Creation);
    QCOMPARE(aspect->events[1].nodeId, child2->id());

    // THEN
    QCOMPARE(spy.events.size(), 2); // 2 x (1 child added to parent change)

    // WHEN -> Reparenting child with backend node to new parent with no backend yet
    aspect->clearNodes();
    spy.events.clear();
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
    QCOMPARE(spy.events.size(), 2);
    // 1 node removed change
    // 1 node added to children change
    QVERIFY(Qt3DCore::QNodePrivate::get(newParent.data())->m_hasBackendNode == true);
    QVERIFY(Qt3DCore::QNodePrivate::get(child)->m_hasBackendNode == true);

    {
        // CHECK event 1 is a Node Removed event
        QVERIFY(spy.events.first().wasLocked());
        const Qt3DCore::QPropertyNodeRemovedChangePtr event = spy.events.takeFirst().change().staticCast<Qt3DCore::QPropertyNodeRemovedChange>();
        QCOMPARE(event->type(), Qt3DCore::PropertyValueRemoved);
        QCOMPARE(event->subjectId(), root->id());
        QCOMPARE(event->removedNodeId(), child->id());
        QCOMPARE(event->metaObject(), child->metaObject());

        const Qt3DCore::QPropertyNodeAddedChangePtr event2 = spy.events.takeFirst().change().staticCast<Qt3DCore::QPropertyNodeAddedChange>();
        QCOMPARE(event2->type(), Qt3DCore::PropertyValueAdded);
        QCOMPARE(event2->addedNodeId(), newParent->id());
        QCOMPARE(event2->metaObject(), newParent->metaObject());
        QCOMPARE(event2->subjectId(), root->id());

        QCOMPARE(aspect->events.count(), 3);

        // child backend is destroyed because it was reparented to node without backend (newParent)
        QCOMPARE(aspect->events[0].type, TestAspect::Destruction);
        QCOMPARE(aspect->events[0].nodeId, child->id());

        // newParent and child both get backends created
        QCOMPARE(aspect->events[1].type, TestAspect::Creation);
        QCOMPARE(aspect->events[1].nodeId, newParent->id());
        QCOMPARE(aspect->events[2].type, TestAspect::Creation);
        QCOMPARE(aspect->events[2].nodeId, child->id());
    }

    // WHEN -> Changing parent to node with existing backend
    aspect->clearNodes();
    child->setParent(child2);

    // THEN
    QCOMPARE(spy.events.size(), 2);

    // 1 node removed change, 1 node added change
    {
        QVERIFY(spy.events.first().wasLocked());
        const Qt3DCore::QPropertyNodeRemovedChangePtr event = spy.events.takeFirst().change().staticCast<Qt3DCore::QPropertyNodeRemovedChange>();
        QCOMPARE(event->type(), Qt3DCore::PropertyValueRemoved);
        QCOMPARE(event->subjectId(), newParent->id());
        QCOMPARE(event->removedNodeId(), child->id());
        QCOMPARE(event->metaObject(), child->metaObject());

        const Qt3DCore::QPropertyNodeAddedChangePtr event2 = spy.events.takeFirst().change().staticCast<Qt3DCore::QPropertyNodeAddedChange>();
        QCOMPARE(event2->type(), Qt3DCore::PropertyValueAdded);
        QCOMPARE(event2->addedNodeId(), child->id());
        QCOMPARE(event2->metaObject(), child->metaObject());
        QCOMPARE(event2->subjectId(), child2->id());
    }

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
    QCOMPARE(spy.events.size(), 2);
    // 1 node removed change
    // 1 node added to children change
    {
        // CHECK event 1 is a Node Removed event
        QVERIFY(spy.events.first().wasLocked());
        const Qt3DCore::QPropertyNodeRemovedChangePtr event = spy.events.takeFirst().change().staticCast<Qt3DCore::QPropertyNodeRemovedChange>();
        QCOMPARE(event->type(), Qt3DCore::PropertyValueRemoved);
        QCOMPARE(event->subjectId(), child2->id());
        QCOMPARE(event->removedNodeId(), child->id());
        QCOMPARE(event->metaObject(), child->metaObject());

        const Qt3DCore::QPropertyNodeAddedChangePtr event2 = spy.events.takeFirst().change().staticCast<Qt3DCore::QPropertyNodeAddedChange>();
        QCOMPARE(event2->type(), Qt3DCore::PropertyValueAdded);
        QCOMPARE(event2->addedNodeId(), newParent2->id());
        QCOMPARE(event2->metaObject(), newParent2->metaObject());
        QCOMPARE(event2->subjectId(), root->id());

        // child backend is destroyed because it was reparented to node without backend (newParent)
        QCOMPARE(aspect->events[0].type, TestAspect::Destruction);
        QCOMPARE(aspect->events[0].nodeId, child->id());
    }
}

//Test creation changes happen for an entire subtree at once, starting at the top
// so that parents are always created before their children. Even if the front-end
// nodes are constructed in a different order.
void tst_Nodes::checkBackendNodesCreatedFromTopDown()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);
    QScopedPointer<MyQEntity> root(new MyQEntity());

    root->setArbiterAndEngine(&spy, &engine);
    QScopedPointer<Qt3DCore::QNode> parentWithBackend(new MyQNode(root.data()));

    // create parent backend node
    QCoreApplication::processEvents();
    QVERIFY(Qt3DCore::QNodePrivate::get(parentWithBackend.get())->m_hasBackendNode);

    // WHEN -> creating 3 nodes and setting one as a property on the other
    //         child2 is set as property on child1, but is created AFTER child1
    spy.events.clear();
    MyQNode *dummyParent(new MyQNode(parentWithBackend.data()));
    MyQNode *child1(new MyQNode(parentWithBackend.data()));
    MyQNode *child2(new MyQNode(dummyParent));
    child1->setNodeProperty(child2);

    // THEN - we should have no events because the new nodes have no backend yet
    QCOMPARE(spy.events.count(), 0);

    // WHEN - create the backend nodes
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(dummyParent->parent() == parentWithBackend.data());
    QVERIFY(child1->parent() == parentWithBackend.data());
    QVERIFY(child2->parent() == dummyParent);

    // THEN
    QCOMPARE(spy.events.size(), 2);
    // 1 node added to children change (dummyParent to parent)
    // 1 node added to children change (child1 to parent)

    {
        QVERIFY(Qt3DCore::QNodePrivate::get(dummyParent)->m_hasBackendNode);
        QVERIFY(Qt3DCore::QNodePrivate::get(child1)->m_hasBackendNode);
        QVERIFY(Qt3DCore::QNodePrivate::get(child2)->m_hasBackendNode);

        // 1st event: dummyParent added to parent
        const auto event = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
        QCOMPARE(event->type(), Qt3DCore::PropertyValueAdded);
        QCOMPARE(event->addedNodeId(), dummyParent->id());
        QCOMPARE(event->subjectId(), parentWithBackend->id());

        // 2nd event: child 1 added to parent
        const auto event2 = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
        QCOMPARE(event2->type(), Qt3DCore::PropertyValueAdded);
        QCOMPARE(event2->addedNodeId(), child1->id());
        QCOMPARE(event2->subjectId(), parentWithBackend->id());

        verifyChildrenCreatedBeforeParents(root.get(), aspect);
    }

}

void tst_Nodes::checkBackendNodesCreatedFromTopDownWithReparenting()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);
    QScopedPointer<MyQEntity> root(new MyQEntity());

    root->setArbiterAndEngine(&spy, &engine);
    QScopedPointer<Qt3DCore::QNode> parentWithBackend(new MyQNode(root.data()));

    // create parent backend node
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(Qt3DCore::QNodePrivate::get(parentWithBackend.get())->m_hasBackendNode);

    // WHEN -> creating a node with a parent with backend, then reparenting it to another
    // parent with backend created after it.
    spy.events.clear();
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
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());
    QScopedPointer<Qt3DCore::QNode> child(new MyQNode());

    // WHEN
    root->setArbiterAndEngine(&spy, &engine);
    root->setSimulateBackendCreated(true);
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

    QCOMPARE(spy.events.size(), 1);

    QVERIFY(spy.events.first().wasLocked());
    const Qt3DCore::QPropertyNodeRemovedChangePtr removalEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeRemovedChange>();
    QCOMPARE(removalEvent->subjectId(), root->id());
    QCOMPARE(removalEvent->removedNodeId(), child->id());
    QCOMPARE(removalEvent->metaObject(), child->metaObject());
}

void tst_Nodes::checkAllBackendCreationDoneInSingleFrame()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    engine.setRunMode(Qt3DCore::QAspectEngine::Manual);
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);

    QScopedPointer<MyQEntity> root(new MyQEntity());
    root->setArbiterAndEngine(&spy, &engine);

    QCoreApplication::processEvents();

    // THEN
    // Due to the way we create root, it has a backend
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->m_hasBackendNode == true);
    QCOMPARE(aspect->events.count(), 1);
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
    QCOMPARE(aspect->events.count(), 2);
    QCOMPARE(aspect->events[0].type, TestAspect::Creation);
    QCOMPARE(aspect->events[0].nodeId, child2->id());
    QCOMPARE(aspect->events[1].type, TestAspect::Creation);
    QCOMPARE(aspect->events[1].nodeId, child1->id());
}

void tst_Nodes::removingMultipleChildNodesFromNode()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&spy, &engine);

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
    QCOMPARE(spy.events.size(), 10);

    // WHEN
    spy.events.clear();
    qDeleteAll(root->children());

    // THEN
    QVERIFY(root->children().count() == 0);
    QCOMPARE(spy.events.size(), 10);
    int i = 0;
    for (const ObserverSpy::ChangeRecord &r : qAsConst(spy.events)) {
        QVERIFY(r.wasLocked());
        const Qt3DCore::QNodeId childId = childIds.at(i);
        Qt3DCore::QPropertyNodeRemovedChangePtr additionEvent = r.change().dynamicCast<Qt3DCore::QPropertyNodeRemovedChange>();
        QCOMPARE(additionEvent->subjectId(), root->id());
        QCOMPARE(additionEvent->removedNodeId(), childId);
        QCOMPARE(additionEvent->metaObject(), &MyQNode::staticMetaObject);
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

void tst_Nodes::checkConstructionSetParentMix()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    auto aspect = new TestAspect;
    engine.registerAspect(aspect);
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&spy, &engine);
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
    QCOMPARE(root->children().count(), 1);
    QCOMPARE(subTreeRoot->children().count(), 100);
    QCOMPARE(spy.events.size(), 1); // 1 child added (subTree to root)

    // Ensure first event is subTreeRoot
    verifyChildrenCreatedBeforeParents(root.data(), aspect);

    const Qt3DCore::QPropertyNodeAddedChangePtr lastEvent = spy.events.takeLast().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
    QVERIFY(!lastEvent.isNull());
    QCOMPARE(lastEvent->subjectId(), root->id());
    QCOMPARE(lastEvent->propertyName(), "children");
    QCOMPARE(lastEvent->addedNodeId(), subTreeRoot->id());
}

void tst_Nodes::checkParentingQEntityToQNode()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&spy, &engine);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    auto subTreeRoot = new Qt3DCore::QEntity(root.data());
    auto childEntity = new Qt3DCore::QEntity(subTreeRoot);
    auto childNode = new Qt3DCore::QNode(subTreeRoot);

    // THEN
    QCoreApplication::processEvents();
    QCOMPARE(spy.events.size(), 1);

    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(subTreeRoot)->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(childEntity)->m_hasBackendNode);
    QVERIFY(Qt3DCore::QNodePrivate::get(childNode)->m_hasBackendNode);

    // WHEN we reparent the childEntity to the childNode (QNode)
    spy.events.clear();
    childEntity->setParent(childNode);
    QCoreApplication::processEvents();

    // THEN we should get
    // - one child removed change for childEntity->subTreeRoot,
    // - one child added change for childEntity->childNode,
    QCOMPARE(spy.events.size(), 2);

    const auto removedEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeRemovedChange>();
    QVERIFY(!removedEvent.isNull());
    QCOMPARE(removedEvent->subjectId(), subTreeRoot->id());

    const auto addedEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
    QVERIFY(!addedEvent.isNull());
    QCOMPARE(addedEvent->subjectId(), childNode->id());

    // The arbiter's dirtyNodes should contain the childEntity and
    //  - the dirty node's parent should be childNode
    //  - the dirty node's parent entity should be the subTreeRoot
    QCOMPARE(spy.dirtyNodes.size(), 1);
    const auto dirtyEntity = qobject_cast<Qt3DCore::QEntity*>(spy.dirtyNodes.takeFirst());
    QVERIFY(dirtyEntity);
    QCOMPARE(dirtyEntity, childEntity);
    QCOMPARE(dirtyEntity->parent(), childNode);
    QCOMPARE(dirtyEntity->parentEntity(), subTreeRoot);
}

void tst_Nodes::checkConstructionWithParent()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&spy, &engine);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN we create a child and then set it as a Node* property
    auto *node = new MyQNode(root.data());
    root->setNodeProperty(node);

    // THEN we should get one child added change
    QCoreApplication::processEvents();
    QCOMPARE(root->children().count(), 1);
    QCOMPARE(spy.events.size(), 1); // 1 child added change

    const auto newChildEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
    QVERIFY(!newChildEvent.isNull());
    QCOMPARE(newChildEvent->subjectId(), root->id());
    QCOMPARE(newChildEvent->propertyName(), "children");
    QCOMPARE(newChildEvent->addedNodeId(), node->id());

    // Ensure the parent node is dirty
    QCOMPARE(spy.events.size(), 0);
    QCOMPARE(spy.dirtyNodes.size(), 1);
    QCOMPARE(spy.dirtyNodes.front(), root.data());
}

void tst_Nodes::checkConstructionWithNonRootParent()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&spy, &engine);
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
    QCOMPARE(root->children().count(), 1);
    QCOMPARE(parent->children().count(), 1);

    QCOMPARE(spy.events.size(), 1); // 1 child added changes

    const auto parentNewChildEvent = spy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
    QVERIFY(!parentNewChildEvent.isNull());
    QCOMPARE(parentNewChildEvent->subjectId(), root->id());
    QCOMPARE(parentNewChildEvent->propertyName(), "children");
    QCOMPARE(parentNewChildEvent->addedNodeId(), parent->id());

    // Ensure second and last event is property set change
    QCOMPARE(spy.dirtyNodes.size(), 1);
    QCOMPARE(spy.dirtyNodes.front(), root.data());
}

void tst_Nodes::checkConstructionAsListElement()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    QScopedPointer<MyQEntity> root(new MyQEntity());

    // WHEN
    root->setArbiterAndEngine(&spy, &engine);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN we create a child and then set it as a Node* property
    auto *node = new MyQNode(root.data());
    root->addAttribute(node);

    // THEN we should get one creation change, one child added change
    // and one property change event, in that order.
    QCoreApplication::processEvents();

    QCOMPARE(root->children().count(), 1);
    QCOMPARE(spy.dirtyNodes.size(), 1); // 1 property change
    QCOMPARE(spy.dirtySubNodes.size(), 1); // 1 child added change
}

void tst_Nodes::checkSceneIsSetOnConstructionWithParent()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QScene scene;
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    root->setArbiterAndScene(&spy, &scene);
    root->setSimulateBackendCreated(true);

    // THEN
    QVERIFY(Qt3DCore::QNodePrivate::get(root.data())->scene() != nullptr);

    // WHEN
    Qt3DCore::QEntity *subTreeRoot = new Qt3DCore::QEntity(root.data());

    QVector<Qt3DCore::QEntity *> children;
    QVector<Qt3DCore::QComponent *> cmps;
    children.reserve(5);
    cmps.reserve(5);
    for (int i = 0; i < 5; ++i) {
        const auto cmp = new MyQComponent(subTreeRoot);
        cmps << cmp;
        children << new Qt3DCore::QEntity(cmp);

        // When cmp is full created, it will also send the creation change for its child entity
    }
    QCoreApplication::processEvents();
    QCOMPARE(root->children().count(), 1);
    QCOMPARE(subTreeRoot->children().count(), 5);
    QCOMPARE(spy.events.size(), 1); // 1 child added (subTree to root)

    spy.events.clear();

    // Add component in a separate loop to ensure components are added after
    // entities have been fully created
    for (int i = 0; i < 5; ++i) {
        children.at(i)->addComponent(cmps.at(i));
    }

    // THEN
    QCOMPARE(spy.events.size(), 0);
    QCOMPARE(spy.dirtySubNodes.size(), 5); // 5 entities changed
}

void tst_Nodes::checkSubNodePostConstructIsCalledWhenReferincingNodeProperty()
{
    // GIVEN
    ObserverSpy spy;
    Qt3DCore::QAspectEngine engine;
    Qt3DCore::QScene scene(&engine);
    QScopedPointer<MyQNode> root(new MyQNode());

    // WHEN
    root->setArbiterAndScene(&spy, &scene);
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

    // WHEN
    MyQNode *fakeRenderState = new MyQNode(material);
    Qt3DCore::QNodePrivate *dPtr = Qt3DCore::QNodePrivate::get(fakeRenderState);

    // THEN
    QVERIFY(!dPtr->m_hasBackendNode);

    // WHEN
    material->m_renderPass->addAttribute(fakeRenderState);

    // THEN
    QVERIFY(dPtr->m_hasBackendNode);
}

void tst_Nodes::appendingParentlessComponentToEntityWithoutScene()
{
    // GIVEN
    ObserverSpy entitySpy;
    ObserverSpy componentSpy;
    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndScene(&entitySpy);
        entity->setSimulateBackendCreated(true);

        MyQComponent *comp = new MyQComponent();
        comp->setArbiter(&componentSpy);

        // THEN
        QVERIFY(entity->parentNode() == nullptr);
        QVERIFY(entity->children().count() == 0);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == nullptr);

        // WHEN
        entity->addComponent(comp);

        // THEN
        QVERIFY(entity->components().count() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());
        QCOMPARE(entitySpy.events.size(), 0);
        QCOMPARE(entitySpy.dirtyNodes.size(), 1);
        QCOMPARE(entitySpy.dirtySubNodes.size(), 1);

        const auto event = entitySpy.dirtySubNodes.first();
        QCOMPARE(event.change, Qt3DCore::ComponentAdded);
        QCOMPARE(event.node, entity.data());
        QCOMPARE(event.subNode, comp);
        QCOMPARE(event.property, nullptr);

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
    ObserverSpy eventSpy;
    Qt3DCore::QAspectEngine engine;

    {
        QScopedPointer<MyQEntity> root(new MyQEntity());
        root->setArbiterAndEngine(&eventSpy, &engine);
        root->setSimulateBackendCreated(true);

        QCoreApplication::processEvents();

        QScopedPointer<MyQEntity> entity(new MyQEntity(root.data()));
        MyQComponent *comp = new MyQComponent();

        // THEN
        QVERIFY(root->parentNode() == nullptr);
        QVERIFY(root->children().count() == 1);
        QVERIFY(root->components().empty());
        QVERIFY(entity->parentNode() == root.data());
        QVERIFY(entity->children().count() == 0);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == nullptr);

        // WHEN
        entity->addComponent(comp);
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(entity->components().count() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());

        QCOMPARE(eventSpy.events.size(), 1);
        // - entity added as child to root
        QVERIFY(eventSpy.events.first().wasLocked());

        QVERIFY(Qt3DCore::QNodePrivate::get(entity.data())->m_hasBackendNode);
        QVERIFY(Qt3DCore::QNodePrivate::get(comp)->m_hasBackendNode);

        {
            const auto event = eventSpy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
            QVERIFY(!event.isNull());
            QCOMPARE(event->type(), Qt3DCore::PropertyValueAdded);
            QCOMPARE(event->subjectId(), root->id());
            QCOMPARE(event->propertyName(), QByteArrayLiteral("children"));
            QCOMPARE(event->addedNodeId(), entity->id());
        }
    }
}

void tst_Nodes::appendingParentlessComponentToEntityWithScene()
{
    // GIVEN
    ObserverSpy eventSpy;
    Qt3DCore::QAspectEngine engine;

    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndEngine(&eventSpy, &engine);
        entity->setSimulateBackendCreated(true);

        QCoreApplication::processEvents();

        MyQComponent *comp = new MyQComponent();

        // THEN
        QVERIFY(entity->parentNode() == nullptr);
        QVERIFY(entity->children().count() == 0);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == nullptr);

        // WHEN
        entity->addComponent(comp);
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(entity->components().count() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());

        QCOMPARE(eventSpy.events.size(), 1);
        // - child added
        QVERIFY(eventSpy.events.first().wasLocked());

        {
            const auto event = eventSpy.events.takeFirst().change().dynamicCast<Qt3DCore::QPropertyNodeAddedChange>();
            QVERIFY(!event.isNull());
            QCOMPARE(event->type(), Qt3DCore::PropertyValueAdded);
            QCOMPARE(event->subjectId(), entity->id());
            QCOMPARE(event->propertyName(), QByteArrayLiteral("children"));
            QCOMPARE(event->addedNodeId(), comp->id());
        }
    }
}


void tst_Nodes::appendingComponentToEntity()
{
    // GIVEN
    ObserverSpy entitySpy;
    ObserverSpy componentSpy;
    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndScene(&entitySpy);
        MyQComponent *comp = new MyQComponent(entity.data());
        comp->setArbiter(&componentSpy);
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(entity->parentNode() == nullptr);
        QVERIFY(entity->children().count() == 1);
        QVERIFY(entity->components().empty());
        QVERIFY(comp->parentNode() == entity.data());

        // WHEN
        entity->addComponent(comp);

        // THEN
        QVERIFY(entity->components().count() == 1);
        QVERIFY(entity->components().first() == comp);
        QVERIFY(comp->parentNode() == entity.data());
        QCOMPARE(entitySpy.events.size(), 0);
        QCOMPARE(entitySpy.dirtyNodes.size(), 1);
        QCOMPARE(entitySpy.dirtySubNodes.size(), 1);
        QCOMPARE(entitySpy.dirtyNodes.first(), entity.data());
        const auto event = entitySpy.dirtySubNodes.takeFirst();
        QCOMPARE(event.node, entity.data());
        QCOMPARE(event.subNode, comp);
        QCOMPARE(event.change, Qt3DCore::ComponentAdded);
        QCOMPARE(event.property, nullptr);
    }
}

void tst_Nodes::removingComponentFromEntity()
{
    // GIVEN
    ObserverSpy entitySpy;
    ObserverSpy componentSpy;
    {
        QScopedPointer<MyQEntity> entity(new MyQEntity());
        entity->setArbiterAndScene(&entitySpy);
        MyQComponent *comp = new MyQComponent();
        comp->setArbiter(&componentSpy);

        // WHEN
        entity->addComponent(comp);

        // THEN
        QVERIFY(entity->components().count() == 1);
        QCOMPARE(entity->children().count(), 1);
        QVERIFY(comp->parent() == entity.data());

        // WHEN
        entitySpy.events.clear();
        entitySpy.dirtyNodes.clear();
        entitySpy.dirtySubNodes.clear();
        componentSpy.events.clear();
        entity->removeComponent(comp);

        // THEN
        QVERIFY(entity->components().count() == 0);
        QVERIFY(comp->parent() == entity.data());
        QVERIFY(entity->children().count() == 1);
        QCOMPARE(entitySpy.events.size(), 0);
        QCOMPARE(entitySpy.dirtyNodes.size(), 1);
        QCOMPARE(entitySpy.dirtySubNodes.size(), 1);
        QCOMPARE(componentSpy.events.size(), 0);
        {
            const auto event = entitySpy.dirtySubNodes.takeFirst();
            qDebug() << event.change;
            QCOMPARE(event.change, Qt3DCore::ComponentRemoved);
            QCOMPARE(event.node, entity.data());
            QCOMPARE(event.subNode, comp);
            QCOMPARE(event.property, nullptr);
        }
    }
}

void tst_Nodes::changeCustomProperty()
{
    // GIVEN
    ObserverSpy spy;
    QScopedPointer<MyQNode> node(new MyQNode());
    node->setArbiterAndScene(&spy);
    // WHEN
    node->setCustomProperty(QStringLiteral("foo"));
    // THEN
    QCOMPARE(spy.dirtyNodes.size(), 1);
    QCOMPARE(spy.dirtyNodes.front(), node.data());
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
    QCOMPARE(node.defaultPropertyTrackingMode(), Qt3DCore::QNode::TrackFinalValues);
}

void tst_Nodes::checkPropertyChanges()
{
    // GIVEN
    Qt3DCore::QNode parentNode;
    Qt3DCore::QNode node;

    {
        // WHEN
        QSignalSpy spy(&node, SIGNAL(parentChanged(QObject *)));
        Qt3DCore::QNode *newValue = &parentNode;
        node.setParent(newValue);

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(node.parentNode(), newValue);
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        node.setParent(newValue);

        // THEN
        QCOMPARE(node.parentNode(), newValue);
        QCOMPARE(spy.count(), 0);
    }
    {
        // WHEN
        QSignalSpy spy(&node, SIGNAL(enabledChanged(bool)));
        const bool newValue = false;
        node.setEnabled(newValue);

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(node.isEnabled(), newValue);
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        node.setEnabled(newValue);

        // THEN
        QCOMPARE(node.isEnabled(), newValue);
        QCOMPARE(spy.count(), 0);
    }
    {
        // WHEN
        QSignalSpy spy(&node, SIGNAL(defaultPropertyTrackingModeChanged(PropertyTrackingMode)));
        const Qt3DCore::QNode::PropertyTrackingMode newValue = Qt3DCore::QNode::TrackAllValues;
        node.setDefaultPropertyTrackingMode(newValue);

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(node.defaultPropertyTrackingMode(), newValue);
        QCOMPARE(spy.count(), 1);

        // WHEN
        spy.clear();
        node.setDefaultPropertyTrackingMode(newValue);

        // THEN
        QCOMPARE(node.defaultPropertyTrackingMode(), newValue);
        QCOMPARE(spy.count(), 0);
    }
    {
        // WHEN
        const QString enabledPropertyName = QStringLiteral("enabled");
        node.setDefaultPropertyTrackingMode(Qt3DCore::QNode::DontTrackValues);
        node.setPropertyTracking(enabledPropertyName, Qt3DCore::QNode::TrackAllValues);

        // THEN
        QCOMPARE(node.propertyTracking(enabledPropertyName), Qt3DCore::QNode::TrackAllValues);

        // WHEN
        node.clearPropertyTracking(enabledPropertyName);

        // THEN
        QCOMPARE(node.propertyTracking(enabledPropertyName), Qt3DCore::QNode::DontTrackValues);
    }
}

void tst_Nodes::checkCreationData()
{
    // GIVEN
    Qt3DCore::QNode root;
    Qt3DCore::QNode node;

    node.setParent(&root);
    node.setEnabled(true);
    const QString enabledPropertyName = QStringLiteral("enabled");
    node.setDefaultPropertyTrackingMode(Qt3DCore::QNode::DontTrackValues);
    node.setPropertyTracking(enabledPropertyName, Qt3DCore::QNode::TrackAllValues);

    // WHEN
    QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

    {
        Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&node);
        creationChanges = creationChangeGenerator.creationChanges();
    }

    // THEN
    {
        QCOMPARE(creationChanges.size(), 1);

        const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChangeBase>(creationChanges.first());

        QCOMPARE(node.id(), creationChangeData->subjectId());
        QCOMPARE(node.isEnabled(), true);
        QCOMPARE(node.isEnabled(), creationChangeData->isNodeEnabled());
        QCOMPARE(node.metaObject(), creationChangeData->metaObject());
    }

    // WHEN
    node.setEnabled(false);

    {
        Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&node);
        creationChanges = creationChangeGenerator.creationChanges();
    }

    // THEN
    {
        QCOMPARE(creationChanges.size(), 1);

        const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChangeBase>(creationChanges.first());

        QCOMPARE(node.id(), creationChangeData->subjectId());
        QCOMPARE(node.isEnabled(), false);
        QCOMPARE(node.isEnabled(), creationChangeData->isNodeEnabled());
        QCOMPARE(node.metaObject(), creationChangeData->metaObject());
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
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 1);
        QCOMPARE(arbiter.dirtyNodes.front(), &node);

        arbiter.dirtyNodes.clear();
    }

    {
        // WHEN
        node.setEnabled(false);

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 0);
    }

}

void tst_Nodes::checkPropertyTrackModeUpdate()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QNode node;
    arbiter.setArbiterOnNode(&node);

    {
        // WHEN
        node.setDefaultPropertyTrackingMode(Qt3DCore::QNode::TrackAllValues);
        QCoreApplication::processEvents();

        // THEN -> this properties is non notifying
        QCOMPARE(arbiter.events.size(), 0);
    }

    {
        // WHEN
        node.setDefaultPropertyTrackingMode(Qt3DCore::QNode::TrackAllValues);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
    }

}

void tst_Nodes::checkTrackedPropertyNamesUpdate()
{
    // GIVEN
    TestArbiter arbiter;
    Qt3DCore::QNode node;
    arbiter.setArbiterOnNode(&node);

    {
        // WHEN
        node.setPropertyTracking(QStringLiteral("883"), Qt3DCore::QNode::TrackAllValues);
        QCoreApplication::processEvents();

        // THEN -> this properties is non notifying
        QCOMPARE(arbiter.events.size(), 0);
    }

    {
        // WHEN
        node.setPropertyTracking(QStringLiteral("883"), Qt3DCore::QNode::DontTrackValues);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
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
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 1);
        QCOMPARE(arbiter.dirtyNodes.front(), node.data());

        // WHEN
        // scene should be unset and node removed from arbiter dirtyList
        node.reset();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 0);
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
        QCOMPARE(arbiter.events.size(), 1); // childAdded
        QCOMPARE(arbiter.dirtyNodes.size(), 1);
        QCOMPARE(arbiter.dirtyNodes.front(), child);

        // WHEN
        // scene should be unset and child node removed from arbiter dirtyList
        node.reset();

        // THEN
        QCOMPARE(arbiter.events.size(), 1); // childRemoved (no destroyed change since we had no backend)
        QCOMPARE(arbiter.dirtyNodes.size(), 0);
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
