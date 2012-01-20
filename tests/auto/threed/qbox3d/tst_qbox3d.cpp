/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "qbox3d.h"
#include <QtGui/qmatrix4x4.h>

class tst_QBox3D : public QObject
{
    Q_OBJECT
public:
    tst_QBox3D() {}
    ~tst_QBox3D() {}

private slots:
    void create();
    void modify();
    void compare();
    void fuzzyCompare();
    void size_data();
    void size();
    void center_data();
    void center();
    void containsPoint_data();
    void containsPoint();
    void containsBox_data();
    void containsBox();
    void intersects_data();
    void intersects();
    void intersect_data();
    void intersect();
    void intersected_data();
    void intersected();
    void intersectRay_data();
    void intersectRay();
    void unitePoint_data();
    void unitePoint();
    void uniteBox_data();
    void uniteBox();
    void unitedPoint_data();
    void unitedPoint();
    void unitedBox_data();
    void unitedBox();
    void transform();
    void transformed();
    void dataStream();
    void properties();
    void metaTypes();
};

void tst_QBox3D::create()
{
    QBox3D box1;
    QVERIFY(box1.isNull());
    QVERIFY(!box1.isFinite());
    QVERIFY(!box1.isInfinite());
    QVERIFY(box1.minimum() == QVector3D(0, 0, 0));
    QVERIFY(box1.maximum() == QVector3D(0, 0, 0));

    QBox3D box2;
    box2.setToInfinite();
    QVERIFY(!box2.isNull());
    QVERIFY(!box2.isFinite());
    QVERIFY(box2.isInfinite());
    QVERIFY(box2.minimum() == QVector3D(0, 0, 0));
    QVERIFY(box2.maximum() == QVector3D(0, 0, 0));

    QBox3D box3(QVector3D(1, 2, 3), QVector3D(4, 5, 6));
    QVERIFY(!box3.isNull());
    QVERIFY(box3.isFinite());
    QVERIFY(!box3.isInfinite());
    QVERIFY(box3.minimum() == QVector3D(1, 2, 3));
    QVERIFY(box3.maximum() == QVector3D(4, 5, 6));

    QBox3D box4(QVector3D(4, 5, 6), QVector3D(1, 2, 3));
    QVERIFY(!box4.isNull());
    QVERIFY(box4.isFinite());
    QVERIFY(!box4.isInfinite());
    QVERIFY(box4.minimum() == QVector3D(1, 2, 3));
    QVERIFY(box4.maximum() == QVector3D(4, 5, 6));

    QBox3D box5(box4);
    QVERIFY(!box5.isNull());
    QVERIFY(box5.isFinite());
    QVERIFY(!box5.isInfinite());
    QVERIFY(box5.minimum() == QVector3D(1, 2, 3));
    QVERIFY(box5.maximum() == QVector3D(4, 5, 6));

    box5 = box2;
    QVERIFY(!box5.isNull());
    QVERIFY(!box5.isFinite());
    QVERIFY(box5.isInfinite());
    QVERIFY(box5.minimum() == QVector3D(0, 0, 0));
    QVERIFY(box5.maximum() == QVector3D(0, 0, 0));
}

void tst_QBox3D::modify()
{
    // Use setExtents, which will swap minimum and maximum.
    QBox3D box;
    box.setExtents(QVector3D(-1, -2, -3), QVector3D(-5, -6, -7));
    QVERIFY(!box.isNull());
    QVERIFY(box.isFinite());
    QVERIFY(!box.isInfinite());
    QVERIFY(box.minimum() == QVector3D(-5, -6, -7));
    QVERIFY(box.maximum() == QVector3D(-1, -2, -3));

    box.setExtents(QVector3D(1, 2, 3), QVector3D(4, 5, 6));
    QVERIFY(!box.isNull());
    QVERIFY(box.isFinite());
    QVERIFY(!box.isInfinite());
    QVERIFY(box.minimum() == QVector3D(1, 2, 3));
    QVERIFY(box.maximum() == QVector3D(4, 5, 6));

    box.setToNull();
    QVERIFY(box.isNull());
    QVERIFY(!box.isFinite());
    QVERIFY(!box.isInfinite());
    QVERIFY(box.minimum() == QVector3D(0, 0, 0));
    QVERIFY(box.maximum() == QVector3D(0, 0, 0));

    // Using setExtents will turn a null box into a finite box.
    box.setExtents(QVector3D(-1, -2, -3), QVector3D(-5, -6, -7));
    QVERIFY(!box.isNull());
    QVERIFY(box.isFinite());
    QVERIFY(!box.isInfinite());
    QVERIFY(box.minimum() == QVector3D(-5, -6, -7));
    QVERIFY(box.maximum() == QVector3D(-1, -2, -3));

    box.setToInfinite();
    QVERIFY(!box.isNull());
    QVERIFY(!box.isFinite());
    QVERIFY(box.isInfinite());
    QVERIFY(box.minimum() == QVector3D(0, 0, 0));
    QVERIFY(box.maximum() == QVector3D(0, 0, 0));

    // Using setExtents will turn an infinite box into a finite box.
    box.setExtents(QVector3D(-1, -2, -3), QVector3D(-5, -6, -7));
    QVERIFY(!box.isNull());
    QVERIFY(box.isFinite());
    QVERIFY(!box.isInfinite());
    QVERIFY(box.minimum() == QVector3D(-5, -6, -7));
    QVERIFY(box.maximum() == QVector3D(-1, -2, -3));
}

