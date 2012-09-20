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
    QTest::addColumn<float>("x1");
    QTest::addColumn<float>("y1");
    QTest::addColumn<float>("z1");
    QTest::addColumn<float>("x2");
    QTest::addColumn<float>("y2");
    QTest::addColumn<float>("z2");

    QTest::newRow("zero")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f;

    QTest::newRow("one")
        << 0.0f << 0.0f << 0.0f
        << 1.0f << 1.0f << 1.0f;

    QTest::newRow("empty")
        << 1.0f << 1.0f << 1.0f
        << 1.0f << 1.0f << 1.0f;

    QTest::newRow("complex")
        << -1.0f << -2.0f << -3.0f
        << 4.0f << 5.0f << 6.0f;
}
void tst_QBox3D::size()
{
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);

    float sizex = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    float sizey = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    float sizez = (z1 < z2) ? (z2 - z1) : (z1 - z2);

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
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);

    float centerx = (x1 + x2) / 2;
    float centery = (y1 + y2) / 2;
    float centerz = (z1 + z2) / 2;

    QBox3D box(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QVERIFY(box.center() == QVector3D(centerx, centery, centerz));

    box.setToNull();
    QVERIFY(box.center() == QVector3D(0, 0, 0));

    box.setToInfinite();
    QVERIFY(box.center() == QVector3D(0, 0, 0));
}

