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
#include <QtCore/qpointer.h>
#include "qglbuilder.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include "qglsection_p.h"
#include "qglmaterialcollection.h"
#include "qglscenenode.h"
#include "qglabstracteffect.h"
#include "qtest_helpers.h"
#include "qgeometrydata.h"

class tst_QGLBuilder : public QObject
{
    Q_OBJECT
public:
    tst_QGLBuilder() {}
    ~tst_QGLBuilder() {}

private slots:
    void createDefault();
    void newSection();
    void newNode();
    void pushNode();
    void popNode();
    void geometryBuild();
    void addTriangles();
    void addQuads();
    void addTriangleFan();
    void addTriangulatedFace();
    void extrude();
    void finalize();
};

// Indices in a QGL::IndexArray are int on desktop, ushort on OpenGL/ES.
// This macro works around the discrepancy to avoid confusing QCOMPARE.
#define QCOMPARE_INDEX(x,y)     QCOMPARE(int(x), int(y))

class TestBuilder : public QGLBuilder
{
public:
    QGLSection *currentSection() { return QGLBuilder::currentSection(); }
    QList<QGLSection*> sections() { return QGLBuilder::sections(); }
};

void tst_QGLBuilder::createDefault()
{
    // Test that a newly created builder works with no defaults
    TestBuilder builder;
    QCOMPARE(builder.currentSection(), (QGLSection*)0);
    QCOMPARE(builder.sections().size(), 0);
    QVERIFY(builder.currentNode() != 0);
    QVERIFY(builder.sceneNode() != 0);
    QVERIFY(builder.sceneNode()->geometry().isEmpty());
    QVERIFY(builder.palette() != 0);
    QGLSceneNode *root = builder.sceneNode();
    QCOMPARE(builder.finalizedSceneNode(), root);
}

void tst_QGLBuilder::newSection()
{
    TestBuilder builder;
    builder.newSection(); // defaults to smooth
    QGLSection *s = builder.currentSection();
    QCOMPARE(s, builder.currentSection());
    QCOMPARE(builder.sections().count(), 1);
    QVERIFY(builder.sections().contains(s));
    QCOMPARE(s->smoothing(), QGL::Smooth);
    builder.newSection(QGL::Faceted);
    QGLSection *s2 = builder.currentSection();
    QCOMPARE(builder.sections().count(), 2);
    QVERIFY(builder.sections().contains(s2));
    builder << QGL::Faceted;
    QCOMPARE(builder.sections().count(), 3);
    QGLSceneNode *root = builder.sceneNode();
    QCOMPARE(builder.finalizedSceneNode(), root);
}

class TestEffect : public QGLAbstractEffect
{
    void setActive(QGLPainter *, bool)
    {
    }
    void update(QGLPainter *, QGLPainter::Updates)
    {
    }
};

