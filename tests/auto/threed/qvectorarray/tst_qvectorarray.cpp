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
#include <QtGui/qmatrix4x4.h>
#include "qvector2darray.h"
#include "qvector3darray.h"
#include "qvector4darray.h"
#include "qtest_helpers.h"

class tst_QVectorArray : public QObject
{
    Q_OBJECT
public:
    tst_QVectorArray() {}
    ~tst_QVectorArray() {}

private slots:
    void create2DArray();
    void create3DArray();
    void create4DArray();

    void scale_data();
    void scale();

    void translateVector_data();
    void translateVector();

    void translateExplicit_data();
    void translateExplicit();

    void transform();
};

void tst_QVectorArray::create2DArray()
{
    QVector2DArray array;
    QVERIFY(array.isEmpty());

    array.append(1.0f, 2.0f);
    array.append(3.0f, 4.0f);
    array.append(QVector2D(5.0f, 6.0f));
    array.append(QPointF(7.0f, 8.0f));
    array.append(QPoint(9, 10));

    QCOMPARE(array.size(), 5);
    QVERIFY(array[0] == QVector2D(1.0f, 2.0f));
    QVERIFY(array[1] == QVector2D(3.0f, 4.0f));
    QVERIFY(array[2] == QVector2D(5.0f, 6.0f));
    QVERIFY(array[3] == QVector2D(7.0f, 8.0f));
    QVERIFY(array[4] == QVector2D(9.0f, 10.0f));

    array.append(QVector2D(11.0f, 12.0f), QVector2D(13.0f, 14.0f));
    array.append(QVector2D(15.0f, 16.0f), QVector2D(17.0f, 18.0f),
                 QVector2D(19.0f, 20.0f));
    array.append(QVector2D(21.0f, 22.0f), QVector2D(23.0f, 24.0f),
                 QVector2D(25.0f, 26.0f));

    for (int index = 0; index < array.size(); ++index)
        QVERIFY(array[index] == QVector2D(index * 2 + 1, index * 2 + 2));

    QVector2DArray array2(34);
    QCOMPARE(array2.size(), 34);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], QVector2D(0.0f, 0.0f));

    QVector2DArray array3(15, QVector2D(1.0f, 2.0f));
    QCOMPARE(array3.size(), 15);
    for (int index = 0; index < array3.size(); ++index)
        QCOMPARE(array3[index], QVector2D(1.0f, 2.0f));
}

void tst_QVectorArray::create3DArray()
{
    QVector3DArray array;
    QVERIFY(array.isEmpty());

    array.append(1.0f, 2.0f, 3.0f);
    array.append(3.0f, 4.0f, 5.0f);
    array.append(QVector3D(5.0f, 6.0f, 7.0f));

    QCOMPARE(array.size(), 3);
    QVERIFY(array[0] == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array[1] == QVector3D(3.0f, 4.0f, 5.0f));
    QVERIFY(array[2] == QVector3D(5.0f, 6.0f, 7.0f));

    array.append(QVector3D(7.0f, 8.0f, 9.0f),
                 QVector3D(9.0f, 10.0f, 11.0f));
    array.append(QVector3D(11.0f, 12.0f, 13.0f),
                 QVector3D(13.0f, 14.0f, 15.0f),
                 QVector3D(15.0f, 16.0f, 17.0f));
    array.append(QVector3D(17.0f, 18.0f, 19.0f),
                 QVector3D(19.0f, 20.0f, 21.0f),
                 QVector3D(21.0f, 22.0f, 23.0f));

    for (int index = 0; index < array.size(); ++index) {
        QVERIFY(array[index] == QVector3D(index * 2 + 1,
                                          index * 2 + 2,
                                          index * 2 + 3));
    }

    QVector3DArray array2(34);
    QCOMPARE(array2.size(), 34);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], QVector3D(0.0f, 0.0f, 0.0f));

    QVector3DArray array3(15, QVector3D(1.0f, 2.0f, 3.0f));
    QCOMPARE(array3.size(), 15);
    for (int index = 0; index < array3.size(); ++index)
        QCOMPARE(array3[index], QVector3D(1.0f, 2.0f, 3.0f));
}

