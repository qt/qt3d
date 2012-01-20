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
#include "qcustomdataarray.h"

class tst_QCustomDataArray : public QObject
{
    Q_OBJECT
public:
    tst_QCustomDataArray() {}
    ~tst_QCustomDataArray() {}

private slots:
    void create();
};

void tst_QCustomDataArray::create()
{
    QCustomDataArray array1;
    QVERIFY(array1.elementType() == QCustomDataArray::Float);
    QVERIFY(array1.elementSize() == sizeof(float));
    QCOMPARE(array1.size(), 0);
    QCOMPARE(array1.count(), 0);
    QVERIFY(array1.isEmpty());

    array1.setElementType(QCustomDataArray::Vector3D);
    QVERIFY(array1.elementType() == QCustomDataArray::Vector3D);
    QVERIFY(array1.elementSize() == sizeof(QVector3D));
    QCOMPARE(array1.size(), 0);
    QCOMPARE(array1.count(), 0);
    QVERIFY(array1.isEmpty());

    QCustomDataArray array2(QCustomDataArray::Vector2D);
    QVERIFY(array2.elementType() == QCustomDataArray::Vector2D);
    QVERIFY(array2.elementSize() == sizeof(QVector2D));
    QCOMPARE(array2.size(), 0);
    QCOMPARE(array2.count(), 0);
    QVERIFY(array2.isEmpty());

    QCustomDataArray array3(QCustomDataArray::Vector3D, 20);
    QVERIFY(array3.elementType() == QCustomDataArray::Vector3D);
    QVERIFY(array3.elementSize() == sizeof(QVector3D));
    QCOMPARE(array3.size(), 20);
    QCOMPARE(array3.count(), 20);
    QVERIFY(!array3.isEmpty());
    QVERIFY(array3.capacity() >= 20);

    QCustomDataArray array4(QCustomDataArray::Vector4D, 20);
    QVERIFY(array4.elementType() == QCustomDataArray::Vector4D);
    QVERIFY(array4.elementSize() == sizeof(QVector4D));
    QCOMPARE(array4.size(), 20);
    QCOMPARE(array4.count(), 20);
    QVERIFY(!array4.isEmpty());
    array4.append(QVector4D(1.0f, 2.0f, 3.0f, 4.0f));

    QCustomDataArray array5(QCustomDataArray::Color, 10);
    QVERIFY(array5.elementType() == QCustomDataArray::Color);
    QVERIFY(array5.elementSize() == sizeof(QColor4ub));
    QCOMPARE(array5.size(), 10);
    QCOMPARE(array5.count(), 10);
    QVERIFY(!array5.isEmpty());
    array5.append(Qt::red);
    array5.setAt(4, Qt::green);
    QVERIFY(array5.colorAt(4) == Qt::green);

    QCustomDataArray array6(array5);
    QVERIFY(array6.elementType() == QCustomDataArray::Color);
    QVERIFY(array6.elementSize() == sizeof(QColor4ub));
    QCOMPARE(array6.size(), 11);
    QCOMPARE(array6.count(), 11);
    QVERIFY(!array6.isEmpty());
    QVERIFY(array6.colorAt(10) == Qt::red);

    // Changes element type from Color to Vector4D.
    array6 = array4;
    QVERIFY(array6.elementType() == QCustomDataArray::Vector4D);
    QVERIFY(array6.elementSize() == sizeof(QVector4D));
    QCOMPARE(array6.size(), 21);
    QCOMPARE(array6.count(), 21);
    QVERIFY(!array6.isEmpty());
    QVERIFY(array6.vector4DAt(20) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));

    array6.clear();
    QVERIFY(array6.elementType() == QCustomDataArray::Vector4D);
    QVERIFY(array6.elementSize() == sizeof(QVector4D));
    QCOMPARE(array6.size(), 0);
    QCOMPARE(array6.count(), 0);
    QVERIFY(array6.isEmpty());

    array6.setElementType(QCustomDataArray::Vector3D);
    QVERIFY(array6.elementType() == QCustomDataArray::Vector3D);
    QVERIFY(array6.elementSize() == sizeof(QVector3D));
    QCOMPARE(array6.size(), 0);
    QCOMPARE(array6.count(), 0);
    QVERIFY(array6.isEmpty());
}

QTEST_APPLESS_MAIN(tst_QCustomDataArray)

#include "tst_qcustomdataarray.moc"
