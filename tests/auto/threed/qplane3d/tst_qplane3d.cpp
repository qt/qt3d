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

#include "qplane3d.h"
#include "qray3d.h"

class tst_QPlane3D : public QObject
{
    Q_OBJECT
public:
    tst_QPlane3D() {}
    ~tst_QPlane3D() {}

private slots:
    void create_data();
    void create();
    void intersection_data();
    void intersection();
    void noIntersection_data();
    void noIntersection();
    void contains_data();
    void contains();
    void distance_data();
    void distance();
    void compare();
    void transform_data();
    void transform();
    void dataStream();
    void properties();
    void metaTypes();
};

// Fix the problem where a compared value happens to be zero (and
// you cannot always predict this, and should not predict it
// since then you produce self-fulling prophecies instead of tests).
// In that case qFuzzyCompare has a completely strict criterion since
// it finds the "fudge factor" by multiplying by zero...
static inline bool fuzzyCompare(float p1, float p2)
{
    float fac = qMin(qAbs(p1), qAbs(p2));
    return (qAbs(p1 - p2) <= (qIsNull(fac) ? 0.00001f : 0.00001f * fac));
}

static inline bool fuzzyCompare(const QVector3D &lhs, const QVector3D &rhs)
{
    if (fuzzyCompare(lhs.x(), rhs.x()) &&
            fuzzyCompare(lhs.y(), rhs.y()) &&
            fuzzyCompare(lhs.z(), rhs.z()))
        return true;
#ifndef QT_NO_DEBUG_STREAM
    qWarning() << "actual:" << lhs;
    qWarning() << "expected:" << rhs;
#endif
    return false;
}

static inline bool fuzzyIsNull(const QVector3D &v)
{
    return fuzzyCompare(v.x(), 0.0f) &&
           fuzzyCompare(v.y(), 0.0f) &&
           fuzzyCompare(v.z(), 0.0f);
}

void tst_QPlane3D::create_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("normal");

    // normalized vectors for the normals.
    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(1.0f, 0.0f, 0.0f);

    QTest::newRow("line paralell -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -1.0f);

    QTest::newRow("vertical line (paralell to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 1.0f, 0.0f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.57735026919f, 0.57735026919f, 0.57735026919f);

    // Unnormalized vectors for the normals.
    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f);

    QTest::newRow("line paralell -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f);

    QTest::newRow("vertical line (paralell to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f);
}

void tst_QPlane3D::create()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, normal);
    QPlane3D plane(point, normal);
    QVERIFY(fuzzyCompare(plane.normal(), normal));
    QVERIFY(fuzzyCompare(plane.origin(), point));

    QPlane3D plane2;
    QVERIFY(plane2.origin() == QVector3D(0, 0, 0));
    QVERIFY(plane2.normal() == QVector3D(1, 0, 0));
    plane2.setOrigin(point);
    plane2.setNormal(normal);
    QVERIFY(fuzzyCompare(plane2.normal(), normal));
    QVERIFY(fuzzyCompare(plane2.origin(), point));
}

void tst_QPlane3D::intersection_data()
{
    // Line
    QTest::addColumn<QVector3D>("point1");
    QTest::addColumn<QVector3D>("direction");

    // Plane
    QTest::addColumn<QVector3D>("point2");
    QTest::addColumn<QVector3D>("normal");

    // Resulting intersection
    QTest::addColumn<QVector3D>("intersection");

    // These direction vectors will get normalized
    QTest::newRow("line on x-axis, plane in z-y")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(4.0f, 1.0f, 1.0f)
            << QVector3D(1.0f, 0.0f, 0.0f)
            << QVector3D(4.0f, 0.0f, 0.0f);

    QTest::newRow("line -z-axis, plane in x-y")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(1.0f, 1.0f, -2.4f)
            << QVector3D(0.0f, 0.0f, -1.0f)
            << QVector3D(3.0f, 3.0f, -2.4f);

    QTest::newRow("line y-axis, plane in x-z")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f)
            << QVector3D(1.5f, 0.6f, 1.5f)
            << QVector3D(0.0f, -7.2f, 0.0f)
            << QVector3D(0.5f, 0.6f, 0.5f);

    QTest::newRow("line equidistant from axes, plane in y-z")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(5.0f, 3.0f, 3.0f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(3.8333332539f, 3.3333332539f, 3.8333332539f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f)
            << QVector3D(5.0f, 3.0f, 3.0f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D( 1.4444446564f, 3.6666665077f, 5.8888893127f);
}

void tst_QPlane3D::intersection()
{
    QFETCH(QVector3D, point1);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point2);
    QFETCH(QVector3D, normal);
    QFETCH(QVector3D, intersection);

    QRay3D line(point1, direction);
    QPlane3D plane(point2, normal);

    float t = plane.intersection(line);
    QVERIFY(!qIsNaN(t));
    QVERIFY(fuzzyCompare(line.point(t), intersection));
    QVERIFY(plane.intersects(line));
}