void tst_QVectorArray::create4DArray()
{
    QVector4DArray array;
    QVERIFY(array.isEmpty());

    array.append(1.0f, 2.0f, 3.0f, 4.0f);
    array.append(3.0f, 4.0f, 5.0f, 6.0f);
    array.append(QVector4D(5.0f, 6.0f, 7.0f, 8.0f));

    QCOMPARE(array.size(), 3);
    QVERIFY(array[0] == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array[1] == QVector4D(3.0f, 4.0f, 5.0f, 6.0f));
    QVERIFY(array[2] == QVector4D(5.0f, 6.0f, 7.0f, 8.0f));

    array.append(QVector4D(7.0f, 8.0f, 9.0f, 10.0f),
                 QVector4D(9.0f, 10.0f, 11.0f, 12.0f));
    array.append(QVector4D(11.0f, 12.0f, 13.0f, 14.0f),
                 QVector4D(13.0f, 14.0f, 15.0f, 16.0f),
                 QVector4D(15.0f, 16.0f, 17.0f, 18.0f));
    array.append(QVector4D(17.0f, 18.0f, 19.0f, 20.0f),
                 QVector4D(19.0f, 20.0f, 21.0f, 22.0f),
                 QVector4D(21.0f, 22.0f, 23.0f, 24.0f));

    for (int index = 0; index < array.size(); ++index) {
        QVERIFY(array[index] == QVector4D(index * 2 + 1,
                                          index * 2 + 2,
                                          index * 2 + 3,
                                          index * 2 + 4));
    }

    QVector4DArray array2(34);
    QCOMPARE(array2.size(), 34);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], QVector4D(0.0f, 0.0f, 0.0f, 0.0f));

    QVector4DArray array3(15, QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QCOMPARE(array3.size(), 15);
    for (int index = 0; index < array3.size(); ++index)
        QCOMPARE(array3[index], QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
}

void tst_QVectorArray::scale_data()
{
    QTest::addColumn<float>("scale");

    QTest::newRow("zero") << 0.0f;
    QTest::newRow("one") << 1.0f;
    QTest::newRow("neg-one") << -1.0f;
    QTest::newRow("two") << 2.0f;
    QTest::newRow("neg-two-point-five") << -2.5f;
    QTest::newRow("half") << 0.5f;
}

void tst_QVectorArray::scale()
{
    QFETCH(float, scale);

    QVector2DArray v2array;
    QVector3DArray v3array;
    QVector4DArray v4array;
    QVector2DArray v2arrayb;
    QVector3DArray v3arrayb;
    QVector4DArray v4arrayb;

    for (int index = 0; index < 64; ++index) {
        v2array.append(index * 4, index * 4 + 1);
        v3array.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4array.append(index * 4, index * 4 + 1, index * 4 + 2,
                       index * 4 + 3);
        v2arrayb.append(index * 4, index * 4 + 1);
        v3arrayb.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4arrayb.append(index * 4, index * 4 + 1, index * 4 + 2,
                        index * 4 + 3);
    }

    // Perform a simple in-place scale.

    v2array.scale(scale);
    v3array.scale(scale);
    v4array.scale(scale);

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) * scale, (index * 4 + 1) * scale));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) * scale, (index * 4 + 1) * scale,
                           (index * 4 + 2) * scale));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) * scale, (index * 4 + 1) * scale,
                           (index * 4 + 2) * scale, (index * 4 + 3) * scale));
    }

    // Increase ref-count on an array and check that detach occurs.

    v2array = v2arrayb;
    v3array = v3arrayb;
    v4array = v4arrayb;

    QVERIFY(v2array.constData() == v2arrayb.constData());
    QVERIFY(v3array.constData() == v3arrayb.constData());
    QVERIFY(v4array.constData() == v4arrayb.constData());

    v2array.scale(scale);
    v3array.scale(scale);
    v4array.scale(scale);

    QVERIFY(v2array.constData() != v2arrayb.constData());
    QVERIFY(v3array.constData() != v3arrayb.constData());
    QVERIFY(v4array.constData() != v4arrayb.constData());

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) * scale, (index * 4 + 1) * scale));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) * scale, (index * 4 + 1) * scale,
                           (index * 4 + 2) * scale));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) * scale, (index * 4 + 1) * scale,
                           (index * 4 + 2) * scale, (index * 4 + 3) * scale));
        QCOMPARE(v2arrayb[index],
                 QVector2D(index * 4, index * 4 + 1));
        QCOMPARE(v3arrayb[index],
                 QVector3D(index * 4, index * 4 + 1, index * 4 + 2));
        QCOMPARE(v4arrayb[index],
                 QVector4D(index * 4, index * 4 + 1,
                           index * 4 + 2, index * 4 + 3));
    }

    // Perform the test again, with scaled() this time.

    v2array = v2arrayb.scaled(scale);
    v3array = v3arrayb.scaled(scale);
    v4array = v4arrayb.scaled(scale);

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) * scale, (index * 4 + 1) * scale));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) * scale, (index * 4 + 1) * scale,
                           (index * 4 + 2) * scale));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) * scale, (index * 4 + 1) * scale,
                           (index * 4 + 2) * scale, (index * 4 + 3) * scale));
        QCOMPARE(v2arrayb[index],
                 QVector2D(index * 4, index * 4 + 1));
        QCOMPARE(v3arrayb[index],
                 QVector3D(index * 4, index * 4 + 1, index * 4 + 2));
        QCOMPARE(v4arrayb[index],
                 QVector4D(index * 4, index * 4 + 1,
                           index * 4 + 2, index * 4 + 3));
    }
}

