/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#include <QtGui/QOpenGLContext>

#include "qglscenenode.h"
#include "qglpainter.h"
#include "qglabstracteffect.h"
#include "qglpicknode.h"
#include "qgraphicstransform3d.h"
#include "qgraphicsscale3d.h"
#include "qgraphicsrotation3d.h"
#include "qglbuilder.h"

#include "qtest_helpers.h"

class tst_QGLSceneNode : public QObject
{
    Q_OBJECT
public:
    tst_QGLSceneNode() {}
    ~tst_QGLSceneNode() {}

private slots:
    void defaultValues();
    void modify();
    void addNode();
    void removeNode();
    void clone();
    void boundingBox_data();
    void boundingBox();
    void position_QTBUG_17279();
    void findSceneNode();
};

// Check that all properties have their expected defaults.
void tst_QGLSceneNode::defaultValues()
{
    QGLSceneNode node;
    QVERIFY(node.options() == QGLSceneNode::NoOptions);
    QVERIFY(node.geometry().isNull());
    QVERIFY(node.boundingBox().isNull());
    QVERIFY(node.localTransform().isIdentity());

    QVERIFY(node.position() == QVector3D(0, 0, 0));
    QCOMPARE(node.x(), 0.0f);
    QCOMPARE(node.y(), 0.0f);
    QCOMPARE(node.z(), 0.0f);

    QVERIFY(node.transforms().isEmpty());

    QVERIFY(node.drawingMode() == QGL::Triangles);
    QVERIFY(node.effect() == QGL::FlatColor);
    QVERIFY(node.userEffect() == 0);
    QVERIFY(!node.hasEffect());

    QCOMPARE(node.start(), 0);
    QCOMPARE(node.count(), 0);

    QCOMPARE(node.materialIndex(), -1);
    QCOMPARE(node.backMaterialIndex(), -1);

    QVERIFY(node.material() == 0);
    QVERIFY(node.backMaterial() == 0);
    QVERIFY(node.palette() == 0);
    QVERIFY(node.pickNode() == 0);

    QVERIFY(node.allChildren().isEmpty());
    QVERIFY(node.children().isEmpty());
}

class TestEffect : public QGLAbstractEffect
{
public:
    void setActive(QGLPainter *, bool) {}
    void update(QGLPainter *, QGLPainter::Updates) {}
};

