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
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>
#include <QtCore/qscopedpointer.h>

using namespace Qt3D;

class tst_Entity : public QObject
{
    Q_OBJECT
public:
    tst_Entity() : QObject() {}
    ~tst_Entity() {}

private slots:
    void constructionDestruction();

    void addComponentSingleParentSingleAggregation();
    void addComponentSingleParentSeveralAggregations();
    void addComponentsSeveralParentsSingleAggregations();
    void addComponentsSeveralParentsSeveralAggregations();

    void removeComponentSingleParentSingleAggregation();
    void removeComponentSingleParentSeveralAggregations();
    void removeComponentsSeveralParentsSingleAggreation();
    void removeComponentsSeveralParentsSeveralAggregations();

    void addSeveralTimesSameComponent();
    void removeSeveralTimesSameComponent();
    void verifyCopy();
};

class MyQComponent : public Qt3D::QComponent
{
    Q_OBJECT
public:
    explicit MyQComponent(Qt3D::QNode *parent = 0)
        : QComponent(parent)
    {}

    ~MyQComponent()
    {
        QNode::cleanup();
    }

protected:
    QT3D_CLONEABLE(MyQComponent)
};


class MyEntity : public Qt3D::QEntity
{
public:
    explicit MyEntity(Qt3D::QNode *parent = 0)
        : QEntity(parent)
    {}

    void makeCopyOf(Qt3D::QEntity *other)
    {
        QEntity::copy(other);
    }
};

void tst_Entity::constructionDestruction()
{
    // GIVEN
    QEntity *entity = Q_NULLPTR;
    // WHEN
    entity = new QEntity;
    // THEN
    QVERIFY(entity != Q_NULLPTR);

    delete entity;

    // GIVEN
    QScopedPointer<QEntity> entity2(new QEntity);
    // WHEN
    entity2.reset(Q_NULLPTR);
    // THEN
    // this should not crash
}

void tst_Entity::addComponentSingleParentSingleAggregation()
{
    // GIVEN
    QScopedPointer<Qt3D::QEntity> entity(new QEntity());
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
    QScopedPointer<Qt3D::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3D::QEntity> entity2(new QEntity());

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
    QScopedPointer<Qt3D::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3D::QEntity> entity2(new QEntity());

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
    QScopedPointer<Qt3D::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3D::QEntity> entity2(new QEntity());

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
    QScopedPointer<Qt3D::QEntity> entity(new QEntity());
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
    QScopedPointer<Qt3D::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3D::QEntity> entity2(new QEntity());

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
    QScopedPointer<Qt3D::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3D::QEntity> entity2(new QEntity());

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
    QScopedPointer<Qt3D::QEntity> entity1(new QEntity());
    QScopedPointer<Qt3D::QEntity> entity2(new QEntity());

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

void tst_Entity::addSeveralTimesSameComponent()
{
    // GIVEN
    QScopedPointer<Qt3D::QEntity> entity(new QEntity());
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
    QScopedPointer<Qt3D::QEntity> entity(new QEntity());
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

void tst_Entity::verifyCopy()
{
    // GIVEN
    QScopedPointer<Qt3D::QEntity> root(new Qt3D::QEntity());
    MyEntity *parentLessEntity = new MyEntity();
    MyEntity *parentedEntity = new MyEntity(root.data());

    QCoreApplication::processEvents();

    // THEN
    QVERIFY(root->id() != parentLessEntity->id());
    QVERIFY(root->id() != parentedEntity->id());
    QVERIFY(root->parentEntityId().isNull());
    QVERIFY(!parentedEntity->parentEntityId().isNull());
    QVERIFY(parentLessEntity->parentEntityId().isNull());

    // WHEN
    MyEntity *parentedEntityCopy = new MyEntity();
    parentedEntityCopy->makeCopyOf(parentedEntity);

    // THEN
    QVERIFY(parentedEntityCopy->id() == parentedEntity->id());
    QVERIFY(parentedEntityCopy->parentEntityId() == parentedEntity->parentEntityId());

    // WHEN
    MyEntity *parentLessEntityCopy = new MyEntity();
    parentLessEntityCopy->makeCopyOf(parentLessEntity);

    // THEN
    QVERIFY(parentLessEntityCopy->id() == parentLessEntity->id());
    QVERIFY(parentLessEntityCopy->parentEntityId() == parentLessEntity->parentEntityId());
}


QTEST_MAIN(tst_Entity)

#include "tst_qentity.moc"
