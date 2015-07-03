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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

class tst_Cloning : public QObject
{
    Q_OBJECT
public:
    tst_Cloning() : QObject() {}
    ~tst_Cloning() {}

private slots:
    void checkEntityCloning();
};

class MyQNode : public Qt3D::QNode
{
    Q_OBJECT
public:
    explicit MyQNode(Qt3D::QNode *parent = 0) : QNode(parent)
    {}

    ~MyQNode()
    {
        QNode::cleanup();
    }

    void setCustomProperty(const QString &s) { m_customProperty = s; }
    QString customProperty() const { return m_customProperty; }

    QT3D_CLONEABLE(MyQNode)

    QString m_customProperty;

    static QNode *clone(QNode *node) {
        return QNode::clone(node);
    }

protected:
    void copy(const Qt3D::QNode *ref) Q_DECL_OVERRIDE
    {
        Qt3D::QNode::copy(ref);
        const MyQNode *refNode = qobject_cast<const MyQNode *>(ref);
        setCustomProperty(refNode->customProperty());
    }
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

    QT3D_CLONEABLE(MyQComponent)
};

void tst_Cloning::checkEntityCloning()
{
    // GIVEN
    Qt3D::QScene *scene = new Qt3D::QScene();
    MyQNode *root = new MyQNode();
    Qt3D::QNodePrivate::get(root)->setScene(scene);

    Qt3D::QEntity *entity = new Qt3D::QEntity(root);

    // WHEN
    MyQComponent *comp1 = new MyQComponent();
    MyQComponent *comp2 = new MyQComponent();
    MyQComponent *comp3 = new MyQComponent();

    MyQNode *childNode = new MyQNode(entity);
    entity->addComponent(comp1);
    entity->addComponent(comp2);
    entity->addComponent(comp3);

    root->setCustomProperty(QStringLiteral("Corvette"));

    // THEN
    QVERIFY(root->customProperty() == QStringLiteral("Corvette"));
    QCOMPARE(root->children().count(), 1);
    QCOMPARE(entity->children().count(), 4);
    QCOMPARE(entity->components().count(), 3);

    //WHEN
    MyQNode *cloneRoot = qobject_cast<MyQNode *>(MyQNode::clone(root));

    // THEN
    QCOMPARE(cloneRoot->children().count(), 1);
    QCOMPARE(cloneRoot->id(), root->id());
    QVERIFY(cloneRoot->customProperty() == root->customProperty());

    Qt3D::QEntity *cloneEntity = qobject_cast<Qt3D::QEntity *>(cloneRoot->children().first());
    QVERIFY(cloneEntity != Q_NULLPTR);
    QCOMPARE(cloneEntity->id(), entity->id());
    QCOMPARE(cloneEntity->children().count(), 4);
    QCOMPARE(cloneEntity->components().count(), 3);

    QList<Qt3D::QNodeId> ids = QList<Qt3D::QNodeId>() << comp1->id() << comp2->id() << comp3->id() << childNode->id();

    Q_FOREACH (QObject *c, cloneEntity->children()) {
        Qt3D::QNode *n = qobject_cast<Qt3D::QNode *>(c);
        QVERIFY(ids.contains(n->id()));
        ids.removeAll(n->id());
    }

    delete cloneRoot;
}

QTEST_APPLESS_MAIN(tst_Cloning)

#include "tst_cloning.moc"

