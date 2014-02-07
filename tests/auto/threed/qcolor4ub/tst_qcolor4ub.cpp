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
#include "qcolor4ub.h"

class tst_QColor4ub : public QObject
{
    Q_OBJECT
public:
    tst_QColor4ub() {}
    ~tst_QColor4ub() {}

private slots:
    void create();
    void modify();
    void copy();
    void compare_data();
    void compare();
    void toColor();
    void checkSize();
};

#define fuzzyCompare(x,y)   (qAbs((x) - (y)) < 0.0001)

void tst_QColor4ub::create()
{
    QColor4ub c1;
    QCOMPARE(c1.red(), 0);
    QCOMPARE(c1.green(), 0);
    QCOMPARE(c1.blue(), 0);
    QCOMPARE(c1.alpha(), 255);
    QCOMPARE(c1.redF(), 0.0f);
    QCOMPARE(c1.greenF(), 0.0f);
    QCOMPARE(c1.blueF(), 0.0f);
    QCOMPARE(c1.alphaF(), 1.0f);

    QColor4ub c2(162, 54, 39);
    QCOMPARE(c2.red(), 162);
    QCOMPARE(c2.green(), 54);
    QCOMPARE(c2.blue(), 39);
    QCOMPARE(c2.alpha(), 255);
    QVERIFY(fuzzyCompare(c2.redF(), 162 / 255.0f));
    QVERIFY(fuzzyCompare(c2.greenF(), 54 / 255.0f));
    QVERIFY(fuzzyCompare(c2.blueF(), 39 / 255.0f));
    QVERIFY(fuzzyCompare(c2.alphaF(), 1.0));

    QColor4ub c3(162, 54, 39, 41);
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 41);
    QVERIFY(fuzzyCompare(c3.redF(), 162 / 255.0f));
    QVERIFY(fuzzyCompare(c3.greenF(), 54 / 255.0f));
    QVERIFY(fuzzyCompare(c3.blueF(), 39 / 255.0f));
    QVERIFY(fuzzyCompare(c3.alphaF(), 41 / 255.0f));

    QColor4ub c4(QColor(162, 54, 39, 41));
    QCOMPARE(c4.red(), 162);
    QCOMPARE(c4.green(), 54);
    QCOMPARE(c4.blue(), 39);
    QCOMPARE(c4.alpha(), 41);

    QColor4ub c5(Qt::red);
    QCOMPARE(c5.red(), 255);
    QCOMPARE(c5.green(), 0);
    QCOMPARE(c5.blue(), 0);
    QCOMPARE(c5.alpha(), 255);

    QColor4ub c6(qRgb(162, 54, 39));
    QCOMPARE(c6.red(), 162);
    QCOMPARE(c6.green(), 54);
    QCOMPARE(c6.blue(), 39);
    QCOMPARE(c6.alpha(), 255);

    QColor4ub c7(qRgba(162, 54, 39, 41));
    QCOMPARE(c7.red(), 162);
    QCOMPARE(c7.green(), 54);
    QCOMPARE(c7.blue(), 39);
    QCOMPARE(c7.alpha(), 41);

    QColor4ub c8(256, -3, 1);    // truncates from int to uchar
    QCOMPARE(c8.red(), 0);
    QCOMPARE(c8.green(), 253);
    QCOMPARE(c8.blue(), 1);
    QCOMPARE(c8.alpha(), 255);

    QColor4ub c9 = QColor4ub::fromRgb(162, 54, 39);
    QCOMPARE(c9.red(), 162);
    QCOMPARE(c9.green(), 54);
    QCOMPARE(c9.blue(), 39);
    QCOMPARE(c9.alpha(), 255);
    c9 = QColor4ub::fromRgb(162, 54, 39, 41);
    QCOMPARE(c9.red(), 162);
    QCOMPARE(c9.green(), 54);
    QCOMPARE(c9.blue(), 39);
    QCOMPARE(c9.alpha(), 41);

    QColor4ub c10 = QColor4ub::fromRgbF(1.0f, 0.5f, 0.25f);
    QCOMPARE(c10.red(), 255);
    QCOMPARE(c10.green(), 128);
    QCOMPARE(c10.blue(), 64);
    QCOMPARE(c10.alpha(), 255);
    c10 = QColor4ub::fromRgbF(1.0f, 0.5f, 0.25f, 0.75f);
    QCOMPARE(c10.red(), 255);
    QCOMPARE(c10.green(), 128);
    QCOMPARE(c10.blue(), 64);
    QCOMPARE(c10.alpha(), 191);

    static uchar const data[4] = {uchar(162), uchar(54), uchar(39), uchar(255)};
    QColor4ub c11 = QColor4ub::fromRaw(data);
    QCOMPARE(c11.red(), 162);
    QCOMPARE(c11.green(), 54);
    QCOMPARE(c11.blue(), 39);
    QCOMPARE(c11.alpha(), 255);
}