// Modify properties and check that the modifications propagate.
void tst_QGLSceneNode::modify()
{
    QGLSceneNode node;

    QSignalSpy updatedSpy(&node, SIGNAL(updated()));

    node.setOptions(QGLSceneNode::ViewNormals);
    QCOMPARE(updatedSpy.count(), 1);
    QVERIFY(node.options() == QGLSceneNode::ViewNormals);

    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);

    node.setLocalTransform(m);
    QCOMPARE(updatedSpy.count(), 2);
    QVERIFY(node.localTransform() == m);

    node.setPosition(QVector3D(1, -2, 3));
    QCOMPARE(updatedSpy.count(), 3);
    QVERIFY(node.position() == QVector3D(1, -2, 3));

    node.setX(-45.0f);
    QCOMPARE(updatedSpy.count(), 4);
    QVERIFY(node.position() == QVector3D(-45.0f, -2, 3));

    node.setY(67.0f);
    QCOMPARE(updatedSpy.count(), 5);
    QVERIFY(node.position() == QVector3D(-45.0f, 67.0f, 3));

    node.setZ(-23.5f);
    QCOMPARE(updatedSpy.count(), 6);
    QVERIFY(node.position() == QVector3D(-45.0f, 67.0f, -23.5f));

    QList<QQuickQGraphicsTransform3D *> transforms;
    transforms.append(new QGraphicsScale3D(this));
    transforms.append(new QGraphicsRotation3D(this));
    node.setTransforms(transforms);
    QCOMPARE(updatedSpy.count(), 7);
    QVERIFY(node.transforms() == transforms);

    QGraphicsScale3D *scale = new QGraphicsScale3D(this);
    transforms.append(scale);
    node.addTransform(scale);
    QCOMPARE(updatedSpy.count(), 8);
    QVERIFY(node.transforms() == transforms);

    node.setDrawingMode(QGL::Points);
    QCOMPARE(updatedSpy.count(), 9);
    QVERIFY(node.drawingMode() == QGL::Points);

    node.setEffect(QGL::LitMaterial);
    QCOMPARE(updatedSpy.count(), 10);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(node.hasEffect());

    node.setEffectEnabled(false);
    QCOMPARE(updatedSpy.count(), 11);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(!node.hasEffect());

    TestEffect userEffect;
    node.setUserEffect(&userEffect);
    QCOMPARE(updatedSpy.count(), 12);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(node.userEffect() == &userEffect);
    QVERIFY(node.hasEffect());

    node.setMaterialIndex(3);
    QCOMPARE(updatedSpy.count(), 13);
    QCOMPARE(node.materialIndex(), 3);
    QVERIFY(node.material() == 0);  // material index is out of range

    node.setBackMaterialIndex(5);
    QCOMPARE(updatedSpy.count(), 14);
    QCOMPARE(node.backMaterialIndex(), 5);
    QVERIFY(node.backMaterial() == 0);  // material index is out of range

    QVERIFY(node.palette() == 0);

    QGLMaterial *mat1 = new QGLMaterial();
    node.setMaterial(mat1);
    QCOMPARE(updatedSpy.count(), 15);
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 5);
    QVERIFY(node.material() == mat1);
    QVERIFY(node.backMaterial() == 0);
    QVERIFY(node.palette() != 0);

    QGLMaterial *mat2 = new QGLMaterial();
    node.setBackMaterial(mat2);
    QCOMPARE(updatedSpy.count(), 16);
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.material() == mat1);
    QVERIFY(node.backMaterial() == mat2);

    QGLMaterial *mat3 = new QGLMaterial();
    node.setMaterial(mat3);
    QCOMPARE(updatedSpy.count(), 17);
    QCOMPARE(node.materialIndex(), 2);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.material() == mat3);
    QVERIFY(node.backMaterial() == mat2);

    node.setMaterial(mat1);
    QCOMPARE(updatedSpy.count(), 18);
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.material() == mat1);
    QVERIFY(node.backMaterial() == mat2);

    QVERIFY(node.palette()->material(0) == mat1);
    QVERIFY(node.palette()->material(1) == mat2);
    QVERIFY(node.palette()->material(2) == mat3);
    QCOMPARE(node.palette()->size(), 3);

    QGLPickNode pick;
    QVERIFY(pick.target() == 0);
    QCOMPARE(pick.id(), -1);
    node.setPickNode(&pick);
    QVERIFY(node.pickNode() == &pick);
    QVERIFY(pick.target() == &node);
    QCOMPARE(pick.id(), -1);
    pick.setId(4);
    QCOMPARE(pick.id(), 4);

    // One last check to make sure there are no leaks between properties.
    // e.g. the position actually changing the rotation, etc.
    QVERIFY(node.options() == QGLSceneNode::ViewNormals);
    QVERIFY(node.localTransform() == m);
    QVERIFY(node.position() == QVector3D(-45.0f, 67.0f, -23.5f));
    QVERIFY(node.transforms() == transforms);
    QVERIFY(node.drawingMode() == QGL::Points);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(node.userEffect() == &userEffect);
    QVERIFY(node.hasEffect());
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.pickNode() == &pick);
}