void tst_QBox3D::compare()
{
    QBox3D box1(QVector3D(-1, -2, -3), QVector3D(-5, -6, -7));
    QBox3D box2(QVector3D(-1, -2, -3), QVector3D(-5, -6, -7));
    QBox3D box3(QVector3D(0, -2, -3), QVector3D(-5, -6, -7));
    QBox3D null1, null2;
    QBox3D infinite1, infinite2;
    infinite1.setToInfinite();
    infinite2.setToInfinite();

    QVERIFY(box1 == box2);
    QVERIFY(box1 != box3);
    QVERIFY(box1 != null1);
    QVERIFY(box1 != infinite1);

    QVERIFY(null1 == null2);
    QVERIFY(null1 != box1);
    QVERIFY(null1 != infinite1);

    QVERIFY(infinite1 == infinite2);
    QVERIFY(infinite1 != box1);
    QVERIFY(infinite1 != null1);
}

void tst_QBox3D::fuzzyCompare()
{
    QBox3D box1(QVector3D(-1, -2, -3), QVector3D(-5, -6, -7));
    QBox3D box2(QVector3D(-1, -2, -3), QVector3D(-5, -6, -7));
    QBox3D box3(QVector3D(0, -2, -3), QVector3D(-5, -6, -7));
    QBox3D null1, null2;
    QBox3D infinite1, infinite2;
    infinite1.setToInfinite();
    infinite2.setToInfinite();

    QVERIFY(qFuzzyCompare(box1, box2));
    QVERIFY(!qFuzzyCompare(box1, box3));
    QVERIFY(!qFuzzyCompare(box1, null1));
    QVERIFY(!qFuzzyCompare(box1, infinite1));

    QVERIFY(qFuzzyCompare(null1, null2));
    QVERIFY(!qFuzzyCompare(null1, box1));
    QVERIFY(!qFuzzyCompare(null1, infinite1));

    QVERIFY(qFuzzyCompare(infinite1, infinite2));
    QVERIFY(!qFuzzyCompare(infinite1, box1));
    QVERIFY(!qFuzzyCompare(infinite1, null1));
}

void tst_QBox3D::size_data()
{
    QTest::addColumn<qreal>("x1");
    QTest::addColumn<qreal>("y1");
    QTest::addColumn<qreal>("z1");
    QTest::addColumn<qreal>("x2");
    QTest::addColumn<qreal>("y2");
    QTest::addColumn<qreal>("z2");

    QTest::newRow("zero")
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0;

    QTest::newRow("one")
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)1.0 << (qreal)1.0 << (qreal)1.0;

    QTest::newRow("empty")
        << (qreal)1.0 << (qreal)1.0 << (qreal)1.0
        << (qreal)1.0 << (qreal)1.0 << (qreal)1.0;

    QTest::newRow("complex")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)4.0 << (qreal)5.0 << (qreal)6.0;
}
void tst_QBox3D::size()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);

    qreal sizex = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    qreal sizey = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    qreal sizez = (z1 < z2) ? (z2 - z1) : (z1 - z2);

    QBox3D box(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QVERIFY(box.size() == QVector3D(sizex, sizey, sizez));

    box.setToNull();
    QVERIFY(box.size() == QVector3D(0, 0, 0));

    box.setToInfinite();
    QVERIFY(box.size() == QVector3D(0, 0, 0));
}