void tst_QBox3D::containsPoint_data()
{
    QTest::addColumn<float>("x");
    QTest::addColumn<float>("y");
    QTest::addColumn<float>("z");
    QTest::addColumn<bool>("contained");

    QTest::newRow("zero")
        << 0.0f << 0.0f << 0.0f << true;

    QTest::newRow("boundary1")
        << -1.0f << -2.0f << -3.0f << true;
    QTest::newRow("boundary2")
        << -1.0f << 5.0f << -3.0f << true;
    QTest::newRow("boundary3")
        << -1.0f << -2.0f << 6.0f << true;
    QTest::newRow("boundary4")
        << -1.0f << 5.0f << 6.0f << true;
    QTest::newRow("boundary5")
        << 4.0f << -2.0f << -3.0f << true;
    QTest::newRow("boundary6")
        << 4.0f << 5.0f << -3.0f << true;
    QTest::newRow("boundary7")
        << 4.0f << -2.0f << 6.0f << true;
    QTest::newRow("boundary8")
        << 4.0f << 5.0f << 6.0f << true;

    QTest::newRow("outside1")
        << -2.0f << -2.0f << -3.0f << false;
    QTest::newRow("outside2")
        << -1.0f << 6.0f << -3.0f << false;
    QTest::newRow("outside3")
        << -1.0f << -2.0f << 7.0f << false;
    QTest::newRow("outside4")
        << -1.0f << -3.0f << 6.0f << false;
    QTest::newRow("outside5")
        << 5.0f << -2.0f << -3.0f << false;
    QTest::newRow("outside6")
        << 4.0f << 6.0f << -3.0f << false;
    QTest::newRow("outside7")
        << 4.0f << -2.0f << 7.0f << false;
    QTest::newRow("outside8")
        << 4.0f << -3.0f << 6.0f << false;
}
void tst_QBox3D::containsPoint()
{
    QFETCH(float, x);
    QFETCH(float, y);
    QFETCH(float, z);
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
    QTest::addColumn<float>("x1");
    QTest::addColumn<float>("y1");
    QTest::addColumn<float>("z1");
    QTest::addColumn<float>("x2");
    QTest::addColumn<float>("y2");
    QTest::addColumn<float>("z2");
    QTest::addColumn<bool>("contained");
    QTest::addColumn<bool>("intersects");
    QTest::addColumn<float>("ix1");     // Intersection box
    QTest::addColumn<float>("iy1");
    QTest::addColumn<float>("iz1");
    QTest::addColumn<float>("ix2");
    QTest::addColumn<float>("iy2");
    QTest::addColumn<float>("iz2");
    QTest::addColumn<float>("ex1");     // Expanded box
    QTest::addColumn<float>("ey1");
    QTest::addColumn<float>("ez1");
    QTest::addColumn<float>("ex2");
    QTest::addColumn<float>("ey2");
    QTest::addColumn<float>("ez2");

    QTest::newRow("zero")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << true << true
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;

    QTest::newRow("side1")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << -2.0f << 3.0f
        << true << true
        << -1.0f << -2.0f << -3.0f
        << 1.0f << -2.0f << 3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("side2")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << -3.0f
        << true << true
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << -3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("side3")
        << -1.0f << -2.0f << -3.0f
        << -1.0f << 2.0f << 3.0f
        << true << true
        << -1.0f << -2.0f << -3.0f
        << -1.0f << 2.0f << 3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("side4")
        << -1.0f << 2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << true
        << -1.0f << 2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("side5")
        << 1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << true
        << 1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("side6")
        << -1.0f << -2.0f << 3.0f
        << 1.0f << 2.0f << 3.0f
        << true << true
        << -1.0f << -2.0f << 3.0f
        << 1.0f << 2.0f << 3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;

    QTest::newRow("outside1")
        << -1.0f << -2.5f << -3.0f
        << 1.0f << -2.5f << 3.0f
        << false << false
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << -1.0f << -2.5f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("outside2")
        << -1.0f << -2.0f << -3.5f
        << 1.0f << 2.0f << -3.5f
        << false << false
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << -1.0f << -2.0f << -3.5f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("outside3")
        << -1.5f << -2.0f << -3.0f
        << -1.5f << 2.0f << 3.0f
        << false << false
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << -1.5f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("outside4")
        << -1.0f << 2.5f << -3.0f
        << 1.0f << 2.5f << 3.0f
        << false << false
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.5f << 3.0f;
    QTest::newRow("outside5")
        << 1.5f << -2.0f << -3.0f
        << 1.5f << 2.0f << 3.0f
        << false << false
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << -1.0f << -2.0f << -3.0f
        << 1.5f << 2.0f << 3.0f;
    QTest::newRow("outside6")
        << -1.0f << -2.0f << 3.5f
        << 1.0f << 2.0f << 3.5f
        << false << false
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.5f;

    QTest::newRow("overlap1")
        << -1.0f << -2.5f << -3.0f
        << 1.0f << -1.5f << 3.0f
        << false << true
        << -1.0f << -2.0f << -3.0f
        << 1.0f << -1.5f << 3.0f
        << -1.0f << -2.5f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("overlap2")
        << -1.0f << -2.0f << -3.5f
        << 1.0f << 2.0f << -2.5f
        << false << true
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << -2.5f
        << -1.0f << -2.0f << -3.5f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("overlap3")
        << -1.5f << -2.0f << -3.0f
        << -0.5f << 2.0f << 3.0f
        << false << true
        << -1.0f << -2.0f << -3.0f
        << -0.5f << 2.0f << 3.0f
        << -1.5f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f;
    QTest::newRow("overlap4")
        << -1.0f << 2.5f << -3.0f
        << 1.0f << 1.5f << 3.0f
        << false << true
        << -1.0f << 2.0f << -3.0f
        << 1.0f << 1.5f << 3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.5f << 3.0f;
    QTest::newRow("overlap5")
        << 1.5f << -2.0f << -3.0f
        << 0.5f << 2.0f << 3.0f
        << false << true
        << 1.0f << -2.0f << -3.0f
        << 0.5f << 2.0f << 3.0f
        << -1.0f << -2.0f << -3.0f
        << 1.5f << 2.0f << 3.0f;
    QTest::newRow("overlap6")
        << -1.0f << -2.0f << 3.5f
        << 1.0f << 2.0f << 2.5f
        << false << true
        << -1.0f << -2.0f << 3.0f
        << 1.0f << 2.0f << 2.5f
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.5f;

    QTest::newRow("surround")
        << -2.0f << -3.0f << -4.0f
        << 2.0f << 3.0f << 4.0f
        << false << true
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << -2.0f << -3.0f << -4.0f
        << 2.0f << 3.0f << 4.0f;
}
void tst_QBox3D::containsBox()
{
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);
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
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);
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
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(float, ix1);
    QFETCH(float, iy1);
    QFETCH(float, iz1);
    QFETCH(float, ix2);
    QFETCH(float, iy2);
    QFETCH(float, iz2);

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
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(float, ix1);
    QFETCH(float, iy1);
    QFETCH(float, iz1);
    QFETCH(float, ix2);
    QFETCH(float, iy2);
    QFETCH(float, iz2);

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
    QTest::addColumn<float>("x1");
    QTest::addColumn<float>("y1");
    QTest::addColumn<float>("z1");
    QTest::addColumn<float>("x2");
    QTest::addColumn<float>("y2");
    QTest::addColumn<float>("z2");
    QTest::addColumn<bool>("intersects");
    QTest::addColumn<float>("mint");
    QTest::addColumn<float>("maxt");
    QTest::addColumn<float>("originx");
    QTest::addColumn<float>("originy");
    QTest::addColumn<float>("originz");
    QTest::addColumn<float>("directionx");
    QTest::addColumn<float>("directiony");
    QTest::addColumn<float>("directionz");

    QTest::newRow("zero")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << false << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f;

    QTest::newRow("zero-x")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << true << 1.0f << 1.0f
        << -1.0f << 0.0f << 0.0f
        << 1.0f << 0.0f << 0.0f;

    QTest::newRow("zero-neg-x")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << true << -1.0f << -1.0f
        << -1.0f << 0.0f << 0.0f
        << -1.0f << 0.0f << 0.0f;

    QTest::newRow("zero-x-miss")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << false << 0.0f << 0.0f
        << 0.0f << 1.0f << 0.0f
        << 1.0f << 0.0f << 0.0f;

    QTest::newRow("zero-y")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << true << 1.0f << 1.0f
        << 0.0f << -1.0f << 0.0f
        << 0.0f << 1.0f << 0.0f;

    QTest::newRow("zero-neg-y")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << true << -1.0f << -1.0f
        << 0.0f << -1.0f << 0.0f
        << 0.0f << -1.0f << 0.0f;

    QTest::newRow("zero-y-miss")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << false << 0.0f << 0.0f
        << 1.0f << 0.0f << 0.0f
        << 0.0f << -1.0f << 0.0f;

    QTest::newRow("zero-z")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << true << 1.0f << 1.0f
        << 0.0f << 0.0f << 1.0f
        << 0.0f << 0.0f << -1.0f;

    QTest::newRow("zero-neg-z")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << true << -1.0f << -1.0f
        << 0.0f << 0.0f << 1.0f
        << 0.0f << 0.0f << 1.0f;

    QTest::newRow("zero-z-miss")
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << false << 0.0f << 0.0f
        << 0.0f << 1.0f << 0.0f
        << 0.0f << 0.0f << 1.0f;

    QTest::newRow("normal-no-ray")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << false << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 0.0f;

    QTest::newRow("normal-from-inside-x")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -1.0f << 1.0f
        << 0.0f << 0.0f << 0.0f
        << 1.0f << 0.0f << 0.0f;

    QTest::newRow("normal-from-inside-neg-x")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -2.0f << 2.0f
        << 0.0f << 0.0f << 0.0f
        << -0.5f << 0.0f << 0.0f;

    QTest::newRow("normal-from-inside-y")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -2.0f << 2.0f
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 1.0f << 0.0f;

    QTest::newRow("normal-from-inside-neg-y")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -4.0f << 4.0f
        << 0.0f << 0.0f << 0.0f
        << 0.0f << -0.5f << 0.0f;

    QTest::newRow("normal-from-inside-z")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -3.0f << 3.0f
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << 1.0f;

    QTest::newRow("normal-from-inside-neg-z")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -6.0f << 6.0f
        << 0.0f << 0.0f << 0.0f
        << 0.0f << 0.0f << -0.5f;

    QTest::newRow("normal-from-outside-x")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << 1.0f << 3.0f
        << -2.0f << 0.0f << 0.0f
        << 1.0f << 0.0f << 0.0f;

    QTest::newRow("normal-from-outside-neg-x")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -6.0f << -2.0f
        << -2.0f << 0.0f << 0.0f
        << -0.5f << 0.0f << 0.0f;

    QTest::newRow("normal-from-outside-miss-x")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << false << 0.0f << 0.0f
        << -2.0f << 3.0f << 0.0f
        << 1.0f << 0.0f << 0.0f;

    QTest::newRow("normal-from-outside-y")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << 1.0f << 5.0f
        << 0.0f << -3.0f << 0.0f
        << 0.0f << 1.0f << 0.0f;

    QTest::newRow("normal-from-outside-neg-y")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -10.0f << -2.0f
        << 0.0f << -3.0f << 0.0f
        << 0.0f << -0.5f << 0.0f;

    QTest::newRow("normal-from-outside-miss-y")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << false << 0.0f << 0.0f
        << 2.0f << -3.0f << 0.0f
        << 0.0f << 1.5f << 0.0f;

    QTest::newRow("normal-from-outside-z")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << 1.0f << 7.0f
        << 0.0f << 0.0f << -4.0f
        << 0.0f << 0.0f << 1.0f;

    QTest::newRow("normal-from-outside-neg-z")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << true << -14.0f << -2.0f
        << 0.0f << 0.0f << -4.0f
        << 0.0f << 0.0f << -0.5f;

    QTest::newRow("normal-from-outside-miss-z")
        << -1.0f << -2.0f << -3.0f
        << 1.0f << 2.0f << 3.0f
        << false << 0.0f << 0.0f
        << 0.0f << 3.0f << -4.0f
        << 0.0f << 0.0f << 1.5f;
}