// Add nodes to build a scene graph.
void tst_QGLSceneNode::addNode()
{
    QGLSceneNode *node1 = new QGLSceneNode();

    // Add a node via the constructor.
    QGLSceneNode *node2 = new QGLSceneNode(node1);
    QVERIFY(node2->parent() == node1);
    QCOMPARE(node1->children().count(), 1);
    QVERIFY(node1->children()[0] == node2);

    // Add a node explicitly, not parented into the graph yet.
    // Because node3's parent is null, it will be parented in.
    QGLSceneNode *node3 = new QGLSceneNode();
    node1->addNode(node3);
    QVERIFY(node3->parent() == node1);
    QCOMPARE(node1->children().count(), 2);
    QVERIFY(node1->children()[0] == node2);
    QVERIFY(node1->children()[1] == node3);

    // Add a node explicily that is parented elsewhere.  Because
    // node4's parent is not null, it will *not* be parented in.
    QGLSceneNode *node4 = new QGLSceneNode(this);
    node1->addNode(node4);
    QVERIFY(node4->parent() == this);
    QCOMPARE(node1->children().count(), 3);
    QVERIFY(node1->children()[0] == node2);
    QVERIFY(node1->children()[1] == node3);
    QVERIFY(node1->children()[2] == node4);

    // Try adding node4 again (should do nothing).
    node1->addNode(node4);
    QVERIFY(node4->parent() == this);
    QCOMPARE(node1->children().count(), 3);
    QVERIFY(node1->children()[0] == node2);
    QVERIFY(node1->children()[1] == node3);
    QVERIFY(node1->children()[2] == node4);

    // Add a null child (should do nothing).
    node1->addNode(0);
    QCOMPARE(node1->children().count(), 3);
    QVERIFY(node1->children()[0] == node2);
    QVERIFY(node1->children()[1] == node3);
    QVERIFY(node1->children()[2] == node4);

    // Add node3 under node2 as well so that it has multiple parents.
    node2->addNode(node3);
    QVERIFY(node3->parent() == node1);  // parent is unchanged
    QCOMPARE(node1->children().count(), 3);
    QVERIFY(node1->children()[0] == node2);
    QVERIFY(node1->children()[1] == node3);
    QVERIFY(node1->children()[2] == node4);
    QCOMPARE(node2->children().count(), 1);
    QVERIFY(node2->children()[0] == node3);

    // Recursively fetch all children.
    QList<QGLSceneNode *> children = node1->allChildren();
    QCOMPARE(children.count(), 3);
    QVERIFY(children[0] == node2);
    QVERIFY(children[1] == node3);
    QVERIFY(children[2] == node4);
    children = node2->allChildren();
    QCOMPARE(children.count(), 1);
    QVERIFY(children[0] == node3);
    QVERIFY(node3->allChildren().isEmpty());
    QVERIFY(node4->allChildren().isEmpty());

    // Delete the top node and ensure that the right objects are destroyed.
    QSignalSpy node1Spy(node1, SIGNAL(destroyed()));
    QSignalSpy node2Spy(node2, SIGNAL(destroyed()));
    QSignalSpy node3Spy(node3, SIGNAL(destroyed()));
    QSignalSpy node4Spy(node4, SIGNAL(destroyed()));
    delete node1;
    QCOMPARE(node1Spy.count(), 1);
    QCOMPARE(node2Spy.count(), 1);
    QCOMPARE(node3Spy.count(), 1);
    QCOMPARE(node4Spy.count(), 0);

    // Clean up the separate node.
    delete node4;
    QCOMPARE(node4Spy.count(), 1);
}

// Remove nodes from an existing scene graph.
void tst_QGLSceneNode::removeNode()
{
    QGLSceneNode *node1 = new QGLSceneNode();
    QGLSceneNode *node2 = new QGLSceneNode();
    QGLSceneNode *node3 = new QGLSceneNode();
    QGLSceneNode *node4 = new QGLSceneNode();
    QGLSceneNode *node5 = new QGLSceneNode(this);

    node1->addNode(node2);
    node1->addNode(node3);
    node1->addNode(node4);
    node2->addNode(node3);
    node2->addNode(node5);

    QSignalSpy node1Spy(node1, SIGNAL(destroyed()));
    QSignalSpy node2Spy(node2, SIGNAL(destroyed()));
    QSignalSpy node3Spy(node3, SIGNAL(destroyed()));
    QSignalSpy node4Spy(node4, SIGNAL(destroyed()));
    QSignalSpy node5Spy(node5, SIGNAL(destroyed()));

    // Removing a node will set its parent pointer back to null
    // if it was removed from its last parent.
    QVERIFY(node4->parent() == node1);
    node1->removeNode(node4);
    QVERIFY(node4->parent() == 0);

    // The node should still exist, just detached.
    QCOMPARE(node4Spy.count(), 0);
    delete node4;
    QCOMPARE(node4Spy.count(), 1);

    // Remove node3, which should transfer ownership.
    QVERIFY(node3->parent() == node1);
    node1->removeNode(node3);
    QVERIFY(node3->parent() == node2);

    // Add node3 back, and check non-transfer of ownership.
    node1->addNode(node3);
    QVERIFY(node3->parent() == node2);
    node1->removeNode(node3);
    QVERIFY(node3->parent() == node2);

    // Remove a node that wasn't parented to the scene graph.
    node2->removeNode(node5);
    QVERIFY(node5->parent() == this);

    // Clean up the rest of the graph.
    QCOMPARE(node1Spy.count(), 0);
    QCOMPARE(node2Spy.count(), 0);
    QCOMPARE(node3Spy.count(), 0);
    QCOMPARE(node4Spy.count(), 1);
    QCOMPARE(node5Spy.count(), 0);
    delete node1;
    QCOMPARE(node1Spy.count(), 1);
    QCOMPARE(node2Spy.count(), 1);
    QCOMPARE(node3Spy.count(), 1);
    QCOMPARE(node4Spy.count(), 1);
    QCOMPARE(node5Spy.count(), 0);
    delete node5;
    QCOMPARE(node5Spy.count(), 1);
}