void tst_QPlane3D::noIntersection_data()
{
    QTest::addColumn<QVector3D>("point1");
    QTest::addColumn<QVector3D>("normal");
    QTest::addColumn<QVector3D>("point2");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("intersection");

    // These direction vectors will get normalized
    QTest::newRow("line on x-axis, plane in z-x")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(4.0f, 1.0f, 1.0f)
            << QVector3D(0.0f, 1.0f, 0.0f);

    QTest::newRow("line -z-axis, lies on plane in z-x")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(1.0f, 3.0f, 1.0f)
            << QVector3D(0.0f, -0.7f, 0.0f);

    QTest::newRow("line on an angle, never meets plane on angle")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.1f, 0.0f, 0.0f)
            << QVector3D(1.0f, 1.0f, 3.0f)
            << QVector3D(0.0f, 1.0f, 0.0f);
}

void tst_QPlane3D::noIntersection()
{
    QFETCH(QVector3D, point1);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point2);
    QFETCH(QVector3D, normal);

    QPlane3D plane(point1, normal);
    QRay3D line(point2, direction);

    float t = plane.intersection(line);
    QVERIFY(qIsNaN(t));
    QVERIFY(!plane.intersects(line));
}

// Find a vector that lies perpendicular to the normal, and in the plane.
static QVector3D vectorInPlane(const QPlane3D &plane)
{
    QVector3D v = QVector3D::crossProduct(plane.normal(), QVector3D(1, 0, 0));
    if (fuzzyIsNull(v))
        v = QVector3D::crossProduct(plane.normal(), QVector3D(0, 1, 0));
    if (fuzzyIsNull(v))
        v = QVector3D::crossProduct(plane.normal(), QVector3D(0, 0, 1));
    v = QVector3D::dotProduct(v, plane.normal()) * plane.normal() /
                plane.normal().lengthSquared();
    Q_ASSERT(fuzzyCompare(QVector3D::dotProduct(v, plane.normal()), 0.0f));
    return v;
}

void tst_QPlane3D::contains_data()
{
    create_data();
}

void tst_QPlane3D::contains()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, normal);
    QPlane3D plane(point, normal);

    QVERIFY(plane.contains(point));
    QVERIFY(!plane.contains(point + normal));
    QVERIFY(!plane.contains(point - normal));

    QVector3D v = vectorInPlane(plane);
    QVERIFY(plane.contains(QRay3D(point, v)));
    QVERIFY(plane.contains(QRay3D(point - v, v)));
    QVERIFY(!plane.contains(QRay3D(point + normal, v)));
    QVERIFY(!plane.contains(QRay3D(point, normal)));
}

void tst_QPlane3D::distance_data()
{
    create_data();
}