void tst_QBox3D::center_data()
{
    // Use the same test data as the size tests.
    size_data();
}
void tst_QBox3D::center()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);

    qreal centerx = (x1 + x2) / 2;
    qreal centery = (y1 + y2) / 2;
    qreal centerz = (z1 + z2) / 2;

    QBox3D box(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QVERIFY(box.center() == QVector3D(centerx, centery, centerz));

    box.setToNull();
    QVERIFY(box.center() == QVector3D(0, 0, 0));

    box.setToInfinite();
    QVERIFY(box.center() == QVector3D(0, 0, 0));
}

void tst_QBox3D::containsPoint_data()
{
    QTest::addColumn<qreal>("x");
    QTest::addColumn<qreal>("y");
    QTest::addColumn<qreal>("z");
    QTest::addColumn<bool>("contained");

    QTest::newRow("zero")
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0 << true;

    QTest::newRow("boundary1")
        << (qreal)-1 << (qreal)-2 << (qreal)-3 << true;
    QTest::newRow("boundary2")
        << (qreal)-1 << (qreal)5 << (qreal)-3 << true;
    QTest::newRow("boundary3")
        << (qreal)-1 << (qreal)-2 << (qreal)6 << true;
    QTest::newRow("boundary4")
        << (qreal)-1 << (qreal)5 << (qreal)6 << true;
    QTest::newRow("boundary5")
        << (qreal)4 << (qreal)-2 << (qreal)-3 << true;
    QTest::newRow("boundary6")
        << (qreal)4 << (qreal)5 << (qreal)-3 << true;
    QTest::newRow("boundary7")
        << (qreal)4 << (qreal)-2 << (qreal)6 << true;
    QTest::newRow("boundary8")
        << (qreal)4 << (qreal)5 << (qreal)6 << true;

    QTest::newRow("outside1")
        << (qreal)-2 << (qreal)-2 << (qreal)-3 << false;
    QTest::newRow("outside2")
        << (qreal)-1 << (qreal)6 << (qreal)-3 << false;
    QTest::newRow("outside3")
        << (qreal)-1 << (qreal)-2 << (qreal)7 << false;
    QTest::newRow("outside4")
        << (qreal)-1 << (qreal)-3 << (qreal)6 << false;
    QTest::newRow("outside5")
        << (qreal)5 << (qreal)-2 << (qreal)-3 << false;
    QTest::newRow("outside6")
        << (qreal)4 << (qreal)6 << (qreal)-3 << false;
    QTest::newRow("outside7")
        << (qreal)4 << (qreal)-2 << (qreal)7 << false;
    QTest::newRow("outside8")
        << (qreal)4 << (qreal)-3 << (qreal)6 << false;
}
void tst_QBox3D::containsPoint()
{
    QFETCH(qreal, x);
    QFETCH(qreal, y);
    QFETCH(qreal, z);
    QFETCH(bool, contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(4, 5, 6));

    if (contained)
        QVERIFY(box.contains(QVector3D(x, y, z)));
    else
        QVERIFY(!box.contains(QVector3D(x, y, z)));

    QBox3D null;
    QVERIFY(!null.contains(QVector3D(x, y, z)));

    QBox3D infinite;
    infinite.setToInfinite();
    QVERIFY(infinite.contains(QVector3D(x, y, z)));
}