void tst_QColor4ub::modify()
{
    QColor4ub c1(162, 54, 39, 41);
    c1.setRed(34);
    c1.setGreen(163);
    c1.setBlue(2);
    c1.setAlpha(200);
    QCOMPARE(c1.red(), 34);
    QCOMPARE(c1.green(), 163);
    QCOMPARE(c1.blue(), 2);
    QCOMPARE(c1.alpha(), 200);
    QVERIFY(fuzzyCompare(c1.redF(), 34 / 255.0f));
    QVERIFY(fuzzyCompare(c1.greenF(), 163 / 255.0f));
    QVERIFY(fuzzyCompare(c1.blueF(), 2 / 255.0f));
    QVERIFY(fuzzyCompare(c1.alphaF(), 200 / 255.0f));

    QColor4ub c2;
    c2.setRedF(1.0f);
    c2.setGreenF(0.5f);
    c2.setBlueF(0.25f);
    c2.setAlphaF(0.75f);
    QCOMPARE(c2.red(), 255);
    QCOMPARE(c2.green(), 128);
    QCOMPARE(c2.blue(), 64);
    QCOMPARE(c2.alpha(), 191);
    QVERIFY(fuzzyCompare(c2.redF(), 1.0f));
    QVERIFY(fuzzyCompare(c2.greenF(), 128 / 255.0f));
    QVERIFY(fuzzyCompare(c2.blueF(), 64 / 255.0f));
    QVERIFY(fuzzyCompare(c2.alphaF(), 191 / 255.0f));

    QColor4ub c3;
    c3.setRgb(162, 54, 39);
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 255);
    c3.setRgb(162, 54, 39, 41);
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 41);

    QColor4ub c4;
    c4.setRgbF(1.0f, 0.5f, 0.25f);
    QCOMPARE(c4.red(), 255);
    QCOMPARE(c4.green(), 128);
    QCOMPARE(c4.blue(), 64);
    QCOMPARE(c4.alpha(), 255);
    c4.setRgbF(1.0f, 0.5f, 0.25f, 0.75f);
    QCOMPARE(c4.red(), 255);
    QCOMPARE(c4.green(), 128);
    QCOMPARE(c4.blue(), 64);
    QCOMPARE(c4.alpha(), 191);
}

void tst_QColor4ub::copy()
{
    QColor4ub c1(162, 54, 39, 41);
    QColor4ub c2(c1);
    QCOMPARE(c2.red(), 162);
    QCOMPARE(c2.green(), 54);
    QCOMPARE(c2.blue(), 39);
    QCOMPARE(c2.alpha(), 41);

    QColor4ub c3;
    c3 = c2;
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 41);

    c1 = Qt::red;
    QCOMPARE(c1.red(), 255);
    QCOMPARE(c1.green(), 0);
    QCOMPARE(c1.blue(), 0);
    QCOMPARE(c1.alpha(), 255);

    c2 = QColor(162, 54, 39, 41);
    QCOMPARE(c2.red(), 162);
    QCOMPARE(c2.green(), 54);
    QCOMPARE(c2.blue(), 39);
    QCOMPARE(c2.alpha(), 41);

    c1 = c2;
    QCOMPARE(c1.red(), 162);
    QCOMPARE(c1.green(), 54);
    QCOMPARE(c1.blue(), 39);
    QCOMPARE(c1.alpha(), 41);
}

void tst_QColor4ub::compare_data()
{
    QTest::addColumn<int>("red1");
    QTest::addColumn<int>("green1");
    QTest::addColumn<int>("blue1");
    QTest::addColumn<int>("alpha1");
    QTest::addColumn<int>("red2");
    QTest::addColumn<int>("green2");
    QTest::addColumn<int>("blue2");
    QTest::addColumn<int>("alpha2");
    QTest::addColumn<bool>("isEqual");

    QTest::newRow("black")
        << 0 << 0 << 0 << 0
        << 0 << 0 << 0 << 0
        << true;

    QTest::newRow("white")
        << 255 << 255 << 255 << 255
        << 255 << 255 << 255 << 255
        << true;

    QTest::newRow("mixed")
        << 162 << 54 << 39 << 41
        << 162 << 54 << 39 << 41
        << true;

    QTest::newRow("red-diff")
        << 162 << 54 << 39 << 41
        << 161 << 54 << 39 << 41
        << false;

    QTest::newRow("green-diff")
        << 162 << 54 << 39 << 41
        << 162 << 53 << 39 << 41
        << false;

    QTest::newRow("blue-diff")
        << 162 << 54 << 39 << 41
        << 162 << 54 << 38 << 41
        << false;

    QTest::newRow("alpha-diff")
        << 162 << 54 << 39 << 41
        << 162 << 54 << 39 << 40
        << false;

    QTest::newRow("all-diff")
        << 162 << 54 << 39 << 41
        << 161 << 53 << 38 << 40
        << false;
}

void tst_QColor4ub::compare()
{
    QFETCH(int, red1);
    QFETCH(int, green1);
    QFETCH(int, blue1);
    QFETCH(int, alpha1);
    QFETCH(int, red2);
    QFETCH(int, green2);
    QFETCH(int, blue2);
    QFETCH(int, alpha2);
    QFETCH(bool, isEqual);

    QColor4ub c1(red1, green1, blue1, alpha1);
    QColor4ub c2(red2, green2, blue2, alpha2);

    if (isEqual) {
        QVERIFY(c1 == c2);
        QVERIFY(!(c1 != c2));
    } else {
        QVERIFY(!(c1 == c2));
        QVERIFY(c1 != c2);
    }
}

void tst_QColor4ub::toColor()
{
    QColor4ub c1(162, 54, 39, 41);
    QColor c2 = c1.toColor();
    QCOMPARE(c2.red(), 162);
    QCOMPARE(c2.green(), 54);
    QCOMPARE(c2.blue(), 39);
    QCOMPARE(c2.alpha(), 41);
}

void tst_QColor4ub::checkSize()
{
    // We need QColor4ub to be precisely packed as four single-byte values.
    QCOMPARE(sizeof(QColor4ub), sizeof(uchar) * 4);
}

QTEST_APPLESS_MAIN(tst_QColor4ub)

#include "tst_qcolor4ub.moc"
