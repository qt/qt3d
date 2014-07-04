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
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcomponent.h>

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
};


void tst_Nodes::defaultNodeConstruction()
{
    Qt3D::QNode *node = new Qt3D::QNode();
    Qt3D::QNode node2(node);

    QVERIFY(node != Q_NULLPTR);
    QVERIFY(!node->children().isEmpty());
    QVERIFY(node2.children().isEmpty());
    QVERIFY(node2.parent() == node);
}

void tst_Nodes::defaultComponentConstruction()
{
    Qt3D::QComponent *comp = new Qt3D::QComponent();
    Qt3D::QComponent comp2(comp);

    QVERIFY(comp != Q_NULLPTR);
    QVERIFY(comp2.parent() == comp);
}

void tst_Nodes::defaultEntityConstrution()
{
    Qt3D::QEntity *entity = new Qt3D::QEntity();
    Qt3D::QEntity entity2(entity);

    QVERIFY(entity->components().isEmpty());
    QVERIFY(entity2.components().isEmpty());
    QVERIFY(entity2.parent() == entity);
}

void tst_Nodes::appendChildNodesToNode()
{
    Qt3D::QNode *node = new Qt3D::QNode();

    for (int i = 0; i < 10; i++) {
        Qt3D::QNode *child = new Qt3D::QNode();
        QVERIFY(child->parent() == Q_NULLPTR);
        node->addChild(child);
        QVERIFY(child->parent() == node);
        QVERIFY(child->parentNode() == node);
    }
    QVERIFY(node->children().count() == 10);
    for (int i = 0; i < 10; i++) {
        Qt3D::QNode *child = new Qt3D::QNode(node);
        QVERIFY(child->parent() == node);
        QVERIFY(child->parentNode() == node);
    }
    QVERIFY(node->children().count() == 20);
    Qt3D::QNode *child = node->children().first();
    Qt3D::QNode *parent = node;
    for (int i = 0; i < 10; i++) {
        QVERIFY(child->parent() == parent);
        QVERIFY(child->parentNode() == parent);
        child->addChild(new Qt3D::QNode());
        parent = child;
        child = child->children().first();
    }
    QVERIFY(node->children().count() == 20);
    child = node->children().first();
    parent = node;
    for (int i = 0; i < 10; i++) {
        QVERIFY(child->parent() == parent);
        QVERIFY(child->parentNode() == parent);
        QVERIFY(child->children().count() == 1);
        parent = child;
        child = child->children().first();
    }
    QVERIFY(child->children().count() == 0);
}

void tst_Nodes::removingChildNodesFromNode()
{
    Qt3D::QNode *root = new Qt3D::QNode();
    Qt3D::QNode *child = new Qt3D::QNode();

    root->addChild(child);
    QVERIFY(root->children().count() == 1);
    root->removeChild(child);
    QVERIFY(root->children().count() == 0);
    QVERIFY(child->parent() == Q_NULLPTR);

    for (int i = 0; i < 10; i++) {
        root->addChild(new Qt3D::QNode());
    }
    QVERIFY(root->children().count() == 10);
    root->removeAllChildren();
    QVERIFY(root->children().count() == 0);

    Qt3D::QNode *firstChild = child;
    for (int i = 0; i < 10; i++) {
        child->addChild(new Qt3D::QNode());
        child = child->children().first();
    }
    QVERIFY(root->children().count() == 0);
    root->addChild(firstChild);
    QVERIFY(root->children().count() == 1);

    Qt3D::QNode *parent = child->parentNode();
    for (int i = 0; i < 10; i++) {
        QVERIFY(parent->children().count() == 1);
        QVERIFY(child->parentNode() == parent);
        parent->removeChild(child);
        QVERIFY(child->parent() == Q_NULLPTR);
        QVERIFY(parent->children().count() == 0);
        child = parent;
        parent = parent->parentNode();
    }
    QVERIFY(parent == root);
    QVERIFY(parent->children().first() == firstChild);
    QVERIFY(firstChild->children().isEmpty());
}

void tst_Nodes::appendingChildEntitiesToNode()
{
    Qt3D::QNode *root = new Qt3D::QNode();

    Qt3D::QEntity *childEntity = new Qt3D::QEntity(root);

    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == Q_NULLPTR);
    QVERIFY(childEntity->parentNode() == root);
}