static bool sameTransforms(const QList<QQuickQGraphicsTransform3D *> &transforms1,
                           const QList<QQuickQGraphicsTransform3D *> &transforms2)
{
    if (transforms1.size() != transforms2.size())
        return false;
    for (int index = 0; index < transforms1.size(); ++index) {
        // We only check the meta object at the moment, and assume
        // that the clone() worked properly.
        if (transforms1.at(index)->metaObject() !=
                transforms2.at(index)->metaObject())
            return false;
    }
    return true;
}

void tst_QGLSceneNode::clone()
{
    QGLSceneNode nodeParent;
    QGLSceneNode node1(&nodeParent);

    QGLSceneNode *node2 = node1.clone();
    QVERIFY(node2 != 0 && node2 != &node1);
    QVERIFY(node2->parent() == &nodeParent);
    QVERIFY(node2->options() == node1.options());
    QVERIFY(node2->geometry().isEmpty());
    QVERIFY(node2->localTransform().isIdentity());
    QVERIFY(node2->position() == QVector3D(0, 0, 0));
    QVERIFY(node2->transforms().isEmpty());
    QVERIFY(node2->drawingMode() == QGL::Triangles);
    QVERIFY(node2->effect() == QGL::FlatColor);
    QVERIFY(!node2->userEffect());
    QVERIFY(!node2->hasEffect());
    QCOMPARE(node2->start(), 0);
    QCOMPARE(node2->count(), 0);
    QCOMPARE(node2->materialIndex(), -1);
    QCOMPARE(node2->backMaterialIndex(), -1);
    QVERIFY(!node2->palette());
    QVERIFY(!node2->pickNode());
    QVERIFY(node2->children().isEmpty());
    delete node2;

    QGeometryData data1;
    data1.appendVertex(QVector3D(1, -2, 3));
    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);
    TestEffect userEffect;
    QGLPickNode pick;
    QList<QQuickQGraphicsTransform3D *> transforms;
    transforms.append(new QGraphicsScale3D(this));
    transforms.append(new QGraphicsRotation3D(this));

    node1.setOptions(QGLSceneNode::ViewNormals);
    node1.setGeometry(data1);
    node1.setLocalTransform(m);
    node1.setPosition(QVector3D(1, -2, 3));
    node1.setTransforms(transforms);
    node1.setDrawingMode(QGL::Points);
    node1.setEffect(QGL::LitMaterial);
    node1.setUserEffect(&userEffect);
    node1.setStart(10);
    node1.setCount(20);
    QGLMaterial *mat1 = new QGLMaterial();
    QGLMaterial *mat2 = new QGLMaterial();
    node1.setMaterial(mat1);
    node1.setBackMaterial(mat2);
    node1.setPickNode(&pick);
    QGLSceneNode *node3 = new QGLSceneNode();
    QGLSceneNode *node4 = new QGLSceneNode();
    node1.addNode(node3);
    node1.addNode(node4);

    // Clone onto the same parent.
    node2 = node1.clone();
    QVERIFY(node2 != 0 && node2 != &node1);
    QVERIFY(node2->parent() == &nodeParent);
    QVERIFY(node2->options() == node1.options());
    QCOMPARE(node2->geometry().count(), 1);
    QVERIFY(node2->localTransform() == m);
    QVERIFY(node2->position() == QVector3D(1, -2, 3));
    QVERIFY(sameTransforms(node2->transforms(), transforms));
    QVERIFY(node2->drawingMode() == QGL::Points);
    QVERIFY(node2->effect() == QGL::LitMaterial);
    QVERIFY(node2->userEffect() == &userEffect);
    QVERIFY(node2->hasEffect());
    QCOMPARE(node2->start(), 10);
    QCOMPARE(node2->count(), 20);
    QCOMPARE(node2->materialIndex(), 0);
    QCOMPARE(node2->backMaterialIndex(), 1);
    QVERIFY(node2->material() == mat1);
    QVERIFY(node2->backMaterial() == mat2);
    QVERIFY(node2->palette() != 0);
    QVERIFY(node2->palette() == node1.palette());
    QVERIFY(!node2->pickNode());    // Pick node should not be cloned
    QCOMPARE(node2->children().count(), 2);
    QVERIFY(node2->children()[0] == node3);
    QVERIFY(node2->children()[1] == node4);
    delete node2;

    // Clone onto a different parent.
    QGLSceneNode node2Parent;
    node2 = node1.clone(&node2Parent);
    QVERIFY(node2->parent() == &node2Parent);
    QCOMPARE(node2Parent.children().count(), 1);
    QVERIFY(node2Parent.children()[0] == node2);
    delete node2;
}

