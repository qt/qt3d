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
#include "qglsection_p.h"
#include "qglbuilder.h"
#include "qgeometrydata.h"
#include "qtest_helpers.h"
#include "qvector3darray.h"

class tst_QGLSection : public QObject
{
    Q_OBJECT
public:
    tst_QGLSection() {}
    ~tst_QGLSection() {}
    void testSmooth(QGLSection *sec, QGLBuilder *builder);
    void testFaceted(QGLSection *sec, QGLBuilder *builder);

private slots:
    void create();
    void modify();
    void append();
    void appendSmooth();
    void appendSmoothMap();
    void appendFaceted();
    void appendFacetedMap();
    void accumNormals();
    void normalizedNormals();
    void appendTexCoord();
    void appendColor();
    void accessors();
};

// Indices in a QGLIndexArray are int on desktop, ushort on OpenGL/ES.
// This macro works around the discrepancy to avoid confusing QCOMPARE.
#define QCOMPARE_INDEX(x,y)     QCOMPARE(int(x), int(y))

class QGLSectionTest : public QGLSection
{
public:
    QGLSectionTest(QGLBuilder *builder, QGL::Smoothing s = QGL::Smooth)
        : QGLSection(builder, s) {}

    void appendSmooth(const QLogicalVertex &vertex)
    {
        QGLSection::appendSmooth(vertex);
    }

    void appendFaceted(const QLogicalVertex &vertex)
    {
        QGLSection::appendFaceted(vertex);
    }

    void append(const QLogicalVertex &vertex)
    {
        QGLSection::append(vertex);
    }
};

class TestBuilder : public QGLBuilder
{
public:
    QGLSection *currentSection() { return QGLBuilder::currentSection(); }
    QList<QGLSection*> sections() { return QGLBuilder::sections(); }
};

static void addGeometry(TestBuilder &builder)
{
    QVector3D va(-1.0f, -1.0f, 0.0f);
    QVector3D vb(1.0f, -1.0f, 0.0f);
    QVector3D vc(1.0f, 1.0f, 0.0f);
    QGeometryData p;
    p.appendVertex(va, vb, vc);
    builder.addTriangles(p);
}

void tst_QGLSection::create()
{
    // Test that a newly created object has the correct defaults.
    TestBuilder builder;
    builder.newSection();
    QGLSection *section = builder.currentSection();
    QVERIFY(section->hasField(QGL::Position));   // need this initially now
    QCOMPARE(section->fields(), (quint32)1);
    QCOMPARE(section->smoothing(), QGL::Smooth);
    QCOMPARE(section->count(), 0);
    QCOMPARE(section->boundingBox(), QBox3D());
    QCOMPARE(section->mapThreshold(), 5);

    // suppress warning
    addGeometry(builder);
    builder.finalizedSceneNode();
}