void tst_QPlane3D::distance()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, normal);
    QPlane3D plane(point, normal);

    QVERIFY(fuzzyCompare(plane.distance(point), 0.0f));
    QVERIFY(fuzzyCompare(plane.distance(point + normal), normal.length()));
    QVERIFY(fuzzyCompare(plane.distance(point - normal), -normal.length()));

    QVector3D v = vectorInPlane(plane);
    QVERIFY(fuzzyCompare(plane.distance(point + v), 0.0f));
    QVERIFY(fuzzyCompare(plane.distance(point + normal + v), normal.length()));
    QVERIFY(fuzzyCompare(plane.distance(point - normal + v), -normal.length()));
}

void tst_QPlane3D::compare()
{
    QPlane3D plane1(QVector3D(10, 20, 30), QVector3D(-1, 2, 4));
    QPlane3D plane2(QVector3D(10, 20, 30), QVector3D(1, -2, -4));
    QPlane3D plane3(QVector3D(0, 20, 30), QVector3D(-1, 2, 4));
    QVERIFY(plane1 == plane1);
    QVERIFY(!(plane1 != plane1));
    QVERIFY(qFuzzyCompare(plane1, plane1));
    QVERIFY(plane1 != plane2);
    QVERIFY(!(plane1 == plane2));
    QVERIFY(!qFuzzyCompare(plane1, plane2));
    QVERIFY(plane1 != plane3);
    QVERIFY(!(plane1 == plane3));
    QVERIFY(!qFuzzyCompare(plane1, plane3));
}

void tst_QPlane3D::transform_data()
{
    create_data();
}

void tst_QPlane3D::transform()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, normal);

    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);

    QPlane3D plane1(point, normal);
    QPlane3D plane2(plane1);
    QPlane3D plane3;

    plane1.transform(m);
    plane3 = plane2.transformed(m);

    QVERIFY(fuzzyCompare(plane1.origin(), plane3.origin()));
    QVERIFY(fuzzyCompare(plane1.normal(), plane3.normal()));

    QVERIFY(fuzzyCompare(plane1.origin(), m * point));
    QVERIFY(fuzzyCompare(plane1.normal(), m.mapVector(normal)));
}

void tst_QPlane3D::dataStream()
{
#ifndef QT_NO_DATASTREAM
    QPlane3D plane(QVector3D(1.0f, 2.0f, 3.0f),
                   QVector3D(4.0f, 5.0f, 6.0f));

    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << plane;
    }

    QPlane3D plane2;
    {
        QDataStream stream2(data);
        stream2 >> plane2;
    }

    QVERIFY(plane == plane2);
#endif
}

class tst_QPlane3DProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPlane3D plane READ plane WRITE setPlane)
public:
    tst_QPlane3DProperties(QObject *parent = 0) : QObject(parent) {}

    QPlane3D plane() const { return p; }
    void setPlane(const QPlane3D& value) { p = value; }

private:
    QPlane3D p;
};

// Test getting and setting properties via the metaobject system.
void tst_QPlane3D::properties()
{
    tst_QPlane3DProperties obj;

    qRegisterMetaType<QPlane3D>();

    obj.setPlane(QPlane3D(QVector3D(1, 2, 3), QVector3D(4, 5, 6)));

    QPlane3D p = qvariant_cast<QPlane3D>(obj.property("plane"));
    QCOMPARE(p.origin(), QVector3D(1, 2, 3));
    QCOMPARE(p.normal(), QVector3D(4, 5, 6));

    obj.setProperty("plane",
                    qVariantFromValue
                        (QPlane3D(QVector3D(-1, -2, -3), QVector3D(-4, -5, -6))));

    p = qvariant_cast<QPlane3D>(obj.property("plane"));
    QCOMPARE(p.origin(), QVector3D(-1, -2, -3));
    QCOMPARE(p.normal(), QVector3D(-4, -5, -6));
}

void tst_QPlane3D::metaTypes()
{
    int id = qMetaTypeId<QPlane3D>();
    QVERIFY(QMetaType::type("QPlane3D") == id);
    QCOMPARE(QByteArray(QMetaType::typeName(id)), QByteArray("QPlane3D"));
    QVERIFY(QMetaType::isRegistered(id));
}

QTEST_APPLESS_MAIN(tst_QPlane3D)

#include "tst_qplane3d.moc"
