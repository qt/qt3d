// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/qcomponent.h>
#include <QtCore/qscopedpointer.h>

using namespace Qt3DCore;

class tst_Entity : public QObject
{
    Q_OBJECT
public:
    tst_Entity() : QObject()
    {
        qRegisterMetaType<Qt3DCore::QNode*>();
    }
    ~tst_Entity() {}

private slots:
    void constructionDestruction();

    void addComponentSingleParentSingleAggregation();
    void addComponentSingleParentSeveralAggregations();
    void addComponentsSeveralParentsSingleAggregations();
    void addComponentsSeveralParentsSeveralAggregations();

    void retrieveSingleComponent();

    void removeComponentSingleParentSingleAggregation();
    void removeComponentSingleParentSeveralAggregations();
    void removeComponentsSeveralParentsSingleAggreation();
    void removeComponentsSeveralParentsSeveralAggregations();

    void addSeveralTimesSameComponent();
    void removeSeveralTimesSameComponent();

    void checkComponentBookkeeping();
};

class MyQComponent : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit MyQComponent(Qt3DCore::QNode *parent = nullptr)
        : QComponent(parent)
    {}
};

class MyQ2Component : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit MyQ2Component(Qt3DCore::QNode *parent = 0)
        : QComponent(parent)
    {}
};

class MyEntity : public Qt3DCore::QEntity
{
public:
    explicit MyEntity(Qt3DCore::QNode *parent = nullptr)
        : QEntity(parent)
    {}
};

void tst_Entity::constructionDestruction()
{
    // GIVEN
    QEntity *entity = nullptr;
    // WHEN
    entity = new QEntity;
    // THEN
    QVERIFY(entity != nullptr);

    delete entity;

    // GIVEN
    QScopedPointer<QEntity> entity2(new QEntity);
    // WHEN
    entity2.reset(nullptr);
    // THEN
    // this should not crash
}

void tst_Entity::addComponentSingleParentSingleAggregation()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new QEntity());
    MyQComponent *comp = new MyQComponent(entity.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 0);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 0);

    // WHEN
    entity->addComponent(comp);

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 1);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 1);
}

void tst_Entity::addComponentSingleParentSeveralAggregations()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3DCore::QEntity> entity2(new QEntity());

    MyQComponent *comp1 = new MyQComponent(entity1.data());
    MyQComponent *comp2 = new MyQComponent(entity1.data());
    MyQComponent *comp3 = new MyQComponent(entity1.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity1.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 0);

    QCOMPARE(entity1->children().size(), 3);
    QCOMPARE(entity2->children().size(), 0);

    QCOMPARE(comp1->entities().size(), 0);
    QCOMPARE(comp2->entities().size(), 0);
    QCOMPARE(comp3->entities().size(), 0);

    // WHEN
    entity1->addComponent(comp1);
    entity1->addComponent(comp2);
    entity1->addComponent(comp3);

    entity2->addComponent(comp1);
    entity2->addComponent(comp2);
    entity2->addComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity1.data());

    QCOMPARE(entity1->components().size(), 3);
    QCOMPARE(entity2->components().size(), 3);

    QCOMPARE(entity1->children().size(), 3);
    QCOMPARE(entity2->children().size(), 0);

    QCOMPARE(comp1->entities().size(), 2);
    QCOMPARE(comp2->entities().size(), 2);
    QCOMPARE(comp3->entities().size(), 2);
}

void tst_Entity::addComponentsSeveralParentsSingleAggregations()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3DCore::QEntity> entity2(new QEntity());

    MyQComponent *comp1 = new MyQComponent(entity1.data());
    MyQComponent *comp2 = new MyQComponent(entity1.data());
    MyQComponent *comp3 = new MyQComponent(entity2.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 0);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 0);
    QCOMPARE(comp2->entities().size(), 0);
    QCOMPARE(comp3->entities().size(), 0);

    // WHEN
    entity1->addComponent(comp1);
    entity1->addComponent(comp2);

    entity2->addComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 2);
    QCOMPARE(entity2->components().size(), 1);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 1);
    QCOMPARE(comp2->entities().size(), 1);
    QCOMPARE(comp3->entities().size(), 1);
}