void tst_QGLSection::modify()
{
    TestBuilder builder;
    builder.newSection();
    QGLSection *section = builder.currentSection();

    QVector3D va(-1.0f, -1.0f, 0.0f);
    QVector3D vb(1.0f, -1.0f, 0.0f);
    QVector3D vc(1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QGeometryData p;
    p.appendVertex(va, vb, vc);
    p.appendNormal(n, n, n);
    section->setMapThreshold(30);
    QCOMPARE(section->mapThreshold(), 30);
    builder.addTriangles(p);
    QCOMPARE(section->count(), 3);
    builder.newSection();
    QGLSection *section2 = builder.currentSection();
    builder.addTriangles(p);
    QCOMPARE(section2->count(), 3);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::append()
{
    TestBuilder builder;
    builder.newSection();
    QGLSection *section = builder.currentSection();

    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(1.0f, 0.0f, 0.0f);
    QLogicalVertex vx(testVertex, testNormal);
    section->append(vx);
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->indices().size(), 1);
    QCOMPARE(section->texCoords().count(), 0);
    QCOMPARE(section->colors().count(), 0);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::appendSmooth()
{
    TestBuilder builder;
    QGLSectionTest *section = new QGLSectionTest(&builder);
    testSmooth(section, &builder);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::appendSmoothMap()
{
    TestBuilder builder;
    QGLSectionTest *section = new QGLSectionTest(&builder);
    int t = section->mapThreshold();
    QVector3D testVertex(-12.34f, -23.45f, -34.56f);
    QVector3D incrVector(0.02, 0.02, 0.02);
    QVector3D testNormal(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < (t - 2); ++i)
    {
        section->appendSmooth(QLogicalVertex(testVertex, testNormal));
        testVertex += incrVector;
    }
    testSmooth(section, &builder);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::testSmooth(QGLSection *section, QGLBuilder *builder)
{
    int poffset = section->count(QGL::Position);
    int noffset = section->count(QGL::Normal);

    // append a vertex - check it appears in the data along with its normal
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal));
    QCOMPARE(section->vertices().count(), poffset + 1);
    QCOMPARE(section->vertices().at(poffset + 0), testVertex);
    QCOMPARE(section->normals().count(), noffset + 1);
    QCOMPARE(section->normals().at(noffset + 0), testNormal);
    QCOMPARE(section->indices().size(), poffset + 1);
    QCOMPARE_INDEX(section->indices()[poffset + 0], poffset + 0);

    // append a vertex equal to one already appended - check it was coalesced
    QVector3D testNormal2(0.0f, 0.0f, 1.0f);
    QVector3D result = testNormal + testNormal2; // QVector3D(0.0f, 0.0f, 7.543f);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), poffset + 1);
    QCOMPARE(section->vertices().at(poffset + 0), testVertex);
    QCOMPARE(section->normals().count(), noffset + 1);
    QCOMPARE(section->normals().at(noffset + 0), result);
    QCOMPARE(section->indices().size(), poffset + 2);
    QCOMPARE_INDEX(section->indices()[poffset + 1], poffset + 0);

    // append a new  different vertex - check it is not coalesced
    QVector3D testVertex2(-1.234f, -2.345f, -3.456f);
    QVector3D testNormal3(-4.321f, -5.432f, -6.543f);
    section->appendSmooth(QLogicalVertex(testVertex2, testNormal3));
    QCOMPARE(section->vertices().count(), poffset + 2);
    QCOMPARE(section->vertices().at(poffset + 1), testVertex2);
    QCOMPARE(section->normals().count(), noffset + 2);
    QCOMPARE(section->normals().at(noffset + 1), testNormal3);
    QCOMPARE(section->indices().size(), poffset + 3);
    QCOMPARE_INDEX(section->indices()[poffset + 2], poffset + 1);

    // append a vertex equal to one already appended, but inside a new section - check its not coalesced
    section = new QGLSectionTest(builder);
    section->appendSmooth(QLogicalVertex(testVertex2, testNormal3));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex2);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal3);
    QCOMPARE(section->indices().size(), 1);
    QCOMPARE_INDEX(section->indices()[0], 0);
}