void tst_QGLSceneNode::boundingBox_data()
{
    QTest::addColumn<QVector3D>("bottomLeftFront");
    QTest::addColumn<QVector3D>("bottomRightFront");
    QTest::addColumn<QVector3D>("topRightFront");
    QTest::addColumn<QVector3D>("topLeftFront");
    QTest::addColumn<QVector3D>("scale");
    QTest::addColumn<QVector3D>("translation");
    QTest::addColumn<QVector3D>("rotation");
    QTest::addColumn<QVector3D>("boxMin");
    QTest::addColumn<QVector3D>("boxMax");

    QTest::newRow("cube-2x2x2-at-origin-no-xform")
            << QVector3D(-1, -1, -1)
            << QVector3D(1, -1, -1)
            << QVector3D(1, 1, -1)
            << QVector3D(-1, 1, -1)
            << QVector3D(0, 0, 0)
            << QVector3D(0, 0, 0)
            << QVector3D(0, 0, 0)
            << QVector3D(-1, -1, -1)
            << QVector3D(1, 1, 1);

    QTest::newRow("cube-2x2x2-offset-no-xform")
            << QVector3D(3, -1, -1)
            << QVector3D(5, -1, -1)
            << QVector3D(5, 1, -1)
            << QVector3D(3, 1, -1)
            << QVector3D(0, 0, 0)
            << QVector3D(0, 0, 0)
            << QVector3D(0, 0, 0)
            << QVector3D(3, -1, -1)
            << QVector3D(5, 1, 1);

    QTest::newRow("cube-2x2x2-at-origin-positioned")
            << QVector3D(-1, -1, -1)
            << QVector3D(1, -1, -1)
            << QVector3D(1, 1, -1)
            << QVector3D(-1, 1, -1)
            << QVector3D(0, 0, 0)
            << QVector3D(-4, -4, -4)
            << QVector3D(0, 0, 0)
            << QVector3D(-5, -5, -5)
            << QVector3D(-3, -3, -3);

    QTest::newRow("cube-2x2x2-offset-positioned")
            << QVector3D(3, -1, -1)
            << QVector3D(5, -1, -1)
            << QVector3D(5, 1, -1)
            << QVector3D(3, 1, -1)
            << QVector3D(0, 0, 0)
            << QVector3D(-4, -4, -4)
            << QVector3D(0, 0, 0)
            << QVector3D(-1, -5, -5)
            << QVector3D(1, -3, -3);

    QTest::newRow("cube-2x2x2-at-origin-positioned-scaled")
            << QVector3D(-1, -1, -1)
            << QVector3D(1, -1, -1)
            << QVector3D(1, 1, -1)
            << QVector3D(-1, 1, -1)
            << QVector3D(4, 4, 4)
            << QVector3D(-4, -4, -4)
            << QVector3D(0, 0, 0)
            << QVector3D(-20, -20, -20)
            << QVector3D(-12, -12, -12);

    QTest::newRow("cube-2x2x2-offset-positioned-scaled-rotated")
            << QVector3D(3, -1, -1)
            << QVector3D(5, -1, -1)
            << QVector3D(5, 1, -1)
            << QVector3D(3, 1, -1)
            << QVector3D(4, 4, 4)
            << QVector3D(-4, -4, -4)
            << QVector3D(0, 0, 45)
            << QVector3D(-4, -20, -20)
            << QVector3D(4, -12, -12);
}