void tst_QBox3D::intersectRay()
{
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);
    QFETCH(bool, intersects);
    QFETCH(float, mint);
    QFETCH(float, maxt);
    QFETCH(float, originx);
    QFETCH(float, originy);
    QFETCH(float, originz);
    QFETCH(float, directionx);
    QFETCH(float, directiony);
    QFETCH(float, directionz);

    QBox3D box(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QRay3D ray(QVector3D(originx, originy, originz),
               QVector3D(directionx, directiony, directionz));

    float minimum_t = -1.0f, maximum_t = -1.0f;
    if (intersects) {
        QVERIFY(box.intersection(ray, &minimum_t, &maximum_t));
        QCOMPARE(minimum_t, mint);
        QCOMPARE(maximum_t, maxt);
        QVERIFY(box.intersects(ray));
        float t = box.intersection(ray);
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
    QFETCH(float, x);
    QFETCH(float, y);
    QFETCH(float, z);
    QFETCH(bool, contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(4, 5, 6));

    QBox3D result(box);
    result.unite(QVector3D(x, y, z));

    if (contained) {
        QVERIFY(result == box);
    } else {
        float minx = (x < -1) ? x : -1;
        float miny = (y < -2) ? y : -2;
        float minz = (z < -3) ? z : -3;
        float maxx = (x > 4) ? x : 4;
        float maxy = (y > 5) ? y : 5;
        float maxz = (z > 6) ? z : 6;
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
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(float, ix1);
    QFETCH(float, iy1);
    QFETCH(float, iz1);
    QFETCH(float, ix2);
    QFETCH(float, iy2);
    QFETCH(float, iz2);
    QFETCH(float, ex1);
    QFETCH(float, ey1);
    QFETCH(float, ez1);
    QFETCH(float, ex2);
    QFETCH(float, ey2);
    QFETCH(float, ez2);

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
    QFETCH(float, x);
    QFETCH(float, y);
    QFETCH(float, z);
    QFETCH(bool, contained);

    QBox3D box(QVector3D(-1, -2, -3), QVector3D(4, 5, 6));

    QBox3D result = box.united(QVector3D(x, y, z));

    if (contained) {
        QVERIFY(result == box);
    } else {
        float minx = (x < -1) ? x : -1;
        float miny = (y < -2) ? y : -2;
        float minz = (z < -3) ? z : -3;
        float maxx = (x > 4) ? x : 4;
        float maxy = (y > 5) ? y : 5;
        float maxz = (z > 6) ? z : 6;
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
    QFETCH(float, x1);
    QFETCH(float, y1);
    QFETCH(float, z1);
    QFETCH(float, x2);
    QFETCH(float, y2);
    QFETCH(float, z2);
    QFETCH(bool, contained);
    QFETCH(bool, intersects);
    QFETCH(float, ix1);
    QFETCH(float, iy1);
    QFETCH(float, iz1);
    QFETCH(float, ix2);
    QFETCH(float, iy2);
    QFETCH(float, iz2);
    QFETCH(float, ex1);
    QFETCH(float, ey1);
    QFETCH(float, ez1);
    QFETCH(float, ex2);
    QFETCH(float, ey2);
    QFETCH(float, ez2);

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

    QBox3D b = qvariant_cast<QBox3D>(obj.property("box"));
    QCOMPARE(b.minimum(), QVector3D(1, 2, 3));
    QCOMPARE(b.maximum(), QVector3D(4, 5, 6));

    obj.setProperty("box",
                    qVariantFromValue
                        (QBox3D(QVector3D(-1, -2, -3), QVector3D(-4, -5, -6))));

    b = qvariant_cast<QBox3D>(obj.property("box"));
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