void tst_QGLSection::appendFaceted()
{
    TestBuilder builder;
    QGLSectionTest *section = new QGLSectionTest(&builder);
    // test the part where its only using the QArray
    testFaceted(section, &builder);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::appendFacetedMap()
{
    TestBuilder builder;
    QGLSectionTest *section = new QGLSectionTest(&builder);
    // now create a new section and fill to just below the threshold for QMap
    int t = section->mapThreshold();
    QVector3D testVertex(-12.34f, -23.45f, -34.56f);
    QVector3D incrVector(0.02f, 0.02f, 0.02f);
    QVector3D testNormal(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < (t - 2); ++i)
    {
        section->appendSmooth(QLogicalVertex(testVertex, testNormal));
        testVertex += incrVector;
    }
    testFaceted(section, &builder);

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::accumNormals()
{
    /*
    This mesh is a cap that could be the top of a sphere - its a central
    quad in the z = 1 plane, with x = 1, y = 1 as the bottom-left corner;
    with four more quads above, below, to the left and to the right, as
    viewed looking down the -ve z axis (plan view).  The four quads around
    the central one join one one edge (in the z = 1 plane) and on the opp.
    edge slope down to the z = 0 plane.  The four corners are filled in
    with triangles, so that the whole figure looks like an octagon in plan.

      plan view - x/y plane:
    ^       ----------
    |      / |     |  \
    y     /  |     |   \
         /___|_____|____\
        |    |     |     |
        |    |     |     |
        |____|_____|_____|
         \   |*    |    /
          \  |     |   /
           \ |     |  /
            ----------       -->  x

      side view - x/z plane:
            ________          z = 1
          / |*    | \
         /  |     |  \
        /   |     |   \
       -----------------      z = 0

    The normals smoothing should make this as "round" as possible.

    Each quad of course, is actually divided from bottom-left to top-right
    into two triangles (not shown in the ascii art for clarity).  The total
    number of triangles in the rendered result should be 14.

    At the vertex marked with the asterisk - #3 in the data below - there
    are 5 triangular faces contributing normals to the smoothed sum. The
    quad on the top has two co-planar triangles, and _each_ of those has
    a normal equal to (0, 0, 1) - only one of these should be summed.
    The other triangles are 1/2 of the two neighbouring quads - one has a
    normal of (-1, 0, 1) the other (0, -1, 1) - and the corner triangle
    which has a normal of (-1, -1, 1).

    The normalized sum of these is roughly (-0.4, -0.4, 0.8).

    If the normal from the extra triangle is erroneously included then
    the normalized sum is roughly (-0.35, -0.35, 0.87).

    The resulting normal is pulled more in the z-direction.

    In general if a a flat plane is broken up into a large number of
    triangles, they can bias the normal calculation.  In a model often
    this can occur where triangulation algorithms produce degenerate
    solutions with many triangles converging on a point.

    This test is to cover this problem.
    */
    TestBuilder builder;
    builder.newSection(QGL::Smooth); // default - but making the point
    //QGLSection *section = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();
    static float data[12*3] = {
        1.0f, 0.0f, 0.0f,
        2.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        2.0f, 1.0f, 1.0f,
        3.0f, 1.0f, 0.0f,
        0.0f, 2.0f, 0.0f,
        1.0f, 2.0f, 1.0f,
        2.0f, 2.0f, 1.0f,
        3.0f, 2.0f, 0.0f,
        1.0f, 3.0f, 0.0f,
        2.0f, 3.0f, 0.0f
    };
    QVector3DArray v =  QVector3DArray::fromRawData((const QVector3D *)data, 12);
    QGeometryData quads;
    quads.appendVertex(v[0], v[1], v[4], v[3]);      // 0
    quads.appendVertex(v[2], v[3], v[7], v[6]);      // 1
    quads.appendVertex(v[3], v[4], v[8], v[7]);      // 2
    quads.appendVertex(v[4], v[5], v[9], v[8]);      // 3
    quads.appendVertex(v[7], v[8], v[11], v[10]);    // 4
    builder.addQuads(quads);
    QGeometryData triangles;
    triangles.appendVertex(v[0], v[3], v[2]);        // 5
    triangles.appendVertex(v[1], v[5], v[4]);        // 6
    triangles.appendVertex(v[6], v[7], v[10]);       // 7
    triangles.appendVertex(v[8], v[9], v[11]);       // 8
    builder.addTriangles(triangles);
    builder.finalizedSceneNode();

    // There are 9 faces as shown above - here are their normals
    QVector3DArray face_norms;
    face_norms.extend(9);
    face_norms[0] = QVector3D(0, -1, 1);
    face_norms[1] = QVector3D(-1, 0, 1);
    face_norms[2] = QVector3D(0, 0, 1);
    face_norms[3] = QVector3D(1, 0, 1);
    face_norms[4] = QVector3D(0, 1, 1);
    face_norms[5] = QVector3D(-1, -1, 1);
    face_norms[6] = QVector3D(1, -1, 1);
    face_norms[7] = QVector3D(-1, 1, 1);
    face_norms[8] = QVector3D(1, 1, 1);
    //for (int i = 0; i < 9; ++i)
    //    face_norms[i].normalize();

    QVector3DArray expected;
    expected.extend(12);
    expected[0] = face_norms[0] + face_norms[5];
    expected[1] = face_norms[0] + face_norms[6];
    expected[4] = face_norms[1] + face_norms[5];
    expected[3] = face_norms[0] + face_norms[1] + face_norms[2] + face_norms[5];
    expected[2] = face_norms[0] + face_norms[2] + face_norms[3] + face_norms[6];
    expected[8] = face_norms[3] + face_norms[6];
    expected[6] = face_norms[1] + face_norms[7];
    expected[5] = face_norms[1] + face_norms[2] + face_norms[4] + face_norms[7];
    expected[7] = face_norms[2] + face_norms[3] + face_norms[4] + face_norms[8];
    expected[9] = face_norms[3] + face_norms[8];
    expected[11] = face_norms[4] + face_norms[7];
    expected[10] = face_norms[4] + face_norms[8];
    for (int i = 0; i < 12; ++i)
        expected[i].normalize();

    QGeometryData res = node->geometry();
    QCOMPARE(res.count(QGL::Position), 12);
    QCOMPARE(res.count(QGL::Normal), 12);
    QCOMPARE(res.indexCount(), 14 * 3);
    QCOMPARE(res.vertexAt(0), v[0]);
    QCOMPARE(res.vertexAt(4), v[2]);
    QCOMPARE(res.vertexAt(7), v[8]);
    QCOMPARE(res.vertexAt(11), v[10]);
    QCOMPARE(res.normalAt(0), expected.at(0));
    QCOMPARE(res.normalAt(1), expected.at(1));
    QCOMPARE(res.normalAt(2), expected.at(2));
    QCOMPARE(res.normalAt(3), expected.at(3));
    QCOMPARE(res.normalAt(4), expected.at(4));
    QCOMPARE(res.normalAt(5), expected.at(5));
    QCOMPARE(res.normalAt(6), expected.at(6));
    QCOMPARE(res.normalAt(7), expected.at(7));
    QCOMPARE(res.normalAt(8), expected.at(8));
    QCOMPARE(res.normalAt(9), expected.at(9));
    QCOMPARE(res.normalAt(10), expected.at(10));
    QCOMPARE(res.normalAt(11), expected.at(11));
}

void tst_QGLSection::normalizedNormals()
{
    const float qRadius = 1.0f;
    const float qHeight = 0.6f;
    const float qThickness = 0.4f;
    const int qNumSlices = 16;

    TestBuilder builder;
    builder.newSection(QGL::Smooth); // default - but making the point
    //QGLSection *section = builder.currentSection();
    QGLSceneNode *node = builder.currentNode();

    // draw two 90 degree arcs, qThickness apart radially, and qHeight apart
    // in the z dimension.  the points on the two arcs are joined into quads
    // but all the points on the high arc are joined to one single point in
    // the same z plane.
    QVector3D ap(qRadius, 0.0f, 0.0f);
    QVector3D bp(qRadius + qThickness, 0.0f, qHeight);
    QVector3D anc(qRadius * 2.0f, qRadius *2.0f, qHeight);
    for (int slice = 1; slice < qNumSlices; ++slice)
    {
        const float pi2 = 2.0f * M_PI;
        float angle = (slice * pi2) / (4 * qNumSlices);
        float cs = cosf(angle);
        float sn = sinf(angle);
        QVector3D a(cs * qRadius, sn * qRadius, 0.0f);
        QVector3D b(cs * (qRadius + qThickness), sn * (qRadius + qThickness), qHeight);
        QGeometryData quad;
        quad.appendVertex(ap, bp, b, a);
        builder.addQuads(quad);
        QGeometryData tri;
        tri.appendVertex(b, bp, anc);
        builder.addTriangles(tri);
        ap = a;
        bp = b;
    }
    builder.finalizedSceneNode();
    QGeometryData data = node->geometry();
    QVERIFY(!data.isNull());
    QCOMPARE(data.count(QGL::Position), 33);
}

void tst_QGLSection::testFaceted(QGLSection *section, QGLBuilder *builder)
{
    int poffset = section->count(QGL::Position);
    int noffset = section->count(QGL::Normal);

    // append a vertex - check it appears in the data along with its normal
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    section->appendFaceted(QLogicalVertex(testVertex, testNormal));
    QCOMPARE(section->vertices().count(), poffset + 1);
    QCOMPARE(section->vertices().at(poffset + 0), testVertex);
    QCOMPARE(section->normals().count(), noffset + 1);
    QCOMPARE(section->normals().at(noffset + 0), testNormal);
    QCOMPARE(section->indices().size(), poffset + 1);
    QCOMPARE_INDEX(section->indices()[poffset + 0], poffset + 0);

    // append a vertex equal to one already appended, but with different normal - check it was NOT coalesced
    QVector3D testNormal2(0.0f, 0.0f, 1.0f);
    section->appendFaceted(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), poffset + 2);
    QCOMPARE(section->vertices().at(poffset + 1), testVertex);
    QCOMPARE(section->normals().count(), noffset + 2);
    QCOMPARE(section->normals().at(noffset + 1), testNormal2);
    QCOMPARE(section->indices().size(), poffset + 2);
    QCOMPARE_INDEX(section->indices()[poffset + 1], poffset + 1);

    // append a vertex equal to one already appended, but with same normal - check it WAS coalesced
    section->appendFaceted(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), poffset + 2);
    QCOMPARE(section->vertices().at(poffset + 1), testVertex);
    QCOMPARE(section->normals().count(), noffset + 2);
    QCOMPARE(section->normals().at(noffset + 1), testNormal2);
    QCOMPARE(section->indices().size(), poffset + 3);
    QCOMPARE_INDEX(section->indices()[poffset + 2], poffset + 1);

    // append a vertex equal to one already appended, with same normal, BUT in a new section - check it was NOT coalesced
    section = new QGLSectionTest(builder);
    section->appendFaceted(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal2);
    QCOMPARE(section->indices().size(), 1);
    QCOMPARE_INDEX(section->indices()[0], 0);
}