void tst_QGLSceneNode::boundingBox()
{
    QFETCH(QVector3D, bottomLeftFront);
    QFETCH(QVector3D, bottomRightFront);
    QFETCH(QVector3D, topRightFront);
    QFETCH(QVector3D, topLeftFront);
    QFETCH(QVector3D, scale);
    QFETCH(QVector3D, translation);
    QFETCH(QVector3D, rotation);
    QFETCH(QVector3D, boxMin);
    QFETCH(QVector3D, boxMax);

    QBox3D expBox(boxMin, boxMax);
    QGLSceneNode *node = new QGLSceneNode;
    node->setObjectName(QLatin1String("Root node"));
    QGLSceneNode *geoNode = 0;
    QGeometryData data;
    QGLBuilder builder;

    data.appendVertex(bottomLeftFront, bottomRightFront, topRightFront, topLeftFront);
    builder.addQuads(data);    // top of cube
    builder.addQuadsInterleaved(data, data.translated(QVector3D(0, 0, 2)));  // sides of cube
    builder.addQuads(data.reversed());   // bottom of cube

    geoNode = builder.finalizedSceneNode();
    geoNode->setObjectName("Geometry node");
    node->addNode(geoNode);

    if (!scale.isNull())
    {
        QMatrix4x4 m = node->localTransform();
        m.scale(scale);
        node->setLocalTransform(m);
    }

    if (!translation.isNull())
    {
        QMatrix4x4 m = node->localTransform();
        m.translate(translation);
        node->setLocalTransform(m);
    }

    if (!rotation.isNull())
    {
        QMatrix4x4 m = node->localTransform();
        if (qIsNull(rotation.x()))
            m.rotate(rotation.x(), 1.0, 0.0, 0.0);
        if (qIsNull(rotation.y()))
            m.rotate(rotation.y(), 0.0, 1.0, 0.0);
        if (qIsNull(rotation.z()))
            m.rotate(rotation.z(), 0.0, 0.0, 1.0);
        node->setLocalTransform(m);
    }

    QBox3D gotBox = node->boundingBox();
    QCOMPARE(gotBox.minimum(), expBox.minimum());
    QCOMPARE(gotBox.maximum(), expBox.maximum());


    // now clear out the root nodes transform and apply the same transform
    // to the child node - should be exactly the same

    QMatrix4x4 saveMat = node->localTransform();
    node->setLocalTransform(QMatrix4x4());       // set back to identity
    geoNode->setLocalTransform(saveMat);

    gotBox = node->boundingBox();
    QCOMPARE(gotBox.minimum(), expBox.minimum());
    QCOMPARE(gotBox.maximum(), expBox.maximum());

    delete node;
}

class TestSceneNode : public QGLSceneNode
{
public:
    QMatrix4x4 resultingModelView;
    QMatrix4x4 transformMatrix;

    TestSceneNode()
        : QGLSceneNode()
    {
        //
    }

protected:
    virtual void drawGeometry(QGLPainter *painter)
    {
        resultingModelView = painter->modelViewMatrix().top();

        // basically reimplement the private function "transform()" here to get the value of the transforms
        QMatrix4x4 m;
        if (!position().isNull())
            m.translate(position());
        if (!localTransform().isIdentity())
            m *= localTransform();
        QList<QQuickQGraphicsTransform3D *> tx = transforms();
        for (int index = tx.size() - 1; index >= 0; --index)
            tx.at(index)->applyTo(&m);

        transformMatrix = m;
    }

};

static void ensureContext(QWindow &win, QOpenGLContext &ctx)
{
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    ctx.setFormat(format);
#ifndef QT_NO_DEBUG_STREAM
    QSurfaceFormat oldFormat = format;
#endif
    ctx.create();
    // TODO: is it possible that the platform will downgrade the actual
    // format, or will it just fail if it can't deliver the actual format
    format = ctx.format();
#ifndef QT_NO_DEBUG_STREAM
    if (oldFormat.swapBehavior() != format.swapBehavior())
        qWarning() << "Could not create context for swap behavior"
                   << oldFormat.swapBehavior();
#endif
    ctx.makeCurrent(&win);
}

