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
#include "qstereoimage.h"
#include <QtGui/qpainter.h>

class tst_QStereoImage : public QObject
{
    Q_OBJECT
public:
    tst_QStereoImage() {}
    ~tst_QStereoImage() {}

private slots:
    void separateNonEqual_data();
    void separateNonEqual();
    void createAndConvert_data();
    void createAndConvert();
    void painting_data();
    void painting();

private:
    static QImage generateImage
        (int width, int height, const QColor &color,
         QImage::Format format = QImage::Format_RGB32);
    static QImage generateStereoPair
        (int width, int height, const QColor &lcolor, const QColor &rcolor,
         QStereoImage::Layout layout = QStereoImage::LeftRight,
         QImage::Format format = QImage::Format_RGB32);
};

QImage tst_QStereoImage::generateImage
    (int width, int height, const QColor &color, QImage::Format format)
{
    QImage image(width, height, format);
    image.fill(color.rgba());
    return image;
}

QImage tst_QStereoImage::generateStereoPair
    (int width, int height, const QColor &lcolor, const QColor &rcolor,
     QStereoImage::Layout layout, QImage::Format format)
{
    QImage image;
    QPoint left, right;

    switch (layout) {
    default:
    case QStereoImage::LeftRight:
        image = QImage(width * 2, height, format);
        left = QPoint(0, 0);
        right = QPoint(width, 0);
        break;

    case QStereoImage::RightLeft:
        image = QImage(width * 2, height, format);
        left = QPoint(width, 0);
        right = QPoint(0, 0);
        break;

    case QStereoImage::TopBottom:
        image = QImage(width, height * 2, format);
        left = QPoint(0, 0);
        right = QPoint(0, height);
        break;

    case QStereoImage::BottomTop:
        image = QImage(width, height * 2, format);
        left = QPoint(0, height);
        right = QPoint(0, 0);
        break;
    }

    uint lpixel = lcolor.rgba();
    uint rpixel = rcolor.rgba();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image.setPixel(x + left.x(), y + left.y(), lpixel);
            image.setPixel(x + right.x(), y + right.y(), rpixel);
        }
    }

    return image;
}

// Test creation of a separate image with non-equal sizes for left
// and right images, and check that conversions do what we expect.
void tst_QStereoImage::separateNonEqual_data()
{
    QTest::addColumn<int>("toLayout");

    QTest::newRow("Separate -> LeftRight")
        << int(QStereoImage::LeftRight);
    QTest::newRow("Separate -> RightLeft")
        << int(QStereoImage::RightLeft);
    QTest::newRow("Separate -> TopBottom")
        << int(QStereoImage::TopBottom);
    QTest::newRow("Separate -> BottomTop")
        << int(QStereoImage::BottomTop);
}
void tst_QStereoImage::separateNonEqual()
{
    QFETCH(int, toLayout);

    QStereoImage::Layout to = QStereoImage::Layout(toLayout);

    QStereoImage image1;
    QVERIFY(image1.isNull());
    QCOMPARE(image1.size(), QSize(0, 0));
    QCOMPARE(image1.width(), 0);
    QCOMPARE(image1.height(), 0);
    QVERIFY(image1.layout() == QStereoImage::Separate);

    QImage left(generateImage(120, 100, Qt::red));
    QImage right(generateImage(125, 90, Qt::blue));

    image1.setLeftImage(left);

    QVERIFY(!image1.isNull());
    QCOMPARE(image1.size(), QSize(120, 100));
    QCOMPARE(image1.width(), 120);
    QCOMPARE(image1.height(), 100);
    QVERIFY(image1.layout() == QStereoImage::Separate);
    QCOMPARE(image1.leftImage(), left);
    QVERIFY(image1.rightImage().isNull());
    QVERIFY(image1.wholeImage().isNull());

    QStereoImage image2(image1);

    image1.setRightImage(right);

    QVERIFY(!image1.isNull());
    QCOMPARE(image1.size(), QSize(125, 100));   // Expands to max(left,right).
    QCOMPARE(image1.width(), 125);
    QCOMPARE(image1.height(), 100);
    QVERIFY(image1.layout() == QStereoImage::Separate);
    QCOMPARE(image1.leftImage(), left);
    QCOMPARE(image1.rightImage(), right);
    QVERIFY(image1.wholeImage().isNull());

    QVERIFY(!image2.isNull());
    QCOMPARE(image2.size(), QSize(120, 100));
    QCOMPARE(image2.width(), 120);
    QCOMPARE(image2.height(), 100);
    QVERIFY(image2.layout() == QStereoImage::Separate);
    QCOMPARE(image2.leftImage(), left);
    QVERIFY(image2.rightImage().isNull());
    QVERIFY(image2.wholeImage().isNull());

    QImage wholeTo(generateStereoPair(125, 100, Qt::red, Qt::blue, to));

    image1.setLayout(to);
    QVERIFY(!image1.isNull());
    QCOMPARE(image1.size(), QSize(125, 100));
    QCOMPARE(image1.width(), 125);
    QCOMPARE(image1.height(), 100);
    QVERIFY(image1.layout() == to);
    QCOMPARE(image1.leftImage().size(), QSize(125, 100));
    QCOMPARE(image1.leftImage().copy(0, 0, 120, 100), left);
    QCOMPARE(image1.rightImage().size(), QSize(125, 100));
    QCOMPARE(image1.rightImage().copy(0, 0, 125, 90), right);
    QCOMPARE(image1.wholeImage().size(), wholeTo.size());
}