void tst_QVectorArray::translateVector_data()
{
    QTest::addColumn<QVector4D>("translation");

    QTest::newRow("zero") << QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
    QTest::newRow("x-axis") << QVector4D(1.0f, 0.0f, 0.0f, 0.0f);
    QTest::newRow("y-axis") << QVector4D(0.0f, 1.0f, 0.0f, 0.0f);
    QTest::newRow("z-axis") << QVector4D(0.0f, 0.0f, 1.0f, 0.0f);
    QTest::newRow("w-axis") << QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
    QTest::newRow("all-axes") << QVector4D(1.0f, -2.0f, 3.5f, -1.5f);
}

void tst_QVectorArray::translateVector()
{
    QFETCH(QVector4D, translation);

    QVector2DArray v2array;
    QVector3DArray v3array;
    QVector4DArray v4array;
    QVector2DArray v2arrayb;
    QVector3DArray v3arrayb;
    QVector4DArray v4arrayb;

    for (int index = 0; index < 64; ++index) {
        v2array.append(index * 4, index * 4 + 1);
        v3array.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4array.append(index * 4, index * 4 + 1, index * 4 + 2,
                       index * 4 + 3);
        v2arrayb.append(index * 4, index * 4 + 1);
        v3arrayb.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4arrayb.append(index * 4, index * 4 + 1, index * 4 + 2,
                        index * 4 + 3);
    }

    // Perform a simple in-place translate.

    v2array.translate(translation.toVector2D());
    v3array.translate(translation.toVector3D());
    v4array.translate(translation);

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y()));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z()));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z(),
                           (index * 4 + 3) + translation.w()));
    }

    // Increase ref-count on an array and check that detach occurs.

    v2array = v2arrayb;
    v3array = v3arrayb;
    v4array = v4arrayb;

    QVERIFY(v2array.constData() == v2arrayb.constData());
    QVERIFY(v3array.constData() == v3arrayb.constData());
    QVERIFY(v4array.constData() == v4arrayb.constData());

    v2array.translate(translation.toVector2D());
    v3array.translate(translation.toVector3D());
    v4array.translate(translation);

    QVERIFY(v2array.constData() != v2arrayb.constData());
    QVERIFY(v3array.constData() != v3arrayb.constData());
    QVERIFY(v4array.constData() != v4arrayb.constData());

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y()));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z()));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z(),
                           (index * 4 + 3) + translation.w()));
        QCOMPARE(v2arrayb[index],
                 QVector2D(index * 4, index * 4 + 1));
        QCOMPARE(v3arrayb[index],
                 QVector3D(index * 4, index * 4 + 1, index * 4 + 2));
        QCOMPARE(v4arrayb[index],
                 QVector4D(index * 4, index * 4 + 1,
                           index * 4 + 2, index * 4 + 3));
    }

    // Perform the test again, with translated() this time.

    v2array = v2arrayb.translated(translation.toVector2D());
    v3array = v3arrayb.translated(translation.toVector3D());
    v4array = v4arrayb.translated(translation);

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y()));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z()));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z(),
                           (index * 4 + 3) + translation.w()));
        QCOMPARE(v2arrayb[index],
                 QVector2D(index * 4, index * 4 + 1));
        QCOMPARE(v3arrayb[index],
                 QVector3D(index * 4, index * 4 + 1, index * 4 + 2));
        QCOMPARE(v4arrayb[index],
                 QVector4D(index * 4, index * 4 + 1,
                           index * 4 + 2, index * 4 + 3));
    }
}

