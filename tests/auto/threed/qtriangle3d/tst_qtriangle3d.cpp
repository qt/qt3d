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
#include "qtriangle3d.h"
#include "qplane3d.h"

class tst_QTriangle3D : public QObject
{
    Q_OBJECT
public:
    tst_QTriangle3D() {}
    ~tst_QTriangle3D() {}

private slots:
    void defaultTriangle();

    void create_data();
    void create();

    void contains_data();
    void contains();

    void intersect_data();
    void intersect();

    void uv_data();
    void uv();

    void transform_data();
    void transform();

    void compare();

    void dataStream();

    void properties();
    void metaTypes();
};

void tst_QTriangle3D::defaultTriangle()
{
    QTriangle3D triangle;
    QCOMPARE(triangle.p(), QVector3D(0.0f, 0.0f, 0.0f));
    QCOMPARE(triangle.q(), QVector3D(1.0f, 0.0f, 0.0f));
    QCOMPARE(triangle.r(), QVector3D(0.0f, 1.0f, 0.0f));
}

void tst_QTriangle3D::create_data()
{
    QTest::addColumn<QVector3D>("p");
    QTest::addColumn<QVector3D>("q");
    QTest::addColumn<QVector3D>("r");
    QTest::addColumn<bool>("isDegenerate");
    QTest::addColumn<QPlane3D>("plane");
    QTest::addColumn<QVector3D>("centroid");

    QTest::newRow("minimal at null")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 0.0f);
    QTest::newRow("minimal at offset")
         << QVector3D(2.0f, 2.0f, 2.0f) // p
         << QVector3D(2.0f, 2.0f, 2.0f) // q
         << QVector3D(2.0f, 2.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(2.0f, 2.0f, 2.0f); // centroid
    QTest::newRow("line from 0(p) [a]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.5f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.5f/3.0f); // centroid
    QTest::newRow("line from 0(p) [b]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 1.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 1.0f); // centroid
    QTest::newRow("line from 0(p) [c]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 1.5f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 3.5f/3.0f); // centroid
    QTest::newRow("line from 0(q) [a]")
         << QVector3D(0.0f, 0.0f, 0.5f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.5f/3.0f); // centroid
    QTest::newRow("line from 0(q) [b]")
         << QVector3D(0.0f, 0.0f, 1.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 1.0f); // centroid
    QTest::newRow("line from 0(q) [c]")
         << QVector3D(0.0f, 0.0f, 1.5f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 3.5f/3.0f); // centroid
    QTest::newRow("line from 0(r) [a]")
         << QVector3D(0.0f, 0.0f, 0.5f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.5f/3.0f); // centroid
    QTest::newRow("line from 0(r) [b]")
         << QVector3D(0.0f, 0.0f, 1.0f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 1.0f); // centroid
    QTest::newRow("line from 0(r) [c]")
         << QVector3D(0.0f, 0.0f, 1.5f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 3.5f/3.0f); // centroid
    QTest::newRow("width 0 [a]")
         << QVector3D(0.0f, 0.0f, 2.0f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 4.0f/3.0f); // centroid
    QTest::newRow("width 0 [b]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.0f/3.0f); // centroid
    QTest::newRow("width 0 [c]")
         << QVector3D(0.0f, 0.0f, 2.0f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 4.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 8.0f/3.0f); // centroid

    /*  The above only tests degenerate triangles colinear with the z axis.
        May also want to tests degenerate triangles offset from origin and
        degenerate triangles on other angles (e.g. x axis, y axis, not colinear
        with any axis)
        */

    QTest::newRow("simple at origin")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 4.0f, 0.0f) // q
         << QVector3D(0.0f, 2.0f, 5.0f) // r
         << false // degenerate
         << QPlane3D(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)) // plane
         << QVector3D(0.0f, 6.0f/3.0f, 5.0f/3.0f); // centroid

    QTest::newRow("simple offset from origin")
         << QVector3D(1.0f, 1.0f, 0.0f) // p
         << QVector3D(1.0f, 5.0f, 0.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << false // degenerate
         << QPlane3D(QVector3D(1.0f, 0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)) // plane
         << QVector3D(1.0f, 3.0f, 5.0f/3.0f); // centroid
}

void tst_QTriangle3D::create()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);
    QFETCH(bool, isDegenerate);
    QFETCH(QPlane3D, plane);
    QFETCH(QVector3D, centroid);

    QTriangle3D triangle(p, q, r);

    QCOMPARE(triangle.p(), p);
    QCOMPARE(triangle.q(), q);
    QCOMPARE(triangle.r(), r);

    QCOMPARE(triangle.faceNormal(), QVector3D::crossProduct(q - p, r - q));

    if (!isDegenerate) {
        QPlane3D trianglePlane = triangle.plane();
        QVERIFY(plane.contains(trianglePlane.origin()));
        QCOMPARE(plane.normal().normalized(),
                 trianglePlane.normal().normalized());
    }
    QCOMPARE(triangle.center(), centroid);

    QTriangle3D triangle2;
    triangle2.setP(p);
    triangle2.setQ(q);
    triangle2.setR(r);
    QCOMPARE(triangle2.p(), p);
    QCOMPARE(triangle2.q(), q);
    QCOMPARE(triangle2.r(), r);
    QVERIFY(triangle == triangle2);
}

void tst_QTriangle3D::contains_data()
{
    QTest::addColumn<QVector3D>("p");
    QTest::addColumn<QVector3D>("q");
    QTest::addColumn<QVector3D>("r");
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<bool>("contains");

    QTest::newRow("minimal at null, same")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << QVector3D(0.0f, 0.0f, 0.0f) // point
         << false; // contains
    QTest::newRow("acute at p")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.0f, 2.0f) // point
         << true; // contains
    QTest::newRow("acute at q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.0f, 4.0f) // point
         << true; // contains
    QTest::newRow("acute at r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 3.0f, 3.0f) // point
         << true; // contains
    QTest::newRow("acute at p-q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.0f, 3.0f) // point
         << true; // contains
    QTest::newRow("acute at p-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.5f, 2.5f) // point
         << true; // contains
    QTest::newRow("acute at q-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.5f, 3.5f) // point
         << true; // contains
    QTest::newRow("acute internal")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.5f, 3.0f) // point
         << true; // contains
    QTest::newRow("acute external a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.8f, 2.0f) // point
         << false; // contains
    QTest::newRow("acute external b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.8f, 4.0f) // point
         << false; // contains
    QTest::newRow("acute external c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 1.4f, 3.0f) // point
         << false; // contains
    QTest::newRow("acute above plane")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.3f, 2.5f, 3.0f) // point
         << false; // contains
    QTest::newRow("acute below plane")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.8f, 2.5f, 3.0f) // point
         << false; // contains
    QTest::newRow("obtuse a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.0f, 2.5f, 4.8f) // point
         << false; // contains
    QTest::newRow("obtuse b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.0f, 2.5f, 4.3f) // point
         << true; // contains
    QTest::newRow("obtuse c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.0f, 2.5f, 4.5f) // point
         << true; // contains
    QTest::newRow("obtuse d")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.2f, 2.5f, 4.3f) // point
         << false; // contains
    QTest::newRow("obtuse e")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.8f, 2.5f, 4.3f) // point
         << false; // contains
}

void tst_QTriangle3D::contains()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);
    QFETCH(QVector3D, point);
    QFETCH(bool, contains);
    // qDebug("Triangle: P(%0.10f, %0.10f, %0.10f) Q(%0.10f, %0.10f, %0.10f) R(%0.10f, %0.10f, %0.10f)",
    //       p.x(), p.y(), p.z(), q.x(), q.y(), q.z(), r.x(), r.y(), r.z());
    // qDebug("Point: (%0.10f, %0.10f, %0.10f)", point.x(), point.y(), point.z());
    QCOMPARE(QTriangle3D(p, q, r).contains(point), contains);
}

void tst_QTriangle3D::intersect_data()
{
    QTest::addColumn<QVector3D>("p");
    QTest::addColumn<QVector3D>("q");
    QTest::addColumn<QVector3D>("r");
    QTest::addColumn<QVector3D>("origin");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("intersection");
    QTest::addColumn<bool>("doesIntersect");

    QTest::newRow("minimal at null, same")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << QVector3D(0.0f, 0.0f, 0.0f) // origin
         << QVector3D(0.0f, 0.0f, 1.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // degenerate triangles cannot intersect

    /*
        lines perpendicular to triangle plane.
    */
    QTest::newRow("acute at p")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.0f, 2.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.0f, 2.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.0f, 4.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.0f, 4.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 3.0f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 3.0f, 3.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at p-q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.0f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.0f, 3.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at p-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.5f, 2.5f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 2.5f) // intersection
         << true; // intersects
    QTest::newRow("acute at q-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.5f, 3.5f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 3.5f) // intersection
         << true; // intersects
    QTest::newRow("acute internal")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.5f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 3.0f) // intersection
         << true; // intersects
    QTest::newRow("acute external a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.8f, 2.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("acute external b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.8f, 4.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("acute external c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 1.4f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("obtuse a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.0f, 2.5f, 4.8f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("obtuse b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.0f, 2.5f, 4.3f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 4.3f) // intersection
         << true; // intersects
    QTest::newRow("obtuse c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.0f, 2.5f, 4.5f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 4.5f) // intersection
         << true; // intersects

    QTest::newRow("in-triangle-plane")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.0f, 2.0f, 2.0f) // origin (p)
         << QVector3D(0.0f, 0.0f, 2.0f) // direction (q - p)
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
}

void tst_QTriangle3D::intersect()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);
    QFETCH(QVector3D, origin);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, intersection);
    QFETCH(bool, doesIntersect);

    QTriangle3D triangle(p, q, r);
    QRay3D line(origin, direction);
    QCOMPARE(triangle.intersects(line), doesIntersect);

    float result = triangle.intersection(line);
    if (doesIntersect)
        QCOMPARE(line.point(result), intersection);
    else
        QVERIFY(qIsNaN(result));
}

void tst_QTriangle3D::uv_data()
{
    contains_data();
}

void tst_QTriangle3D::uv()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);

    if (p == q || p == r || q == r)
        return;     // Ignore degenerate triangles for this test.

    QTriangle3D triangle(p, q, r);

    QCOMPARE(triangle.uv(p), QVector2D(1, 0));
    QCOMPARE(triangle.uv(q), QVector2D(0, 1));
    QCOMPARE(triangle.uv(r), QVector2D(0, 0));

    QCOMPARE(triangle.uv((p + q) / 2.0f), QVector2D(0.5f, 0.5f));
    QCOMPARE(triangle.uv((p + r) / 2.0f), QVector2D(0.5f, 0.0f));
    QCOMPARE(triangle.uv((q + r) / 2.0f), QVector2D(0.0f, 0.5f));

    QVector2D v1(triangle.uv((p + q + r) / 3.0f));
    QVector2D v2(1.0f / 3.0f, 1.0f / 3.0f);
    QVERIFY(qFuzzyCompare(float(v1.x()), float(v2.x())));
    QVERIFY(qFuzzyCompare(float(v1.y()), float(v2.y())));
}

void tst_QTriangle3D::transform_data()
{
    create_data();
}

void tst_QTriangle3D::transform()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);

    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);

    QTriangle3D tri1(p, q, r);
    QTriangle3D tri2(tri1);
    QTriangle3D tri3;

    tri1.transform(m);
    tri3 = tri2.transformed(m);

    QCOMPARE(tri1.p(), tri3.p());
    QCOMPARE(tri1.q(), tri3.q());
    QCOMPARE(tri1.r(), tri3.r());

    QCOMPARE(tri1.p(), m * p);
    QCOMPARE(tri1.q(), m * q);
    QCOMPARE(tri1.r(), m * r);
}