// Test creating stereo images in various formats and converting
// them to all other possible formats.
void tst_QStereoImage::createAndConvert_data()
{
    QTest::addColumn<int>("fromLayout");
    QTest::addColumn<int>("toLayout");

    // Foo -> Separate is handled in createAndConvert() below, so we
    // don't need to explicitly list it here.

    QTest::newRow("Separate -> LeftRight")
        << int(QStereoImage::Separate)
        << int(QStereoImage::LeftRight);
    QTest::newRow("Separate -> RightLeft")
        << int(QStereoImage::Separate)
        << int(QStereoImage::RightLeft);
    QTest::newRow("Separate -> TopBottom")
        << int(QStereoImage::Separate)
        << int(QStereoImage::TopBottom);
    QTest::newRow("Separate -> BottomTop")
        << int(QStereoImage::Separate)
        << int(QStereoImage::BottomTop);

    QTest::newRow("LeftRight -> LeftRight")
        << int(QStereoImage::LeftRight)
        << int(QStereoImage::LeftRight);
    QTest::newRow("LeftRight -> RightLeft")
        << int(QStereoImage::LeftRight)
        << int(QStereoImage::RightLeft);
    QTest::newRow("LeftRight -> TopBottom")
        << int(QStereoImage::LeftRight)
        << int(QStereoImage::TopBottom);
    QTest::newRow("LeftRight -> BottomTop")
        << int(QStereoImage::LeftRight)
        << int(QStereoImage::BottomTop);

    QTest::newRow("RightLeft -> LeftRight")
        << int(QStereoImage::RightLeft)
        << int(QStereoImage::LeftRight);
    QTest::newRow("RightLeft -> RightLeft")
        << int(QStereoImage::RightLeft)
        << int(QStereoImage::RightLeft);
    QTest::newRow("RightLeft -> TopBottom")
        << int(QStereoImage::RightLeft)
        << int(QStereoImage::TopBottom);
    QTest::newRow("RightLeft -> BottomTop")
        << int(QStereoImage::RightLeft)
        << int(QStereoImage::BottomTop);

    QTest::newRow("TopBottom -> LeftRight")
        << int(QStereoImage::TopBottom)
        << int(QStereoImage::LeftRight);
    QTest::newRow("TopBottom -> RightLeft")
        << int(QStereoImage::TopBottom)
        << int(QStereoImage::RightLeft);
    QTest::newRow("TopBottom -> TopBottom")
        << int(QStereoImage::TopBottom)
        << int(QStereoImage::TopBottom);
    QTest::newRow("TopBottom -> BottomTop")
        << int(QStereoImage::TopBottom)
        << int(QStereoImage::BottomTop);

    QTest::newRow("BottomTop -> LeftRight")
        << int(QStereoImage::BottomTop)
        << int(QStereoImage::LeftRight);
    QTest::newRow("BottomTop -> RightLeft")
        << int(QStereoImage::BottomTop)
        << int(QStereoImage::RightLeft);
    QTest::newRow("BottomTop -> TopBottom")
        << int(QStereoImage::BottomTop)
        << int(QStereoImage::TopBottom);
    QTest::newRow("BottomTop -> BottomTop")
        << int(QStereoImage::BottomTop)
        << int(QStereoImage::BottomTop);
}