void tst_Entity::addComponentsSeveralParentsSeveralAggregations()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3DCore::QEntity> entity2(new QEntity());

    MyQComponent *comp1 = new MyQComponent(entity1.data());
    MyQComponent *comp2 = new MyQComponent(entity1.data());
    MyQComponent *comp3 = new MyQComponent(entity2.data());
    QCoreApplication::processEvents();

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 0);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 0);
    QCOMPARE(comp2->entities().size(), 0);
    QCOMPARE(comp3->entities().size(), 0);

    // WHEN
    entity1->addComponent(comp1);
    entity1->addComponent(comp2);
    entity1->addComponent(comp3);

    entity2->addComponent(comp1);
    entity2->addComponent(comp2);
    entity2->addComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 3);
    QCOMPARE(entity2->components().size(), 3);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 2);
    QCOMPARE(comp2->entities().size(), 2);
    QCOMPARE(comp3->entities().size(), 2);
}

void tst_Entity::removeComponentSingleParentSingleAggregation()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new QEntity());
    MyQComponent *comp = new MyQComponent(entity.data());
    QCoreApplication::processEvents();
    entity->addComponent(comp);

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 1);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 1);

    // WHEN
    entity->removeComponent(comp);

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 0);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 0);
}

void tst_Entity::removeComponentSingleParentSeveralAggregations()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3DCore::QEntity> entity2(new QEntity());

    MyQComponent *comp1 = new MyQComponent(entity1.data());
    MyQComponent *comp2 = new MyQComponent(entity1.data());
    MyQComponent *comp3 = new MyQComponent(entity1.data());
    QCoreApplication::processEvents();

    entity1->addComponent(comp1);
    entity1->addComponent(comp2);
    entity1->addComponent(comp3);

    entity2->addComponent(comp1);
    entity2->addComponent(comp2);
    entity2->addComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity1.data());

    QCOMPARE(entity1->components().size(), 3);
    QCOMPARE(entity2->components().size(), 3);

    QCOMPARE(entity1->children().size(), 3);
    QCOMPARE(entity2->children().size(), 0);

    QCOMPARE(comp1->entities().size(), 2);
    QCOMPARE(comp2->entities().size(), 2);
    QCOMPARE(comp3->entities().size(), 2);

    // WHEN
    entity1->removeComponent(comp1);
    entity1->removeComponent(comp2);
    entity1->removeComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity1.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 3);

    QCOMPARE(entity1->children().size(), 3);
    QCOMPARE(entity2->children().size(), 0);

    QCOMPARE(comp1->entities().size(), 1);
    QCOMPARE(comp2->entities().size(), 1);
    QCOMPARE(comp3->entities().size(), 1);

    // WHEN
    entity2->removeComponent(comp1);
    entity2->removeComponent(comp2);
    entity2->removeComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity1.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 0);

    QCOMPARE(entity1->children().size(), 3);
    QCOMPARE(entity2->children().size(), 0);

    QCOMPARE(comp1->entities().size(), 0);
    QCOMPARE(comp2->entities().size(), 0);
    QCOMPARE(comp3->entities().size(), 0);
}

void tst_Entity::removeComponentsSeveralParentsSingleAggreation()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3DCore::QEntity> entity2(new QEntity());

    MyQComponent *comp1 = new MyQComponent(entity1.data());
    MyQComponent *comp2 = new MyQComponent(entity1.data());
    MyQComponent *comp3 = new MyQComponent(entity2.data());
    QCoreApplication::processEvents();

    // WHEN
    entity1->addComponent(comp1);
    entity1->addComponent(comp2);
    entity2->addComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 2);
    QCOMPARE(entity2->components().size(), 1);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 1);
    QCOMPARE(comp2->entities().size(), 1);
    QCOMPARE(comp3->entities().size(), 1);

    // WHEN
    entity1->removeComponent(comp1);
    entity1->removeComponent(comp2);
    entity2->removeComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 0);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 0);
    QCOMPARE(comp2->entities().size(), 0);
    QCOMPARE(comp3->entities().size(), 0);
}

void tst_Entity::removeComponentsSeveralParentsSeveralAggregations()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3DCore::QEntity> entity2(new QEntity());

    MyQComponent *comp1 = new MyQComponent(entity1.data());
    MyQComponent *comp2 = new MyQComponent(entity1.data());
    MyQComponent *comp3 = new MyQComponent(entity2.data());
    QCoreApplication::processEvents();

    // WHEN
    entity1->addComponent(comp1);
    entity1->addComponent(comp2);
    entity1->addComponent(comp3);

    entity2->addComponent(comp1);
    entity2->addComponent(comp2);
    entity2->addComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 3);
    QCOMPARE(entity2->components().size(), 3);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 2);
    QCOMPARE(comp2->entities().size(), 2);
    QCOMPARE(comp3->entities().size(), 2);

    // WHEN
    entity1->removeComponent(comp1);
    entity1->removeComponent(comp2);
    entity1->removeComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 3);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 1);
    QCOMPARE(comp2->entities().size(), 1);
    QCOMPARE(comp3->entities().size(), 1);

    // WHEN
    entity2->removeComponent(comp1);
    entity2->removeComponent(comp2);
    entity2->removeComponent(comp3);

    // THEN
    QVERIFY(comp1->parent() == entity1.data());
    QVERIFY(comp2->parent() == entity1.data());
    QVERIFY(comp3->parent() == entity2.data());

    QCOMPARE(entity1->components().size(), 0);
    QCOMPARE(entity2->components().size(), 0);

    QCOMPARE(entity1->children().size(), 2);
    QCOMPARE(entity2->children().size(), 1);

    QCOMPARE(comp1->entities().size(), 0);
    QCOMPARE(comp2->entities().size(), 0);
    QCOMPARE(comp3->entities().size(), 0);
}