void tst_QGLSceneNode::position_QTBUG_17279()
{
    QSKIP("QWidget: Cannot create a QWidget when no GUI is being used");
    QGeometryData geom;
    geom.appendVertex(QVector3D(0, 0, 0),
                      QVector3D(1.414f, 1.414f, 0),
                      QVector3D(2, 0, 0));
    TestSceneNode *node = new TestSceneNode;

    QVERIFY(node->resultingModelView.isIdentity());
    QVERIFY(node->transformMatrix.isIdentity());

    node->setGeometry(geom);
    node->setCount(3);
    node->setPosition(QVector3D(0.f, 0.f, -5.f));

    QVERIFY(node->localTransform().isIdentity());
    QCOMPARE(node->position().z(), -5.0);

    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid())
        QSKIP("GL Implementation not valid");

    QGLPainter p(&glw);
    QGLCamera cam;
    p.setCamera(&cam);

    QMatrix4x4 m = p.modelViewMatrix().top();

    node->draw(&p);

    QVERIFY(m == p.modelViewMatrix().top());

    QCOMPARE(node->resultingModelView(2, 3), -15.0);
    QCOMPARE(node->transformMatrix(2, 3), -5.0);

    QGeometryData geom2;
    geom2.appendVertex(QVector3D(0, 0, 0),
                      QVector3D(-1.414f, 1.414f, 0),
                      QVector3D(-2, 0, 0));
    TestSceneNode *node2 = new TestSceneNode;
    node2->setGeometry(geom2);
    node2->setCount(3);
    node2->setPosition(QVector3D(0.f, -1.f, -5.f));

    node2->draw(&p);

    QVERIFY(m == p.modelViewMatrix().top());

    QCOMPARE(node2->resultingModelView(1, 3), -1.0);
    QCOMPARE(node2->transformMatrix(1, 3), -1.0);
    QCOMPARE(node2->resultingModelView(2, 3), -15.0);
    QCOMPARE(node2->transformMatrix(2, 3), -5.0);
}

void tst_QGLSceneNode::findSceneNode()
{
    //Create the following scenegraph
    //    ""--+--"Item1"
    //        |
    //        +--"Item2"--""--"Item1"--"Item3"
    QGeometryData geom;
    geom.appendVertex(QVector3D(0, 0, 0),
                      QVector3D(1.414f, 1.414f, 0),
                      QVector3D(2, 0, 0));

    QGLBuilder builder;
    QGLSceneNode * root = builder.currentNode();

    builder.pushNode()->setObjectName(QLatin1String("Item1"));
    builder.currentNode()->setGeometry(geom);
    QGLSceneNode *Item1Node1 = builder.currentNode();

    builder.newNode()->setObjectName(QLatin1String("Item2"));
    builder.currentNode()->setGeometry(geom);

    builder.pushNode();
    builder.currentNode()->setGeometry(geom);

    builder.pushNode()->setObjectName(QLatin1String("Item1"));
    builder.currentNode()->setGeometry(geom);
    QGLSceneNode *Item1Node2 = builder.currentNode();

    builder.pushNode()->setObjectName(QLatin1String("Item3"));
    builder.currentNode()->setGeometry(geom);

    //Test various combinations of node finding
    QString test1Str("Item1");
    QString test2Str("Item3");
    QString test3Str("Item2::Item1");
    QString test4Str("Item2::::Item1");
    QString test5Str("Item1::Item1");
    QString test6Str("Item4");
    QGLSceneNode *nullNode = 0;

    QCOMPARE(root->findSceneNode(test1Str), Item1Node1);
    QCOMPARE(root->findSceneNode(test2Str), nullNode);
    QCOMPARE(root->findSceneNode(test3Str), Item1Node2);
    QCOMPARE(root->findSceneNode(test4Str), Item1Node2);
    QCOMPARE(root->findSceneNode(test5Str), nullNode);
    QCOMPARE(root->findSceneNode(test6Str) , nullNode);

    builder.finalizedSceneNode();
}

QTEST_MAIN(tst_QGLSceneNode)

#include "tst_qglscenenode.moc"