void tst_Nodes::removingChildEntitiesFromNode()
{
    Qt3D::QNode *root = new Qt3D::QNode();

    Qt3D::QEntity *childEntity = new Qt3D::QEntity(root);

    QVERIFY(root->children().first() == childEntity);
    QVERIFY(childEntity->parentEntity() == Q_NULLPTR);
    QVERIFY(childEntity->parentNode() == root);

    root->removeChild(childEntity);

    QVERIFY(root->children().isEmpty());
    QVERIFY(childEntity->parentNode() == Q_NULLPTR);
    QVERIFY(childEntity->parent() == Q_NULLPTR);
}

void tst_Nodes::appendingComponentsToEntity()
{
    Qt3D::QNode *root = new Qt3D::QNode();

    Qt3D::QEntity *entity = new Qt3D::QEntity(root);

    Qt3D::QComponent *comp1 = new Qt3D::QComponent(root);
    Qt3D::QComponent *comp2 = new Qt3D::QComponent(entity);
    Qt3D::QComponent *comp3 = new Qt3D::QComponent();

    QVERIFY(entity->parentNode() == root);
    QVERIFY(root->children().count() == 2);
    QVERIFY(entity->children().count() == 1);
    QVERIFY(entity->components().empty());
    QVERIFY(comp3->parentNode() == Q_NULLPTR);

    entity->addComponent(comp1);
    QVERIFY(entity->components().count() == 1);
    QVERIFY(entity->components().first() == comp1);
    QVERIFY(comp1->parentNode() == root);
    QVERIFY(comp2->parentNode() == entity);
    QVERIFY(comp3->parentNode() == Q_NULLPTR);

    entity->addComponent(comp2);
    QVERIFY(entity->components().count() == 2);
    QVERIFY(entity->components().first() == comp1);
    QVERIFY(entity->components().last() == comp2);
    QVERIFY(comp1->parentNode() == root);
    QVERIFY(comp2->parentNode() == entity);
    QVERIFY(comp3->parentNode() == Q_NULLPTR);

    entity->addComponent(comp3);
    QVERIFY(entity->components().count() == 3);
    QVERIFY(entity->components().first() == comp1);
    QVERIFY(entity->components().last() == comp3);
    QVERIFY(comp1->parentNode() == root);
    QVERIFY(comp2->parentNode() == entity);
    QVERIFY(comp3->parentNode() == entity);
    QCOMPARE(entity->children().count(), 2);
}

void tst_Nodes::removingComponentsFromEntity()
{
    Qt3D::QNode *root = new Qt3D::QNode();
    Qt3D::QEntity *entity = new Qt3D::QEntity(root);

    Qt3D::QComponent *comp1 = new Qt3D::QComponent(root);
    Qt3D::QComponent *comp2 = new Qt3D::QComponent(entity);
    Qt3D::QComponent *comp3 = new Qt3D::QComponent();


    entity->addComponent(comp1);
    entity->addComponent(comp2);
    entity->addComponent(comp3);
    QVERIFY(entity->components().count() == 3);
    QCOMPARE(entity->children().count(), 2);

    entity->removeComponent(comp2);
    QVERIFY(entity->components().count() == 2);
    QVERIFY(comp2->parent() == entity);
    QVERIFY(entity->children().count() == 2);

    entity->removeComponent(comp1);
    QVERIFY(entity->components().count() == 1);
    QVERIFY(comp1->parentNode() == root);

    entity->removeComponent(comp3);
    QVERIFY(entity->components().count() == 0);
    QVERIFY(comp3->parentNode() == entity);

    entity->addComponent(comp1);
    entity->addComponent(comp2);
    entity->addComponent(comp3);
    QVERIFY(entity->components().count() == 3);
    QCOMPARE(entity->children().count(), 2);

    entity->removeAllChildren();
    QCOMPARE(entity->components().count(), 3);
    QCOMPARE(entity->children().count(), 0);
}

QTEST_APPLESS_MAIN(tst_Nodes)

#include "tst_nodes.moc"