void tst_Entity::retrieveSingleComponent()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity1(new QEntity());

    MyQComponent *comp1 = new MyQComponent(entity1.data());
    MyQComponent *comp2 = new MyQComponent(entity1.data());
    QCoreApplication::processEvents();
    entity1->addComponent(comp1);
    entity1->addComponent(comp2);

    // WHEN
    const QList<MyQComponent *> myQComponentsInEntity = entity1->componentsOfType<MyQComponent>();
    const QList<MyQ2Component *> myQ2ComponentsInEntity = entity1->componentsOfType<MyQ2Component>();

    // THEN
    QVERIFY(myQComponentsInEntity.size() == 2);
    QVERIFY(myQComponentsInEntity[0] == comp1);
    QVERIFY(myQComponentsInEntity[1] == comp2);

    QVERIFY(myQ2ComponentsInEntity.size() == 0);
}

void tst_Entity::addSeveralTimesSameComponent()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new QEntity());
    MyQComponent *comp = new MyQComponent(entity.data());
    QCoreApplication::processEvents();
    entity->addComponent(comp);

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 1);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 1);

    // WHEN
    entity->addComponent(comp);

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 1);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 1);
}

void tst_Entity::removeSeveralTimesSameComponent()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> entity(new QEntity());
    MyQComponent *comp = new MyQComponent(entity.data());
    QCoreApplication::processEvents();
    entity->addComponent(comp);
    entity->removeComponent(comp);

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 0);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 0);

    // WHEN
    entity->removeComponent(comp);

    // THEN
    QVERIFY(comp->parent() == entity.data());
    QCOMPARE(entity->components().size(), 0);
    QCOMPARE(entity->children().size(), 1);
    QCOMPARE(comp->entities().size(), 0);
}

void tst_Entity::checkComponentBookkeeping()
{
    // GIVEN
    QScopedPointer<Qt3DCore::QEntity> rootEntity(new Qt3DCore::QEntity);
    {
        // WHEN
        QScopedPointer<Qt3DCore::QComponent> comp(new MyQComponent(rootEntity.data()));
        rootEntity->addComponent(comp.data());

        // THEN
        QCOMPARE(comp->parent(), rootEntity.data());
        QCOMPARE(rootEntity->components().size(), 1);
    }
    // THEN (Should not crash and comp should be automatically removed)
    QVERIFY(rootEntity->components().empty());

    {
        // WHEN
        QScopedPointer<Qt3DCore::QEntity> someOtherEntity(new Qt3DCore::QEntity);
        QScopedPointer<Qt3DCore::QComponent> comp(new MyQComponent(someOtherEntity.data()));
        rootEntity->addComponent(comp.data());

        // THEN
        QCOMPARE(comp->parent(), someOtherEntity.data());
        QCOMPARE(rootEntity->components().size(), 1);

        // WHEN
        int sigCount = 0;
        QObject *sigSender = comp.data();
        connect(comp.data(), &QComponent::removedFromEntity, [&sigCount, sigSender](QEntity *) {
            QComponent *c = qobject_cast<QComponent *>(sigSender);
            if (sigSender && c)
                sigCount++; // test the sender is still a QComponent when signal is emitted
        });

        comp.reset();
        rootEntity.reset();

        // THEN (Should not crash when the comp is destroyed (tests for failed removal of destruction helper)
        QCOMPARE(sigCount, 1);
    }
}

Qt3DCore::QNodeId parentEntityId(Qt3DCore::QEntity *entity)
{
    Qt3DCore::QEntityPrivate *d = static_cast<Qt3DCore::QEntityPrivate*>(Qt3DCore::QNodePrivate::get(entity));
    return d->parentEntityId();
}

QTEST_MAIN(tst_Entity)

#include "tst_qentity.moc"
