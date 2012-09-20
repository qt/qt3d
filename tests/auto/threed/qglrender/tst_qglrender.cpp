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

#include "qglrenderorder.h"
#include "qglrendersequencer.h"
#include "qglrenderordercomparator.h"
#include "qglscenenode.h"
#include "qglpainter.h"
#include "qglbuilder.h"
#include "qglview.h"

class tst_QGLRender : public QObject
{
    Q_OBJECT
public:
    tst_QGLRender() {}
    ~tst_QGLRender() {}

private slots:
    void create();
    void values();
    void repo();
    void sequence();
};

void tst_QGLRender::create()
{
    QGLSceneNode node;
    QGLSceneNode node2;
    QSharedPointer<QGLMaterialCollection> pal(new QGLMaterialCollection());
    node.setPalette(pal);
    node2.setPalette(pal);

    QGLRenderState s;
    QGLRenderOrder order(&node, s);
    QGLRenderOrder other(order);     // copy constructor
    QCOMPARE(order.node(), &node);
    QCOMPARE(order.node(), other.node());
    QGLRenderOrderComparator cmp;
    QVERIFY(cmp.isEqualTo(order, other));          // operator==

    QGLRenderOrder other2(&node2, s);
    QVERIFY(cmp.isEqualTo(other2, order));  // same because nodes have same values
    other2 = order;                  // operator=
    QVERIFY(cmp.isEqualTo(other2, order));  // same again

    QGLRenderState state0;
    QGLRenderOrder other3(&node2, state0);
    QVERIFY(cmp.isEqualTo(other2, other3));
}

void tst_QGLRender::values()
{
    QGLSceneNode node;   // by default has QGL::FlatColor
    QGLSceneNode node2;
    QSharedPointer<QGLMaterialCollection> pal(new QGLMaterialCollection());
    node.setPalette(pal);
    node2.setPalette(pal);
    node2.setEffect(QGL::LitMaterial);

    QGLRenderState s;
    QGLRenderOrder order(&node, s);
    QGLRenderOrder other(&node2, s);
    QGLRenderOrderComparator cmp;
    QVERIFY(!cmp.isEqualTo(other, order));
    QVERIFY(cmp.isLessThan(order, other));

    QGLRenderState state;
    QGLMaterial *mat = new QGLMaterial;
    mat->setAmbientColor(Qt::blue);
    int ix = pal->addMaterial(mat);
    QGLSceneNode node3;
    node3.setPalette(pal);
    node3.setMaterialIndex(ix);
    node3.setEffect(QGL::LitMaterial);
    state.updateFrom(&node3);
    QGLRenderOrder other2(&node, state);
    QVERIFY(other2.effectiveHasEffect());
    QCOMPARE(other2.effectiveMaterial(), mat);
    QCOMPARE(other2.effectiveStandardEffect(), QGL::LitMaterial);
    QCOMPARE(other2.effectiveUserEffect(), (QGLAbstractEffect*)0);
    QVERIFY(cmp.isLessThan(order, other2));
    QVERIFY(cmp.isLessThan(other, other2));
}

void tst_QGLRender::repo()
{
    QGLSceneNode node;   // by default has QGL::FlatColor
    QGLSceneNode node2;
    QSharedPointer<QGLMaterialCollection> pal(new QGLMaterialCollection());
    node.setPalette(pal);
    node2.setPalette(pal);
    node2.setEffect(QGL::LitDecalTexture2D);

    QGLRenderState s;
    QGLRenderOrder order(&node, s);
    QGLRenderOrder other(&node2, s);
    QGLRenderOrder order2(&node, s);
    QGLRenderOrderComparator cmp;
    QVERIFY(!cmp.isEqualTo(other, order));
    QVERIFY(cmp.isLessThan(order, other));
    QVERIFY(cmp.isEqualTo(order2, order));
    QCOMPARE(order2.node(), order.node());
}

class TestPainter : public QGLPainter
{
public:
    TestPainter(QWindow *w) : QGLPainter(w) {}
    void draw(QGL::DrawingMode mode, const QGLIndexBuffer& indices,
              int offset, int count)
    {
        m_starts.append(offset);
        m_counts.append(count);
        QGLPainter::draw(mode, indices, offset, count);
    }
    QList<int> starts() const { return m_starts; }
    QList<int> counts() const { return m_counts; }
private:
    QList<int> m_starts;
    QList<int> m_counts;
};

class TestView : public QGLView
{
public:
    TestView(QGLSceneNode *node)
        : m_node(node)
    {}
    void paintGL(QGLPainter *painter)
    {
        painter->setEye(QGL::NoEye);
        painter->setCamera(camera());
        m_node->draw(painter);
    }
private:
    QGLSceneNode *m_node;
};

void tst_QGLRender::sequence()
{
    QSKIP("QWidget: Cannot create a QWidget when no GUI is being used");
    QSharedPointer<QGLMaterialCollection> palette(new QGLMaterialCollection());

    // create a yellow lit material
    QGLMaterial *mat = new QGLMaterial;
    mat->setAmbientColor(Qt::yellow);
    int ix0 = palette->addMaterial(mat);

    // create a blue lit material
    mat = new QGLMaterial;
    mat->setAmbientColor(Qt::blue);
    int ix1 = palette->addMaterial(mat);

    // create a grey textured material
    int tx0;
    {
        QImage uv(1024, 1024, QImage::Format_ARGB32);
        uv.fill(qRgba(196, 196, 196, 196));
        mat = new QGLMaterial;
        mat->setAmbientColor(Qt::gray);
        QGLTexture2D *tex = new QGLTexture2D;
        tex->setImage(uv);
        mat->setTexture(tex);
        tx0 = palette->addMaterial(mat);
    }

    QGLSceneNode *scene = new QGLSceneNode;
    scene->setPalette(palette);
    QGLSceneNode *node = 0;
    QGLSceneNode *prim;
    {
        QGLBuilder builder(palette);
        QVector3D a(-1.0f, -1.0f, 0.0f);
        QVector3D b(1.0f, -1.0f, 0.0f);
        QVector3D c(1.0f, 1.0f, 0.0f);
        QGeometryData p;
        p.appendVertex(a, b, c);
        p.generateTextureCoordinates();
        builder.addTriangles(p);
        prim = builder.currentNode();
        prim->setMaterialIndex(ix0);
        builder.newSection();
        QVector3D d(-1.2f, -1.2f, 0.0f);
        QVector3D e(1.2f, -1.2f, 0.0f);
        QVector3D f(1.2f, 1.2f, 0.0f);
        QVector3D g(-1.2f, 1.2f, 0.0f);
        QGeometryData q;
        q.appendVertex(d, e, f, g);
        q.generateTextureCoordinates();
        builder.addQuads(q);
        prim = builder.currentNode();
        prim->setMaterialIndex(ix1);
        node = builder.finalizedSceneNode();
    }

    scene->addNode(node);
    QGLSceneNode *cl = prim->clone(scene);
    cl->setMaterialIndex(tx0);
    cl->setEffect(QGL::LitDecalTexture2D);

    TestView widget(scene);
    if (!widget.context()->isValid())
        QSKIP("GL Implementation not valid");

    TestPainter *ptr = new TestPainter(&widget);

    widget.paintGL(ptr);

    QList<int> starts = ptr->starts();
    QList<int> counts = ptr->counts();
    QCOMPARE(starts.at(0), 0);
    QCOMPARE(counts.at(0), 3);
    QCOMPARE(starts.at(1), 3);
    QCOMPARE(counts.at(1), 6);
}

QTEST_MAIN(tst_QGLRender)

#include "tst_qglrender.moc"