void tst_QTriangle3D::compare()
{
    QVector3D p1(1.0f, 2.0f, 3.0f);
    QVector3D q1(4.0f, 5.0f, 6.0f);
    QVector3D r1(7.0f, 8.0f, 9.0f);
    QVector3D p2(-1.0f, -2.0f, -3.0f);
    QVector3D q2(-4.0f, -5.0f, -6.0f);
    QVector3D r2(-7.0f, -8.0f, -9.0f);

    QTriangle3D tri1(p1, q1, r1);
    QTriangle3D tri2(p1, q1, r1);
    QVERIFY(tri1 == tri2);
    QVERIFY(!(tri1 != tri2));
    QVERIFY(qFuzzyCompare(tri1, tri2));

    QTriangle3D tri3(p2, q1, r1);
    QVERIFY(tri1 != tri3);
    QVERIFY(!(tri1 == tri3));
    QVERIFY(!qFuzzyCompare(tri1, tri3));

    QTriangle3D tri4(p1, q2, r1);
    QVERIFY(tri1 != tri4);
    QVERIFY(!(tri1 == tri4));
    QVERIFY(!qFuzzyCompare(tri1, tri4));

    QTriangle3D tri5(p1, q1, r2);
    QVERIFY(tri1 != tri5);
    QVERIFY(!(tri1 == tri5));
    QVERIFY(!qFuzzyCompare(tri1, tri5));

    QTriangle3D tri6(p2, q2, r2);
    QVERIFY(tri1 != tri6);
    QVERIFY(!(tri1 == tri6));
    QVERIFY(!qFuzzyCompare(tri1, tri6));
}