void tst_QGLBuilder::newNode()
{
    TestBuilder builder;
    builder.newSection();  // calls new node
    QGLSceneNode *node = builder.currentNode();

    // newly created node works and has all defaults
    QCOMPARE(node->effect(), QGL::FlatColor); // flat color is the default
    QCOMPARE(node->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node->materialIndex(), -1);
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 0);

    // add some settings and geometry to the new node
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    node->setUserEffect(eff);
    node->setMaterialIndex(5);
    QGeometryData p;
    p.appendVertex(QVector3D(), QVector3D(1.0f, 2.0f, 3.0f), QVector3D(4.0f, 5.0f, 6.0f));
    builder.addTriangles(p);

    // now create a new node
    QGLSceneNode *node2 = builder.newNode();

    // the previous node got cleaned up properly, with its count updated
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 3);

    // new node counts off from where the previous one finished and has same defaults
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 0);
    QCOMPARE(node2->effect(), QGL::FlatColor); // lit material is the default
    QCOMPARE(node2->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node2->materialIndex(), -1);

    builder.addTriangles(p);
    builder.newNode();

    // confirm that 2nd and last node in chain was also finished properly
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 3);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::pushNode()
{
    TestBuilder builder;
    builder.newSection();
    QGLSceneNode *node = builder.newNode();
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    node->setUserEffect(eff);
    node->setMaterialIndex(5);
    QGeometryData p;
    p.appendVertex(QVector3D(), QVector3D(1.0f, 2.0f, 3.0f), QVector3D(4.0f, 5.0f, 6.0f));
    builder.addTriangles(p);

    QGLSceneNode *node2 = builder.pushNode();
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 3);
    QCOMPARE(builder.currentNode(), node2);
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 0);
    QCOMPARE(node2->parent(), node);
    QCOMPARE(node2->effect(), QGL::FlatColor); // lit material is the default
    QCOMPARE(node2->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node2->materialIndex(), -1);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::popNode()
{
    TestBuilder builder;
    builder.newSection();
    QGLSceneNode *node = builder.newNode();
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    QMatrix4x4 mat;
    mat.rotate(45.0f, 0.0f, 1.0f, 0.0f);
    node->setLocalTransform(mat);
    node->setUserEffect(eff);
    node->setMaterialIndex(5);
    QGeometryData p;
    p.appendVertex(QVector3D(), QVector3D(1.0f, 2.0f, 3.0f), QVector3D(4.0f, 5.0f, 6.0f));
    builder.addTriangles(p);

    QGLSceneNode *node2 = builder.pushNode();

    builder.addTriangles(p);

    QGLSceneNode *node3 = builder.popNode();
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 3);

    QCOMPARE(node3->effect(), QGL::LitDecalTexture2D);
    QCOMPARE(node3->userEffect(), eff);
    QCOMPARE(node3->materialIndex(), 5);
    QCOMPARE(node3->localTransform(), mat);
    QCOMPARE(node3->start(), 6);
    QCOMPARE(node3->count(), 0);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::geometryBuild()
{
    // here we really just test that the right values get added
    // to the underlying QGeometryData
    TestBuilder builder;
    builder.newSection();
    QGLSection *sec = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();

    QCOMPARE(sec->count(), 0);  // empty to start off with
    QCOMPARE(node->count(), 0);

    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D e(1.0f, 2.0f, 0.0f);
    QVector3D f(-1.0f, 2.0f, 0.0f);
    QVector3D origin;
    QVector3D norm(0.0f, 0.0f, 1.0f);

    QGeometryData p;
    p.appendVertex(a, b, c);
    builder.addTriangles(p);
    QCOMPARE(sec->count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(node->count(), 3);

    builder.newSection();
    QGeometryData q;
    sec = builder.currentSection();
    node = builder.currentNode();
    QCOMPARE(sec->count(), 0);  // empty to start off with
    QCOMPARE(node->count(), 0);
    QVector3DArray data;
    data << a << b << d << c << f << e;
    q.appendVertexArray(data);
    builder.addTriangleStrip(q);
    QCOMPARE(sec->count(), 6);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), d);
    QCOMPARE(sec->vertex(3), c);
    QCOMPARE(sec->vertex(4), f);
    QCOMPARE(sec->vertex(5), e);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->normal(5).normalized(), norm);
    QCOMPARE(node->count(), 12); // TRIANGLE_STRIP will here draw 4 triangles = 12 indices

    // now go on and test TRIANGLE_FAN
    builder.newSection();
    sec = builder.currentSection();
    node = builder.currentNode();
    QGeometryData r;
    r.appendVertex(a);
    r.appendNormal(norm);
    r.appendVertex(b);
    r.appendNormal(norm);
    r.appendVertex(c);
    r.appendNormal(norm);
    r.appendVertex(d);
    r.appendNormal(norm);
    r.appendVertex(e);
    r.appendNormal(norm);
    builder.addTriangleFan(r);
    QCOMPARE(sec->count(), 5);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->vertex(4), e);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(4).normalized(), norm);
    QCOMPARE(node->count(), 9); // TRIANGLE_FAN will here draw 3 triangles = 9 indices

    QGeometryData s;
    //builder.begin(QGL::TRIANGULATED_FACE);
    builder.newSection();
    sec = builder.currentSection();
    node = builder.currentNode();
    s.appendVertex(a);
    s.appendColor(Qt::red);
    s.appendVertex(b);
    s.appendColor(QColor4ub(Qt::green));
    s.appendVertex(c);
    s.appendVertex(d);
    s.appendVertex(e);
    QArray<QColor4ub> cdata;
    cdata.append(Qt::blue, Qt::yellow, Qt::black);
    s.appendColorArray(cdata);
    builder.addTriangulatedFace(s);
    QCOMPARE(sec->count(), 5);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(4), e);
    QCOMPARE(sec->colorAt(0), QColor4ub(Qt::red));
    QCOMPARE(sec->colorAt(1), QColor4ub(Qt::green));
    QCOMPARE(sec->colorAt(2), QColor4ub(Qt::blue));
    QCOMPARE(sec->colorAt(3), QColor4ub(Qt::yellow));
    QCOMPARE(sec->colorAt(4), QColor4ub(Qt::black));
    QCOMPARE(node->count(), 12); // TRIANGLE_FAN will here draw 4 triangles = 12 indices

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::addTriangles()
{
    TestBuilder builder;
    builder.newSection();
    QGLSection *sec = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector2D ta(0.0, 0.0);
    QVector2D tb(1.0, 0.0);
    QVector2D tc(1.0, 1.0);
    QVector3D norm(0.0f, 0.0f, 1.0f);
    QGeometryData p;
    p.appendVertex(a, b, c);
    p.appendNormal(norm, norm, norm);

    builder.addTriangles(p);
    QCOMPARE(sec->vertices().count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->normals().count(), 3);
    QCOMPARE(sec->indices().size(), 3);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector2D td(0.0f, 1.0f);
    QGeometryData q;
    q.appendVertex(a, c, d);
    p.appendNormal(norm, norm, norm, norm);
    builder.addTriangles(q);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    builder.newSection();
    sec = builder.currentSection();
    node = builder.currentNode();
    QGeometryData r;
    r.appendVertex(a, b, c);
    r.appendTexCoord(ta, tb, tc);
    builder.addTriangles(r);
    QCOMPARE(sec->vertices().count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->normals().count(), 3);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 3);
    QCOMPARE(sec->texCoords().at(0), ta);
    QCOMPARE(sec->texCoords().at(1), tb);
    QCOMPARE(sec->texCoords().at(2), tc);
    QCOMPARE(sec->indices().size(), 3);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE(sec->fields(),
             QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal) | QGL::fieldMask(QGL::TextureCoord0));

    QGeometryData s;
    s.appendVertex(a, b, d);
    s.appendTexCoord(ta, tb, td);
    builder.addTriangles(s);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 4);
    QCOMPARE(sec->texCoords().at(0), QVector2D(0.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(1), QVector2D(1.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(2), QVector2D(1.0f, 1.0f));
    QCOMPARE(sec->texCoords().at(3), QVector2D(0.0f, 1.0f));
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 1);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(),
             QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal) | QGL::fieldMask(QGL::TextureCoord0));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    // correctly handle not being a multiple of 3 - discard spare ones
    builder.newSection();
    sec = builder.currentSection();
    QGeometryData t;
    t.appendVertex(a, b, c, d);
    builder.addTriangles(t);
    QCOMPARE(sec->count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::addQuads()
{
    TestBuilder builder;
    builder.newSection();
    QGLSection *sec = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D norm(0.0f, 0.0f, 1.0f);
    QGeometryData p;
    p.appendVertex(a, b, c, d);
    p.appendNormal(norm, norm, norm, norm);

    builder.addQuads(p);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    builder.newSection();
    sec = builder.currentSection();
    node = builder.currentNode();
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    QVector2D td(0.0f, 1.0f);
    QGeometryData q;
    q.appendVertex(a, b, c, d);
    q.appendTexCoord(ta, tb, tc, td);
    builder.addQuads(q);

    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 4);
    QCOMPARE(sec->texCoords().at(0), ta);
    QCOMPARE(sec->texCoords().at(1), tb);
    QCOMPARE(sec->texCoords().at(2), tc);
    QCOMPARE(sec->texCoords().at(3), td);
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(),
             QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal) | QGL::fieldMask(QGL::TextureCoord0));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::addTriangleFan()
{
    TestBuilder builder;
    builder.newSection(QGL::Faceted);
    QGLSection *sec = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    float one_on_root2 = 1.0f / sqrtf(2.0f);
    QVector3D n1(0.0f, -one_on_root2, one_on_root2);
    QVector3D n2(one_on_root2, 0.0f, one_on_root2);
    QVector3D n3(0.0f, one_on_root2, one_on_root2);
    QVector3D center(0.0f, 0.0f, 1.0f);
    QVector2DArray edges;

    // if edges has length < 2, exit without doing anything
    QGeometryData p;
    p.appendVertex(center);
    builder.addTriangleFan(p);
    QCOMPARE(sec->vertices().count(), 0);
    QCOMPARE(sec->normals().count(), 0);
    QCOMPARE(sec->indices().size(), 0);

    p.appendVertex(a, b, c, d);

    builder.addTriangleFan(p);
    sec->normalizeNormals();
    QCOMPARE(sec->vertices().count(), 9);
    QCOMPARE(sec->vertex(0), center);
    QCOMPARE(sec->vertex(1), a);
    QCOMPARE(sec->vertex(2), b);
    QCOMPARE(sec->vertex(3), center);
    QCOMPARE(sec->vertex(4), b);
    QCOMPARE(sec->vertex(5), c);
    QCOMPARE(sec->vertex(6), center);
    QCOMPARE(sec->vertex(7), c);
    QCOMPARE(sec->vertex(8), d);
    QCOMPARE(sec->normals().count(), 9);
    QCOMPARE(sec->normal(0), n1);
    QCOMPARE(sec->normal(1), n1);
    QCOMPARE(sec->normal(2), n1);
    QCOMPARE(sec->normal(3), n2);
    QCOMPARE(sec->normal(4), n2);
    QCOMPARE(sec->normal(5), n2);
    QCOMPARE(sec->normal(6), n3);
    QCOMPARE(sec->normal(7), n3);
    QCOMPARE(sec->normal(8), n3);
    QCOMPARE(sec->indices().size(), 9);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 3);
    QCOMPARE_INDEX(sec->indices()[4], 4);
    QCOMPARE_INDEX(sec->indices()[5], 5);
    QCOMPARE_INDEX(sec->indices()[6], 6);
    QCOMPARE_INDEX(sec->indices()[7], 7);
    QCOMPARE_INDEX(sec->indices()[8], 8);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 9);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::addTriangulatedFace()
{
    TestBuilder builder;
    builder.newSection(QGL::Faceted);
    QGLSection *sec = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector2DArray edges;

    // if edges has length < 2, exit without doing anything
    QGeometryData p;
    builder.addTriangleFan(p);
    QCOMPARE(sec->vertices().count(), 0);
    QCOMPARE(sec->normals().count(), 0);
    QCOMPARE(sec->indices().size(), 0);

    p.appendVertex(center);
    p.appendVertex(a, b, c, d);

    builder.addTriangulatedFace(p);
    sec->normalizeNormals();
    QCOMPARE(sec->vertices().count(), 5);
    QCOMPARE(sec->vertex(0), center);
    QCOMPARE(sec->vertex(1), a);
    QCOMPARE(sec->vertex(2), b);
    QCOMPARE(sec->vertex(3), c);
    QCOMPARE(sec->vertex(4), d);
    QCOMPARE(sec->normals().count(), 5);
    QCOMPARE(sec->normal(0), n);
    QCOMPARE(sec->normal(1), n);
    QCOMPARE(sec->normal(2), n);
    QCOMPARE(sec->normal(3), n);
    QCOMPARE(sec->normal(4), n);
    QCOMPARE(sec->indices().size(), 12);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE_INDEX(sec->indices()[6], 0);
    QCOMPARE_INDEX(sec->indices()[7], 3);
    QCOMPARE_INDEX(sec->indices()[8], 4);
    QCOMPARE_INDEX(sec->indices()[9], 0);
    QCOMPARE_INDEX(sec->indices()[10], 4);
    QCOMPARE_INDEX(sec->indices()[11], 1);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 12);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::extrude()
{
    TestBuilder builder;
    builder.newSection();
    QGLSection *sec = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QVector3DArray edges;

    // if edges has length < 2, exit without doing anything
    QGeometryData p;
    QGeometryData q;
    builder.addQuadsInterleaved(p, q);
    QCOMPARE(sec->vertices().count(), 0);
    QCOMPARE(sec->normals().count(), 0);
    QCOMPARE(sec->indices().size(), 0);

    p.appendVertex(a, b, c, d);
    edges = p.vertices();
    p.appendVertex(a);
    q = p.translated(-n);

    float one_on_root2 = 1.0f / sqrtf(2.0f);
    QVector3D n1(-one_on_root2, -one_on_root2, 0.0f);
    QVector3D n2(one_on_root2, -one_on_root2, 0.0f);
    QVector3D n3(one_on_root2, one_on_root2, 0.0f);
    QVector3D n4(-one_on_root2, one_on_root2, 0.0f);

    builder.addQuadsInterleaved(p, q);
    //sec->normalizeNormals();

    QCOMPARE(sec->count(), 8);
    QVector3DArray vrts = sec->vertices();
    QVector3DArray nrms = sec->normals();
    QGL::IndexArray inxs = sec->indices();

    QCOMPARE(vrts.count(), 8);
    QCOMPARE(vrts.at(0), edges.at(0) - n);
    QCOMPARE(vrts.at(1), edges.at(1) - n);
    QCOMPARE(vrts.at(2), edges.at(1));
    QCOMPARE(vrts.at(3), edges.at(0));
    QCOMPARE(vrts.at(4), edges.at(2) - n);
    QCOMPARE(vrts.at(5), edges.at(2));
    QCOMPARE(vrts.at(6), edges.at(3) - n);
    QCOMPARE(vrts.at(7), edges.at(3));

    QCOMPARE(nrms.count(), 8);
    QCOMPARE(nrms.at(0).normalized(), n1);
    QCOMPARE(nrms.at(1).normalized(), n2);
    QCOMPARE(nrms.at(2).normalized(), n2);
    QCOMPARE(nrms.at(3).normalized(), n1);
    QCOMPARE(nrms.at(4).normalized(), n3);
    QCOMPARE(nrms.at(5).normalized(), n3);
    QCOMPARE(nrms.at(6).normalized(), n4);
    QCOMPARE(nrms.at(7).normalized(), n4);

    static int checkIx[] = {
        0, 1, 2, 0, 2, 3,
        1, 4, 5, 1, 5, 2,
        4, 6, 7, 4, 7, 5,
        6, 0, 3, 6, 3, 7
    };
    QCOMPARE(sec->indices().size(), 24);

    QCOMPARE_INDEX(inxs[0], checkIx[0]);
    QCOMPARE_INDEX(inxs[1], checkIx[1]);
    QCOMPARE_INDEX(inxs[2], checkIx[2]);
    QCOMPARE_INDEX(inxs[3], checkIx[3]);
    QCOMPARE_INDEX(inxs[4], checkIx[4]);
    QCOMPARE_INDEX(inxs[5], checkIx[5]);
    QCOMPARE_INDEX(inxs[6], checkIx[6]);
    QCOMPARE_INDEX(inxs[7], checkIx[7]);
    QCOMPARE_INDEX(inxs[8], checkIx[8]);
    QCOMPARE_INDEX(inxs[9], checkIx[9]);
    QCOMPARE_INDEX(inxs[10], checkIx[10]);
    QCOMPARE_INDEX(inxs[11], checkIx[11]);
    QCOMPARE_INDEX(inxs[12], checkIx[12]);
    QCOMPARE_INDEX(inxs[13], checkIx[13]);
    QCOMPARE_INDEX(inxs[14], checkIx[14]);
    QCOMPARE_INDEX(inxs[15], checkIx[15]);
    QCOMPARE_INDEX(inxs[16], checkIx[16]);
    QCOMPARE_INDEX(inxs[17], checkIx[17]);
    QCOMPARE_INDEX(inxs[18], checkIx[18]);
    QCOMPARE_INDEX(inxs[19], checkIx[19]);
    QCOMPARE_INDEX(inxs[20], checkIx[20]);
    QCOMPARE_INDEX(inxs[21], checkIx[21]);
    QCOMPARE_INDEX(inxs[22], checkIx[22]);
    QCOMPARE_INDEX(inxs[23], checkIx[23]);

    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 24);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLBuilder::finalize()
{
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);

    // backwards wound triangle - on back face
    QVector3D e(1.0f, -1.0f, 1.0f);
    QVector3D f(-1.0f, -1.0f, 1.0f);
    QVector3D g(-1.0f, 1.0f, 1.0f);
    // normal points back
    QVector3D n10(0.0f, 0.0f, -1.0f);

    QGeometryData p;
    p.appendVertex(center);
    p.appendVertex(a, b, c, d);

    float one_on_root2 = 1.0f / sqrtf(2.0f);
    QVector3D n0(0.0f, 0.0f, 1.0f);
    QVector3D n1(-one_on_root2, -one_on_root2, 0.0f);
    QVector3D n2(one_on_root2, -one_on_root2, 0.0f);
    QVector3D n3(one_on_root2, one_on_root2, 0.0f);
    QVector3D n4(-one_on_root2, one_on_root2, 0.0f);

    TestBuilder builder;
    builder.newSection();
    QGLSceneNode *node = builder.currentNode();

    builder.addTriangulatedFace(p);

    builder.newSection();
    QGLSceneNode *node2 = builder.currentNode();

    QGeometryData s;
    s.appendVertex(a, b, c, d);
    s.appendVertex(a);
    builder.addQuadsInterleaved(s, s.translated(-n));

    QPointer<QGLSceneNode> nodeEmpty0 = builder.newNode();

    builder.newSection();
    QGLSceneNode *node3 = builder.currentNode();

    QGeometryData q;
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    q.appendVertex(e, f, g);
    q.appendTexCoord(ta, tb, tc);
    // reverse winding, backwards normal == n10
    builder.addTriangles(q);

    QPointer<QGLSceneNode> nodeEmpty1 = builder.pushNode();

    builder.finalizedSceneNode();

    QCOMPARE(nodeEmpty0.data(), (QGLSceneNode*)0);
    QCOMPARE(nodeEmpty1.data(), (QGLSceneNode*)0);

    QCOMPARE(builder.sections().count(), 0);

    QGeometryData geom = node->geometry();
    QGL::IndexArray ids = geom.indices();

    QCOMPARE(geom.count(QGL::Position), 13);
    QCOMPARE(ids.size(), 36);

    // triangulated face
    int tf = ids[node->start()]; // beginning of triangulated face
    QCOMPARE(node->count(), 12);
    QCOMPARE(geom.vertexAt(tf), center);
    QCOMPARE(geom.vertexAt(tf + 2), b);
    QCOMPARE(geom.normalAt(tf), n0);
    QCOMPARE(geom.normalAt(tf + 2), n0);

    geom = node2->geometry();
    ids = geom.indices();

    int ext = ids[node2->start()]; // beginning of extrude
    int last = ids[node2->start() + (node2->count() - 1)];
    QCOMPARE(node2->count(), 24);
    QCOMPARE(geom.vertexAt(ext), a - n);
    QCOMPARE(geom.normalAt(ext), n1);
    QCOMPARE(geom.vertexAt(last), d);
    QCOMPARE(geom.normalAt(last), n4);

    // the first two nodes and the builder itself all reference the same
    // geometry object, since they all have the same types: just vertices
    // and normals.  the last node has geometry with textures, so its in
    // a different geometry object.
    QVERIFY(node->geometry() == geom);
    QVERIFY(node2->geometry() == geom);
    QVERIFY(!(node3->geometry() == geom));

    geom = node3->geometry();
    QGL::IndexArray ids2 = geom.indices();

    int tri = ids2[node->start()];
    QCOMPARE(geom.vertexAt(tri), e);
    QCOMPARE(geom.normalAt(tri), n10);
    QCOMPARE(geom.texCoordAt(tri), ta);
}

QTEST_APPLESS_MAIN(tst_QGLBuilder)

#include "tst_qglbuilder.moc"