void tst_QGLSection::appendTexCoord()
{
    TestBuilder builder;
    QGLSectionTest *section = new QGLSectionTest(&builder);

    // note that the tests above do the case of texCoord, InvalidTexCoord

    // append a texture - check it appears in the data at the correct index
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    QVector2D testTexCoord(0.0f, 0.0f);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->indices().size(), 1);
    QCOMPARE_INDEX(section->indices()[0], 0);

    // append same texture - will coalesce and index the vert
    section->appendSmooth(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->indices().size(), 2);
    QCOMPARE_INDEX(section->indices()[1], 0);

    // new vertex created to carry the updated texture coord, even though
    // the normal and vertex are the same and thus the logical vert would
    // otherwise coalesced - the new vert is a copy (same vert and normal)
    QVector2D testTexCoord2(1.0, 1.0);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal, testTexCoord2));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal);
    QCOMPARE(section->texCoords().count(), 2);
    QCOMPARE(section->texCoords().at(1), testTexCoord2);
    QCOMPARE(section->indices().size(), 3);
    QCOMPARE_INDEX(section->indices()[2], 1);

    section = new QGLSectionTest(&builder);

    // append a faceted vertex with a tex coord check it appears in the data
    // in a new section now, so the same vert and normal wont be coalesced
    section->appendFaceted(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->indices().size(), 1);
    QCOMPARE_INDEX(section->indices()[0], 0);

    // append a vertex & normal equal to one already appended, but with different tex coord
    // check it was NOT coalesced, dup vert created
    section->appendFaceted(QLogicalVertex(testVertex, testNormal, testTexCoord2));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal);
    QCOMPARE(section->texCoords().count(), 2);
    QCOMPARE(section->texCoords().at(1), testTexCoord2);
    QCOMPARE(section->indices().size(), 2);
    QCOMPARE_INDEX(section->indices()[1], 1);

    // append a vertex equal to first one appended above, with same normal, and
    // same texture - check it WAS coalesced to index 0
    section->appendFaceted(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal);
    QCOMPARE(section->texCoords().count(), 2);
    QCOMPARE(section->texCoords().at(1), testTexCoord2);
    QCOMPARE(section->indices().size(), 3);
    QCOMPARE_INDEX(section->indices()[2], 0);

    QVERIFY(section->hasField(QGL::Normal));
    QVERIFY(section->hasField(QGL::TextureCoord0));
    QVERIFY(!section->hasField(QGL::Color));

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::appendColor()
{
    TestBuilder builder;
    QGLSectionTest *section = new QGLSectionTest(&builder);

    QColor4ub color(32, 64, 128, 255);
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    section->append(QLogicalVertex(testVertex, color));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->colors().count(), 1);
    QCOMPARE(section->colors().at(0), color);

    QVERIFY(section->hasField(QGL::Color));
    QVERIFY(!section->hasField(QGL::TextureCoord0));
    QVERIFY(!section->hasField(QGL::Normal));

    section = new QGLSectionTest(&builder);

    QVector2D testTexCoord(0.0f, 0.0f);
    QLogicalVertex lv;
    lv.setVertex(testVertex);
    lv.setColor(color);
    lv.setTexCoord(testTexCoord);
    section->append(lv);
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->colors().count(), 1);
    QCOMPARE(section->colors().at(0), color);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);

    QVERIFY(section->hasField(QGL::Color));
    QVERIFY(section->hasField(QGL::TextureCoord0));
    QVERIFY(!section->hasField(QGL::Normal));

    // suppress warning
    builder.finalizedSceneNode();
}

void tst_QGLSection::accessors()
{
    TestBuilder builder;
    QGLSectionTest *section = new QGLSectionTest(&builder);

    QCOMPARE(section->smoothing(), QGL::Smooth);

    QGLSectionTest *section2 = new QGLSectionTest(&builder, QGL::Faceted);
    QCOMPARE(section2->smoothing(), QGL::Faceted);

    // suppress warning
    builder.finalizedSceneNode();
}

QTEST_APPLESS_MAIN(tst_QGLSection)

#include "tst_qglsection.moc"