void tst_QTriangle3D::dataStream()
{
#ifndef QT_NO_DATASTREAM
    QTriangle3D triangle(QVector3D(1.0f, 2.0f, 3.0f),
                         QVector3D(4.0f, 5.0f, 6.0f),
                         QVector3D(7.0f, 8.0f, 9.0f));

    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << triangle;
    }

    QTriangle3D triangle2;
    {
        QDataStream stream2(data);
        stream2 >> triangle2;
    }

    QVERIFY(triangle == triangle2);
#endif
}

class tst_QTriangle3DProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QTriangle3D triangle READ triangle WRITE setTriangle)
public:
    tst_QTriangle3DProperties(QObject *parent = 0) : QObject(parent) {}

    QTriangle3D triangle() const { return t; }
    void setTriangle(const QTriangle3D& value) { t = value; }

private:
    QTriangle3D t;
};

// Test getting and setting properties via the metaobject system.
void tst_QTriangle3D::properties()
{
    tst_QTriangle3DProperties obj;

    qRegisterMetaType<QTriangle3D>();

    obj.setTriangle(QTriangle3D(QVector3D(1, 2, 3), QVector3D(4, 5, 6),
                                QVector3D(7, 8, 9)));

    QTriangle3D t = qvariant_cast<QTriangle3D>(obj.property("triangle"));
    QCOMPARE(t.p(), QVector3D(1, 2, 3));
    QCOMPARE(t.q(), QVector3D(4, 5, 6));
    QCOMPARE(t.r(), QVector3D(7, 8, 9));

    obj.setProperty("triangle",
                    qVariantFromValue
                        (QTriangle3D(QVector3D(-1, -2, -3),
                                     QVector3D(-4, -5, -6),
                                     QVector3D(-7, -8, -9))));

    t = qvariant_cast<QTriangle3D>(obj.property("triangle"));
    QCOMPARE(t.p(), QVector3D(-1, -2, -3));
    QCOMPARE(t.q(), QVector3D(-4, -5, -6));
    QCOMPARE(t.r(), QVector3D(-7, -8, -9));
}

void tst_QTriangle3D::metaTypes()
{
    int id = qMetaTypeId<QTriangle3D>();
    QVERIFY(QMetaType::type("QTriangle3D") == id);
    QCOMPARE(QByteArray(QMetaType::typeName(id)), QByteArray("QTriangle3D"));
    QVERIFY(QMetaType::isRegistered(id));
}

QTEST_APPLESS_MAIN(tst_QTriangle3D)

#include "tst_qtriangle3d.moc"