void tst_QBox3D::containsBox_data()
{
    QTest::addColumn<qreal>("x1");
    QTest::addColumn<qreal>("y1");
    QTest::addColumn<qreal>("z1");
    QTest::addColumn<qreal>("x2");
    QTest::addColumn<qreal>("y2");
    QTest::addColumn<qreal>("z2");
    QTest::addColumn<bool>("contained");
    QTest::addColumn<bool>("intersects");
    QTest::addColumn<qreal>("ix1");     // Intersection box
    QTest::addColumn<qreal>("iy1");
    QTest::addColumn<qreal>("iz1");
    QTest::addColumn<qreal>("ix2");
    QTest::addColumn<qreal>("iy2");
    QTest::addColumn<qreal>("iz2");
    QTest::addColumn<qreal>("ex1");     // Expanded box
    QTest::addColumn<qreal>("ey1");
    QTest::addColumn<qreal>("ez1");
    QTest::addColumn<qreal>("ex2");
    QTest::addColumn<qreal>("ey2");
    QTest::addColumn<qreal>("ez2");

    QTest::newRow("zero")
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << true << true
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;

    QTest::newRow("side1")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)-2.0 << (qreal)3.0
        << true << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)-2.0 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("side2")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)-3.0
        << true << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)-3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("side3")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)-1.0 << (qreal)2.0 << (qreal)3.0
        << true << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)-1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("side4")
        << (qreal)-1.0 << (qreal)2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << true << true
        << (qreal)-1.0 << (qreal)2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("side5")
        << (qreal)1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << true << true
        << (qreal)1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("side6")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << true << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;

    QTest::newRow("outside1")
        << (qreal)-1.0 << (qreal)-2.5 << (qreal)-3.0
        << (qreal)1.0 << (qreal)-2.5 << (qreal)3.0
        << false << false
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)-1.0 << (qreal)-2.5 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("outside2")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.5
        << (qreal)1.0 << (qreal)2.0 << (qreal)-3.5
        << false << false
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.5
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("outside3")
        << (qreal)-1.5 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)-1.5 << (qreal)2.0 << (qreal)3.0
        << false << false
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)-1.5 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("outside4")
        << (qreal)-1.0 << (qreal)2.5 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.5 << (qreal)3.0
        << false << false
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.5 << (qreal)3.0;
    QTest::newRow("outside5")
        << (qreal)1.5 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.5 << (qreal)2.0 << (qreal)3.0
        << false << false
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.5 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("outside6")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)3.5
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.5
        << false << false
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)0.0 << (qreal)0.0 << (qreal)0.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.5;

    QTest::newRow("overlap1")
        << (qreal)-1.0 << (qreal)-2.5 << (qreal)-3.0
        << (qreal)1.0 << (qreal)-1.5 << (qreal)3.0
        << false << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)-1.5 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.5 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("overlap2")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.5
        << (qreal)1.0 << (qreal)2.0 << (qreal)-2.5
        << false << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)-2.5
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.5
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("overlap3")
        << (qreal)-1.5 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)-0.5 << (qreal)2.0 << (qreal)3.0
        << false << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)-0.5 << (qreal)2.0 << (qreal)3.0
        << (qreal)-1.5 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("overlap4")
        << (qreal)-1.0 << (qreal)2.5 << (qreal)-3.0
        << (qreal)1.0 << (qreal)1.5 << (qreal)3.0
        << false << true
        << (qreal)-1.0 << (qreal)2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)1.5 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.5 << (qreal)3.0;
    QTest::newRow("overlap5")
        << (qreal)1.5 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)0.5 << (qreal)2.0 << (qreal)3.0
        << false << true
        << (qreal)1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)0.5 << (qreal)2.0 << (qreal)3.0
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.5 << (qreal)2.0 << (qreal)3.0;
    QTest::newRow("overlap6")
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)3.5
        << (qreal)1.0 << (qreal)2.0 << (qreal)2.5
        << false << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)2.5
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.5;

    QTest::newRow("surround")
        << (qreal)-2.0 << (qreal)-3.0 << (qreal)-4.0
        << (qreal)2.0 << (qreal)3.0 << (qreal)4.0
        << false << true
        << (qreal)-1.0 << (qreal)-2.0 << (qreal)-3.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)-2.0 << (qreal)-3.0 << (qreal)-4.0
        << (qreal)2.0 << (qreal)3.0 << (qreal)4.0;
}
void tst_QBox3D::containsBox()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(bool, contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QBox3D other(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));

    if (contained)
        QVERIFY(box.contains(other));
    else
        QVERIFY(!box.contains(other));

    QBox3D null;
    QVERIFY(!null.contains(other));

    QBox3D infinite;
    infinite.setToInfinite();
    QVERIFY(infinite.contains(other));

    QVERIFY(!box.contains(null));
    QVERIFY(!box.contains(infinite));
}

void tst_QBox3D::intersects_data()
{
    // Use the same test data as containsBox().
    containsBox_data();
}
void tst_QBox3D::intersects()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);

    Q_UNUSED(contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QBox3D other(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));

    if (intersects)
        QVERIFY(box.intersects(other));
    else
        QVERIFY(!box.intersects(other));

    QBox3D null;
    QVERIFY(!null.intersects(other));

    QBox3D infinite;
    infinite.setToInfinite();
    QVERIFY(infinite.intersects(other));

    QVERIFY(!box.intersects(null));
    QVERIFY(box.intersects(infinite));
}