void tst_QVectorArray::translateExplicit_data()
{
    translateVector_data();
}

void tst_QVectorArray::translateExplicit()
{
    QFETCH(QVector4D, translation);

    QVector2DArray v2array;
    QVector3DArray v3array;
    QVector4DArray v4array;
    QVector2DArray v2arrayb;
    QVector3DArray v3arrayb;
    QVector4DArray v4arrayb;

    for (int index = 0; index < 64; ++index) {
        v2array.append(index * 4, index * 4 + 1);
        v3array.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4array.append(index * 4, index * 4 + 1, index * 4 + 2,
                       index * 4 + 3);
        v2arrayb.append(index * 4, index * 4 + 1);
        v3arrayb.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4arrayb.append(index * 4, index * 4 + 1, index * 4 + 2,
                        index * 4 + 3);
    }

    // Perform a simple in-place translate.

    v2array.translate(translation.x(), translation.y());
    v3array.translate(translation.x(), translation.y(), translation.z());
    v4array.translate(translation.x(), translation.y(),
                      translation.z(), translation.w());

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y()));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z()));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z(),
                           (index * 4 + 3) + translation.w()));
    }

    // Perform the test again, with translated() this time.

    v2array = v2arrayb.translated(translation.x(), translation.y());
    v3array = v3arrayb.translated(translation.x(), translation.y(),
                                  translation.z());
    v4array = v4arrayb.translated(translation.x(), translation.y(),
                                  translation.z(), translation.w());

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QCOMPARE(v2array[index],
                 QVector2D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y()));
        QCOMPARE(v3array[index],
                 QVector3D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z()));
        QCOMPARE(v4array[index],
                 QVector4D((index * 4) + translation.x(),
                           (index * 4 + 1) + translation.y(),
                           (index * 4 + 2) + translation.z(),
                           (index * 4 + 3) + translation.w()));
        QCOMPARE(v2arrayb[index],
                 QVector2D(index * 4, index * 4 + 1));
        QCOMPARE(v3arrayb[index],
                 QVector3D(index * 4, index * 4 + 1, index * 4 + 2));
        QCOMPARE(v4arrayb[index],
                 QVector4D(index * 4, index * 4 + 1,
                           index * 4 + 2, index * 4 + 3));
    }
}

static bool fuzzyCompare(const QVector2D &v1, const QVector2D &v2)
{
    return qAbs(v1.x() - v2.x()) <= 0.00001 &&
           qAbs(v1.y() - v2.y()) <= 0.00001;
}

static bool fuzzyCompare(const QVector3D &v1, const QVector3D &v2)
{
    return qAbs(v1.x() - v2.x()) <= 0.00001 &&
           qAbs(v1.y() - v2.y()) <= 0.00001 &&
           qAbs(v1.z() - v2.z()) <= 0.00001;
}

static bool fuzzyCompare(const QVector4D &v1, const QVector4D &v2)
{
    return qAbs(v1.x() - v2.x()) <= 0.00001 &&
           qAbs(v1.y() - v2.y()) <= 0.00001 &&
           qAbs(v1.z() - v2.z()) <= 0.00001 &&
           qAbs(v1.w() - v2.w()) <= 0.00001;
}