void tst_QStereoImage::createAndConvert()
{
    QFETCH(int, fromLayout);
    QFETCH(int, toLayout);

    QStereoImage::Layout from = QStereoImage::Layout(fromLayout);
    QStereoImage::Layout to = QStereoImage::Layout(toLayout);

    QStereoImage image2;

    QStereoImage image1(QSize(120, 100), QImage::Format_RGB32, from);
    QVERIFY(!image1.isNull());
    QCOMPARE(image1.size(), QSize(120, 100));
    QCOMPARE(image1.width(), 120);
    QCOMPARE(image1.height(), 100);
    QVERIFY(image1.layout() == from);
    QCOMPARE(image1.leftImage().size(), QSize(120, 100));
    QCOMPARE(image1.rightImage().size(), QSize(120, 100));
    if (from == QStereoImage::Separate)
        QVERIFY(image1.wholeImage().isNull());
    else if (from == QStereoImage::LeftRight || from == QStereoImage::RightLeft)
        QCOMPARE(image1.wholeImage().size(), QSize(240, 100));
    else
        QCOMPARE(image1.wholeImage().size(), QSize(120, 200));

    QImage left(generateImage(120, 100, Qt::red));
    QImage right(generateImage(120, 100, Qt::blue));
    QImage whole(generateStereoPair(120, 100, Qt::red, Qt::blue, from));
    QImage wholeTo(generateStereoPair(120, 100, Qt::red, Qt::blue, to));

    image1.setLeftImage(left);
    image1.setRightImage(right);

    QCOMPARE(image1.leftImage(), left);
    QCOMPARE(image1.rightImage(), right);
    if (from == QStereoImage::Separate)
        QVERIFY(image1.wholeImage().isNull());
    else
        QCOMPARE(image1.wholeImage(), whole);

    image2 = image1;

    // Check separate conversion, which will also test copy-on-write.
    image1.setLayout(QStereoImage::Separate);

    QCOMPARE(image1.leftImage(), left);
    QCOMPARE(image1.rightImage(), right);
    QVERIFY(image1.wholeImage().isNull());
    QVERIFY(!image1.isNull());
    QCOMPARE(image1.size(), QSize(120, 100));
    QCOMPARE(image1.width(), 120);
    QCOMPARE(image1.height(), 100);
    QVERIFY(image1.layout() == QStereoImage::Separate);

    QVERIFY(image2.layout() == from);

    image2.setLayout(to);
    QVERIFY(!image2.isNull());
    QCOMPARE(image2.size(), QSize(120, 100));
    QCOMPARE(image2.width(), 120);
    QCOMPARE(image2.height(), 100);
    QVERIFY(image2.layout() == to);
    QCOMPARE(image2.leftImage(), left);
    QCOMPARE(image2.rightImage(), right);
    QCOMPARE(image2.wholeImage(), wholeTo);

    // Test creation with width and height instead of QSize.
    QStereoImage image3(120, 100, QImage::Format_RGB32, from);
    QVERIFY(!image3.isNull());
    QCOMPARE(image3.size(), QSize(120, 100));
    QCOMPARE(image3.width(), 120);
    QCOMPARE(image3.height(), 100);
    QVERIFY(image3.layout() == from);
    image3.setLeftImage(left);
    image3.setRightImage(right);
    QCOMPARE(image3.leftImage(), left);
    QCOMPARE(image3.rightImage(), right);
    if (from == QStereoImage::Separate)
        QVERIFY(image3.wholeImage().isNull());
    else
        QCOMPARE(image3.wholeImage(), whole);
}

// Test painting into the two halves of a stereo image.
void tst_QStereoImage::painting_data()
{
    QTest::addColumn<int>("layoutValue");

    QTest::newRow("Separate")
        << int(QStereoImage::Separate);
    QTest::newRow("LeftRight")
        << int(QStereoImage::LeftRight);
    QTest::newRow("RightLeft")
        << int(QStereoImage::RightLeft);
    QTest::newRow("TopBottom")
        << int(QStereoImage::TopBottom);
    QTest::newRow("BottomTop")
        << int(QStereoImage::BottomTop);
}
void tst_QStereoImage::painting()
{
    QFETCH(int, layoutValue);

    QStereoImage::Layout layout = QStereoImage::Layout(layoutValue);

    QStereoImage image1(QSize(120, 100), QImage::Format_ARGB32, layout);
    QVERIFY(image1.layout() == layout);

    QImage left(generateImage(120, 100, Qt::red, QImage::Format_ARGB32));
    QImage right(generateImage(120, 100, Qt::blue, QImage::Format_ARGB32));
    QImage leftTo(generateImage(120, 100, Qt::green, QImage::Format_ARGB32));
    QImage rightTo(generateImage(120, 100, Qt::yellow, QImage::Format_ARGB32));

    image1.setLeftImage(left);
    image1.setRightImage(right);

    // Fill a rectangle that is partially out of bounds to check
    // that painting is actually being constrained by a clip region.
    QPainter leftPainter;
    image1.beginPaintingLeft(&leftPainter);
    leftPainter.fillRect(-10, -10, 600, 300, Qt::green);
    leftPainter.end();

    QCOMPARE(image1.leftImage(), leftTo);
    QCOMPARE(image1.rightImage(), right);

    QPainter rightPainter;
    image1.beginPaintingRight(&rightPainter);
    rightPainter.fillRect(-10, -10, 600, 300, Qt::yellow);
    rightPainter.end();

    QCOMPARE(image1.leftImage(), leftTo);
    QCOMPARE(image1.rightImage(), rightTo);

    // Draw the left and right halves into a different painter.
    QImage whole(generateStereoPair(120, 100, Qt::green, Qt::yellow,
                                    QStereoImage::LeftRight,
                                    QImage::Format_ARGB32));
    QImage image2(240, 100, QImage::Format_ARGB32);
    QPainter painter(&image2);
    image1.drawLeft(&painter, QRectF(0, 0, 120, 100));
    image1.drawRight(&painter, QRectF(120, 0, 120, 100));
    painter.end();
    QCOMPARE(image2, whole);
}

QTEST_APPLESS_MAIN(tst_QStereoImage)

#include "tst_qstereoimage.moc"