void tst_QBox3D::intersect_data()
{
    // Use the same test data as containsBox().
    containsBox_data();
}
void tst_QBox3D::intersect()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(qreal, ix1);
    QFETCH(qreal, iy1);
    QFETCH(qreal, iz1);
    QFETCH(qreal, ix2);
    QFETCH(qreal, iy2);
    QFETCH(qreal, iz2);

    Q_UNUSED(contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QBox3D other(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QBox3D result(QVector3D(ix1, iy1, iz1), QVector3D(ix2, iy2, iz2));

    QBox3D ibox(box);
    ibox.intersect(other);
    if (intersects)
        QVERIFY(ibox == result);
    else
        QVERIFY(ibox.isNull());

    QBox3D null;
    null.intersect(other);
    QVERIFY(null.isNull());

    QBox3D infinite;
    infinite.setToInfinite();
    infinite.intersect(other);
    QVERIFY(infinite == other);

    QBox3D ibox2(box);
    QBox3D null2;
    ibox2.intersect(null2);
    QVERIFY(ibox2.isNull());

    QBox3D ibox3(box);
    QBox3D infinite2;
    infinite2.setToInfinite();
    ibox3.intersect(infinite2);
    QVERIFY(ibox3 == box);
}

void tst_QBox3D::intersected_data()
{
    // Use the same test data as containsBox().
    containsBox_data();
}
void tst_QBox3D::intersected()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(qreal, ix1);
    QFETCH(qreal, iy1);
    QFETCH(qreal, iz1);
    QFETCH(qreal, ix2);
    QFETCH(qreal, iy2);
    QFETCH(qreal, iz2);

    Q_UNUSED(contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QBox3D other(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QBox3D result(QVector3D(ix1, iy1, iz1), QVector3D(ix2, iy2, iz2));

    QBox3D ibox = box.intersected(other);
    if (intersects)
        QVERIFY(ibox == result);
    else
        QVERIFY(ibox.isNull());

    QBox3D null;
    ibox = null.intersected(other);
    QVERIFY(ibox.isNull());

    QBox3D infinite;
    infinite.setToInfinite();
    ibox = infinite.intersected(other);
    QVERIFY(ibox == other);

    QVERIFY(box.intersected(null).isNull());
    QVERIFY(box.intersected(infinite) == box);
}

void tst_QBox3D::intersectRay_data()
{
    QTest::addColumn<qreal>("x1");
    QTest::addColumn<qreal>("y1");
    QTest::addColumn<qreal>("z1");
    QTest::addColumn<qreal>("x2");
    QTest::addColumn<qreal>("y2");
    QTest::addColumn<qreal>("z2");
    QTest::addColumn<bool>("intersects");
    QTest::addColumn<qreal>("mint");
    QTest::addColumn<qreal>("maxt");
    QTest::addColumn<qreal>("originx");
    QTest::addColumn<qreal>("originy");
    QTest::addColumn<qreal>("originz");
    QTest::addColumn<qreal>("directionx");
    QTest::addColumn<qreal>("directiony");
    QTest::addColumn<qreal>("directionz");

    QTest::newRow("zero")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("zero-x")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << true << qreal(1.0) << qreal(1.0)
        << qreal(-1.0) << qreal(0.0) << qreal(0.0)
        << qreal(1.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("zero-neg-x")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << true << qreal(-1.0) << qreal(-1.0)
        << qreal(-1.0) << qreal(0.0) << qreal(0.0)
        << qreal(-1.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("zero-x-miss")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(1.0) << qreal(0.0)
        << qreal(1.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("zero-y")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << true << qreal(1.0) << qreal(1.0)
        << qreal(0.0) << qreal(-1.0) << qreal(0.0)
        << qreal(0.0) << qreal(1.0) << qreal(0.0);

    QTest::newRow("zero-neg-y")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << true << qreal(-1.0) << qreal(-1.0)
        << qreal(0.0) << qreal(-1.0) << qreal(0.0)
        << qreal(0.0) << qreal(-1.0) << qreal(0.0);

    QTest::newRow("zero-y-miss")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(1.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(-1.0) << qreal(0.0);

    QTest::newRow("zero-z")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << true << qreal(1.0) << qreal(1.0)
        << qreal(0.0) << qreal(0.0) << qreal(1.0)
        << qreal(0.0) << qreal(0.0) << qreal(-1.0);

    QTest::newRow("zero-neg-z")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << true << qreal(-1.0) << qreal(-1.0)
        << qreal(0.0) << qreal(0.0) << qreal(1.0)
        << qreal(0.0) << qreal(0.0) << qreal(1.0);

    QTest::newRow("zero-z-miss")
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(1.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(1.0);

    QTest::newRow("normal-no-ray")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("normal-from-inside-x")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-1.0) << qreal(1.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(1.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("normal-from-inside-neg-x")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-2.0) << qreal(2.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(-0.5) << qreal(0.0) << qreal(0.0);

    QTest::newRow("normal-from-inside-y")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-2.0) << qreal(2.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(1.0) << qreal(0.0);

    QTest::newRow("normal-from-inside-neg-y")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-4.0) << qreal(4.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(-0.5) << qreal(0.0);

    QTest::newRow("normal-from-inside-z")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-3.0) << qreal(3.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(1.0);

    QTest::newRow("normal-from-inside-neg-z")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-6.0) << qreal(6.0)
        << qreal(0.0) << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(0.0) << qreal(-0.5);

    QTest::newRow("normal-from-outside-x")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(1.0) << qreal(3.0)
        << qreal(-2.0) << qreal(0.0) << qreal(0.0)
        << qreal(1.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("normal-from-outside-neg-x")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-6.0) << qreal(-2.0)
        << qreal(-2.0) << qreal(0.0) << qreal(0.0)
        << qreal(-0.5) << qreal(0.0) << qreal(0.0);

    QTest::newRow("normal-from-outside-miss-x")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(-2.0) << qreal(3.0) << qreal(0.0)
        << qreal(1.0) << qreal(0.0) << qreal(0.0);

    QTest::newRow("normal-from-outside-y")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(1.0) << qreal(5.0)
        << qreal(0.0) << qreal(-3.0) << qreal(0.0)
        << qreal(0.0) << qreal(1.0) << qreal(0.0);

    QTest::newRow("normal-from-outside-neg-y")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-10.0) << qreal(-2.0)
        << qreal(0.0) << qreal(-3.0) << qreal(0.0)
        << qreal(0.0) << qreal(-0.5) << qreal(0.0);

    QTest::newRow("normal-from-outside-miss-y")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(2.0) << qreal(-3.0) << qreal(0.0)
        << qreal(0.0) << qreal(1.5) << qreal(0.0);

    QTest::newRow("normal-from-outside-z")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(1.0) << qreal(7.0)
        << qreal(0.0) << qreal(0.0) << qreal(-4.0)
        << qreal(0.0) << qreal(0.0) << qreal(1.0);

    QTest::newRow("normal-from-outside-neg-z")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << true << qreal(-14.0) << qreal(-2.0)
        << qreal(0.0) << qreal(0.0) << qreal(-4.0)
        << qreal(0.0) << qreal(0.0) << qreal(-0.5);

    QTest::newRow("normal-from-outside-miss-z")
        << qreal(-1.0) << qreal(-2.0) << qreal(-3.0)
        << qreal(1.0) << qreal(2.0) << qreal(3.0)
        << false << qreal(0.0) << qreal(0.0)
        << qreal(0.0) << qreal(3.0) << qreal(-4.0)
        << qreal(0.0) << qreal(0.0) << qreal(1.5);
}

void tst_QBox3D::intersectRay()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(bool, intersects);
    QFETCH(qreal, mint);
    QFETCH(qreal, maxt);
    QFETCH(qreal, originx);
    QFETCH(qreal, originy);
    QFETCH(qreal, originz);
    QFETCH(qreal, directionx);
    QFETCH(qreal, directiony);
    QFETCH(qreal, directionz);

    QBox3D box(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QRay3D ray(QVector3D(originx, originy, originz),
               QVector3D(directionx, directiony, directionz));

    qreal minimum_t = -1.0f, maximum_t = -1.0f;
    if (intersects) {
        QVERIFY(box.intersection(ray, &minimum_t, &maximum_t));
        QCOMPARE(minimum_t, mint);
        QCOMPARE(maximum_t, maxt);
        QVERIFY(box.intersects(ray));
        qreal t = box.intersection(ray);
        if (mint >= 0.0f)
            QCOMPARE(t, mint);
        else if (maxt >= 0.0f)
            QCOMPARE(t, maxt);
        else
            QVERIFY(qIsNaN(t));
    } else {
        QVERIFY(!box.intersection(ray, &minimum_t, &maximum_t));
        QVERIFY(qIsNaN(minimum_t));
        QVERIFY(qIsNaN(maximum_t));
        QVERIFY(!box.intersects(ray));
        QVERIFY(qIsNaN(box.intersection(ray)));
    }
}

void tst_QBox3D::unitePoint_data()
{
    // Use the same test data as containsPoint().
    containsPoint_data();
}
void tst_QBox3D::unitePoint()
{
    QFETCH(qreal, x);
    QFETCH(qreal, y);
    QFETCH(qreal, z);
    QFETCH(bool, contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(4, 5, 6));

    QBox3D result(box);
    result.unite(QVector3D(x, y, z));

    if (contained) {
        QVERIFY(result == box);
    } else {
        qreal minx = (x < -1) ? x : -1;
        qreal miny = (y < -2) ? y : -2;
        qreal minz = (z < -3) ? z : -3;
        qreal maxx = (x > 4) ? x : 4;
        qreal maxy = (y > 5) ? y : 5;
        qreal maxz = (z > 6) ? z : 6;
        QBox3D expected(QVector3D(minx, miny, minz), QVector3D(maxx, maxy, maxz));
        QVERIFY(result == expected);
    }

    QBox3D null;
    null.unite(QVector3D(x, y, z));
    QVERIFY(null == QBox3D(QVector3D(x, y, z), QVector3D(x, y, z)));

    QBox3D infinite;
    infinite.setToInfinite();
    infinite.unite(QVector3D(x, y, z));
    QVERIFY(infinite.isInfinite());
}

void tst_QBox3D::uniteBox_data()
{
    // Use the same test data as containsBox().
    containsBox_data();
}
void tst_QBox3D::uniteBox()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(qreal, ix1);
    QFETCH(qreal, iy1);
    QFETCH(qreal, iz1);
    QFETCH(qreal, ix2);
    QFETCH(qreal, iy2);
    QFETCH(qreal, iz2);
    QFETCH(qreal, ex1);
    QFETCH(qreal, ey1);
    QFETCH(qreal, ez1);
    QFETCH(qreal, ex2);
    QFETCH(qreal, ey2);
    QFETCH(qreal, ez2);

    Q_UNUSED(contained);
    Q_UNUSED(intersects);
    Q_UNUSED(ix1);
    Q_UNUSED(iy1);
    Q_UNUSED(iz1);
    Q_UNUSED(ix2);
    Q_UNUSED(iy2);
    Q_UNUSED(iz2);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QBox3D other(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QBox3D result(QVector3D(ex1, ey1, ez1), QVector3D(ex2, ey2, ez2));

    QBox3D ibox(box);
    ibox.unite(other);
    QVERIFY(ibox == result);

    QBox3D null;
    null.unite(other);
    QVERIFY(null == other);

    QBox3D infinite;
    infinite.setToInfinite();
    infinite.unite(other);
    QVERIFY(infinite.isInfinite());

    QBox3D ibox2(box);
    QBox3D null2;
    ibox2.unite(null2);
    QVERIFY(ibox2 == box);

    QBox3D ibox3(box);
    QBox3D infinite2;
    infinite2.setToInfinite();
    ibox3.unite(infinite2);
    QVERIFY(ibox3.isInfinite());
}

void tst_QBox3D::unitedPoint_data()
{
    // Use the same test data as containsPoint().
    containsPoint_data();
}
void tst_QBox3D::unitedPoint()
{
    QFETCH(qreal, x);
    QFETCH(qreal, y);
    QFETCH(qreal, z);
    QFETCH(bool, contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(4, 5, 6));

    QBox3D result = box.united(QVector3D(x, y, z));

    if (contained) {
        QVERIFY(result == box);
    } else {
        qreal minx = (x < -1) ? x : -1;
        qreal miny = (y < -2) ? y : -2;
        qreal minz = (z < -3) ? z : -3;
        qreal maxx = (x > 4) ? x : 4;
        qreal maxy = (y > 5) ? y : 5;
        qreal maxz = (z > 6) ? z : 6;
        QBox3D expected(QVector3D(minx, miny, minz), QVector3D(maxx, maxy, maxz));
        QVERIFY(result == expected);
    }

    QBox3D null;
    result = null.united(QVector3D(x, y, z));
    QVERIFY(result == QBox3D(QVector3D(x, y, z), QVector3D(x, y, z)));

    QBox3D infinite;
    infinite.setToInfinite();
    result = infinite.united(QVector3D(x, y, z));
    QVERIFY(result.isInfinite());
}

void tst_QBox3D::unitedBox_data()
{
    // Use the same test data as containsBox().
    containsBox_data();
}
void tst_QBox3D::unitedBox()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(qreal, ix1);
    QFETCH(qreal, iy1);
    QFETCH(qreal, iz1);
    QFETCH(qreal, ix2);
    QFETCH(qreal, iy2);
    QFETCH(qreal, iz2);
    QFETCH(qreal, ex1);
    QFETCH(qreal, ey1);
    QFETCH(qreal, ez1);
    QFETCH(qreal, ex2);
    QFETCH(qreal, ey2);
    QFETCH(qreal, ez2);

    Q_UNUSED(contained);
    Q_UNUSED(intersects);
    Q_UNUSED(ix1);
    Q_UNUSED(iy1);
    Q_UNUSED(iz1);
    Q_UNUSED(ix2);
    Q_UNUSED(iy2);
    Q_UNUSED(iz2);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QBox3D other(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QBox3D result(QVector3D(ex1, ey1, ez1), QVector3D(ex2, ey2, ez2));

    QBox3D ibox = box.united(other);
    QVERIFY(ibox == result);

    QBox3D null;
    ibox = null.united(other);
    QVERIFY(ibox == other);

    QBox3D infinite;
    infinite.setToInfinite();
    ibox = infinite.united(other);
    QVERIFY(ibox.isInfinite());

    QBox3D ibox2(box);
    QBox3D null2;
    ibox = ibox2.united(null2);
    QVERIFY(ibox == box);

    QBox3D ibox3(box);
    QBox3D infinite2;
    infinite2.setToInfinite();
    ibox = ibox3.united(infinite2);
    QVERIFY(ibox.isInfinite());
}

void tst_QBox3D::transform()
{
    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QMatrix4x4 m;
    m.rotate(90, 0, 1, 0);
    box.transform(m);
    QVERIFY(box.minimum() == QVector3D(-3, -2, -1));
    QVERIFY(box.maximum() == QVector3D(3, 2, 1));

    QBox3D null;
    null.transform(m);
    QVERIFY(null.isNull());

    QBox3D infinite;
    infinite.setToInfinite();
    infinite.transform(m);
    QVERIFY(infinite.isInfinite());
}

void tst_QBox3D::transformed()
{
    QBox3D box(QVector3D(-1, -2, -3), QVector3D(1, 2, 3));
    QMatrix4x4 m;
    m.rotate(90, 0, 1, 0);
    QBox3D box2 = box.transformed(m);
    QVERIFY(box2.minimum() == QVector3D(-3, -2, -1));
    QVERIFY(box2.maximum() == QVector3D(3, 2, 1));

    QBox3D null;
    box2 = null.transformed(m);
    QVERIFY(box2.isNull());

    QBox3D infinite;
    infinite.setToInfinite();
    box2 = infinite.transformed(m);
    QVERIFY(box2.isInfinite());
}

void tst_QBox3D::dataStream()
{
#ifndef QT_NO_DATASTREAM
    QBox3D box1(QVector3D(1.0f, 2.0f, 3.0f), QVector3D(4.0f, 5.0f, 6.0f));
    QBox3D box2; // null
    QBox3D box3;
    box3.setToInfinite();

    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << box1;
        stream << box2;
        stream << box3;
    }

    QBox3D rbox1;
    QBox3D rbox2;
    QBox3D rbox3;
    {
        QDataStream stream2(data);
        stream2 >> rbox1;
        stream2 >> rbox2;
        stream2 >> rbox3;
    }

    QVERIFY(box1 == rbox1);
    QVERIFY(box2 == rbox2);
    QVERIFY(box3 == rbox3);
#endif
}

class tst_QBox3DProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QBox3D box READ box WRITE setBox)
public:
    tst_QBox3DProperties(QObject *parent = 0) : QObject(parent) {}

    QBox3D box() const { return b; }
    void setBox(const QBox3D& value) { b = value; }

private:
    QBox3D b;
};

// Test getting and setting properties via the metaobject system.
void tst_QBox3D::properties()
{
    tst_QBox3DProperties obj;

    qRegisterMetaType<QBox3D>();

    obj.setBox(QBox3D(QVector3D(1, 2, 3), QVector3D(4, 5, 6)));

    QBox3D b = qVariantValue<QBox3D>(obj.property("box"));
    QCOMPARE(b.minimum(), QVector3D(1, 2, 3));
    QCOMPARE(b.maximum(), QVector3D(4, 5, 6));

    obj.setProperty("box",
                    qVariantFromValue
                        (QBox3D(QVector3D(-1, -2, -3), QVector3D(-4, -5, -6))));

    b = qVariantValue<QBox3D>(obj.property("box"));
    QCOMPARE(b.minimum(), QVector3D(-4, -5, -6));
    QCOMPARE(b.maximum(), QVector3D(-1, -2, -3));
}

void tst_QBox3D::metaTypes()
{
    int id = qMetaTypeId<QBox3D>();
    QVERIFY(QMetaType::type("QBox3D") == id);
    QCOMPARE(QByteArray(QMetaType::typeName(id)), QByteArray("QBox3D"));
    QVERIFY(QMetaType::isRegistered(id));
}

QTEST_APPLESS_MAIN(tst_QBox3D)

#include "tst_qbox3d.moc"