void tst_QVectorArray::transform()
{
    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);

    QVector2DArray v2array;
    QVector3DArray v3array;
    QVector4DArray v4array;
    QVector2DArray v2arrayb;
    QVector3DArray v3arrayb;
    QVector4DArray v4arrayb;

    for (int index = 0; index < 64; ++index) {
        v2array.append(index * 4, index * 4 + 1);
        v3array.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4array.append(index * 4, index * 4 + 1, index * 4 + 2,
                       index * 4 + 3);
        v2arrayb.append(index * 4, index * 4 + 1);
        v3arrayb.append(index * 4, index * 4 + 1, index * 4 + 2);
        v4arrayb.append(index * 4, index * 4 + 1, index * 4 + 2,
                        index * 4 + 3);
    }

    // Perform a simple in-place transform.

    v2array.transform(m);
    v3array.transform(m);
    v4array.transform(m);

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QVector2D v2(index * 4, index * 4 + 1);
        QVector3D v3(index * 4, index * 4 + 1, index * 4 + 2);
        QVector4D v4(index * 4, index * 4 + 1, index * 4 + 2, index * 4 + 3);
        QVERIFY(fuzzyCompare(v2array[index], (m * QVector3D(v2)).toVector2D()));
        QVERIFY(fuzzyCompare(v3array[index], m * v3));
        QVERIFY(fuzzyCompare(v4array[index], m * v4));
    }

    // Increase ref-count on an array and check that detach occurs.

    v2array = v2arrayb;
    v3array = v3arrayb;
    v4array = v4arrayb;

    QVERIFY(v2array.constData() == v2arrayb.constData());
    QVERIFY(v3array.constData() == v3arrayb.constData());
    QVERIFY(v4array.constData() == v4arrayb.constData());

    v2array.transform(m);
    v3array.transform(m);
    v4array.transform(m);

    QVERIFY(v2array.constData() != v2arrayb.constData());
    QVERIFY(v3array.constData() != v3arrayb.constData());
    QVERIFY(v4array.constData() != v4arrayb.constData());

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QVector2D v2(index * 4, index * 4 + 1);
        QVector3D v3(index * 4, index * 4 + 1, index * 4 + 2);
        QVector4D v4(index * 4, index * 4 + 1, index * 4 + 2, index * 4 + 3);
        QVERIFY(fuzzyCompare(v2array[index], (m * QVector3D(v2)).toVector2D()));
        QVERIFY(fuzzyCompare(v3array[index], m * v3));
        QVERIFY(fuzzyCompare(v4array[index], m * v4));
        QVERIFY(fuzzyCompare(v2arrayb[index], v2));
        QVERIFY(fuzzyCompare(v3arrayb[index], v3));
        QVERIFY(fuzzyCompare(v4arrayb[index], v4));
    }

    // Perform the test again, with translated() this time.

    v2array = v2arrayb.transformed(m);
    v3array = v3arrayb.transformed(m);
    v4array = v4arrayb.transformed(m);

    QCOMPARE(v2array.size(), 64);
    QCOMPARE(v3array.size(), 64);
    QCOMPARE(v4array.size(), 64);

    for (int index = 0; index < 64; ++index) {
        QVector2D v2(index * 4, index * 4 + 1);
        QVector3D v3(index * 4, index * 4 + 1, index * 4 + 2);
        QVector4D v4(index * 4, index * 4 + 1, index * 4 + 2, index * 4 + 3);
        QVERIFY(fuzzyCompare(v2array[index], (m * QVector3D(v2)).toVector2D()));
        QVERIFY(fuzzyCompare(v3array[index], m * v3));
        QVERIFY(fuzzyCompare(v4array[index], m * v4));
        QVERIFY(fuzzyCompare(v2arrayb[index], v2));
        QVERIFY(fuzzyCompare(v3arrayb[index], v3));
        QVERIFY(fuzzyCompare(v4arrayb[index], v4));
    }
}

QTEST_APPLESS_MAIN(tst_QVectorArray)

#include "tst_qvectorarray.moc"
