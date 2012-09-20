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
#include "qareaallocator.h"

class tst_QAreaAllocator : public QObject
{
    Q_OBJECT
public:
    tst_QAreaAllocator() {}
    ~tst_QAreaAllocator() {}

private slots:
    void create();
    void allocateSimple();
    void allocateGeneral();
    void allocateUniform();
};

void tst_QAreaAllocator::create()
{
    QSimpleAreaAllocator alloc1(QSize(400, 200));
    QCOMPARE(alloc1.size(), QSize(400, 200));
    QCOMPARE(alloc1.minimumAllocation(), QSize(1, 1));
    QCOMPARE(alloc1.margin(), QSize(0, 0));

    alloc1.setMinimumAllocation(QSize(5, 7));
    QCOMPARE(alloc1.minimumAllocation(), QSize(5, 7));

    alloc1.setMargin(QSize(8, 9));
    QCOMPARE(alloc1.margin(), QSize(8, 9));

    alloc1.expand(QSize(300, 21));
    QCOMPARE(alloc1.size(), QSize(400, 200));

    alloc1.expand(QSize(300, 210));
    QCOMPARE(alloc1.size(), QSize(400, 210));

    alloc1.expand(QSize(3000, 110));
    QCOMPARE(alloc1.size(), QSize(3000, 210));

    alloc1.expandBy(QSize(-1, -1000));
    QCOMPARE(alloc1.size(), QSize(3000, 210));

    alloc1.expandBy(QSize(1, 10));
    QCOMPARE(alloc1.size(), QSize(3001, 220));

    QGeneralAreaAllocator alloc2(QSize(400, 200));
    QCOMPARE(alloc2.size(), QSize(512, 256));
    QCOMPARE(alloc2.minimumAllocation(), QSize(8, 8));
    QCOMPARE(alloc2.margin(), QSize(0, 0));

    alloc2.expand(QSize(256, 96));
    QCOMPARE(alloc2.size(), QSize(512, 256));

    alloc2.expand(QSize(256, 257));
    QCOMPARE(alloc2.size(), QSize(512, 512));

    alloc2.expand(QSize(513, 257));
    QCOMPARE(alloc2.size(), QSize(1024, 512));

    alloc2.expandBy(QSize(-100, -100));
    QCOMPARE(alloc2.size(), QSize(1024, 512));

    alloc2.expandBy(QSize(1, 2));
    QCOMPARE(alloc2.size(), QSize(2048, 1024));

    QUniformAreaAllocator alloc3(QSize(400, 200), QSize(16, 16));
    QCOMPARE(alloc3.size(), QSize(400, 200));
    QCOMPARE(alloc3.minimumAllocation(), QSize(1, 1));
    QCOMPARE(alloc3.margin(), QSize(0, 0));

    alloc3.expand(QSize(256, 96));
    QCOMPARE(alloc3.size(), QSize(400, 200));

    alloc3.expand(QSize(256, 257));
    QCOMPARE(alloc3.size(), QSize(400, 257));

    alloc3.expand(QSize(513, 257));
    QCOMPARE(alloc3.size(), QSize(513, 257));

    alloc3.expandBy(QSize(-100, -100));
    QCOMPARE(alloc3.size(), QSize(513, 257));

    alloc3.expandBy(QSize(1, 2));
    QCOMPARE(alloc3.size(), QSize(514, 259));
}

void tst_QAreaAllocator::allocateSimple()
{
    // The simple allocator returns regions in a left-to-right,
    // top-to-bottom order across the image extents.
    QSimpleAreaAllocator alloc1(QSize(100, 100));
    QRect rect;
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            rect = alloc1.allocate(QSize(10, 10));
            QCOMPARE(rect, QRect(x * 10, y * 10, 10, 10));
        }
    }
    rect = alloc1.allocate(QSize(10, 10));
    QVERIFY(rect.isNull());

    // Release doesn't do anything on the simple allocator.
    alloc1.release(QRect(0, 0, 10, 10));
    rect = alloc1.allocate(QSize(10, 10));
    QVERIFY(rect.isNull());

    // Test minimum allocation sizes.
    QSimpleAreaAllocator alloc2(QSize(100, 100));
    alloc2.setMinimumAllocation(QSize(10, 5));
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            rect = alloc2.allocate(QSize(x + 1, y + 1));
            if (y < 5)
                QCOMPARE(rect, QRect(x * 10, y * 5, x + 1, y + 1));
            else
                QCOMPARE(rect, QRect(x * 10, (y - 5) * 10 + 25, x + 1, y + 1));
        }
    }
    rect = alloc2.allocate(QSize(10, 10));
    QCOMPARE(rect, QRect(0, 75, 10, 10));   // Still some space left over.
    rect = alloc2.allocate(QSize(10, 26));  // Won't fit.
    QVERIFY(rect.isNull());
    rect = alloc2.allocate(QSize(10, 10));  // Should still fit.
    QCOMPARE(rect, QRect(10, 75, 10, 10));
}

void tst_QAreaAllocator::allocateGeneral()
{
    QGeneralAreaAllocator alloc1(QSize(128, 128));
    QRect rect;
    QList<QRect> allocations;
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            // The order in which allocations are returned is not
            // as predictable as QSimpleAreaAllocator.  But we can
            // ensure that all allocations are unique and 8x8-aligned.
            rect = alloc1.allocate(QSize(8, 8));
            QVERIFY((rect.x() % 8) == 0);
            QVERIFY((rect.y() % 8) == 0);
            QVERIFY((rect.height() % 8) == 0);
            QVERIFY((rect.height() % 8) == 0);
            QVERIFY(!allocations.contains(rect));
            allocations += rect;
        }
    }
    QCOMPARE(allocations.size(), 256);
    rect = alloc1.allocate(QSize(8, 8));
    QVERIFY(rect.isNull());

    // Double the width and height and then do some more allocations at 16x16.
    alloc1.expandBy(QSize(1, 1));
    for (int z = 0; z < 3; ++z) {
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                rect = alloc1.allocate(QSize(16, 16));
                QVERIFY((rect.x() % 8) == 0);
                QVERIFY((rect.y() % 8) == 0);
                QVERIFY((rect.height() % 16) == 0);
                QVERIFY((rect.height() % 16) == 0);
                QVERIFY(!allocations.contains(rect));
                allocations += rect;
            }
        }
    }
    QCOMPARE(allocations.size(), 256 + 3 * 8 * 8);
    rect = alloc1.allocate(QSize(8, 8));
    QVERIFY(rect.isNull());

    // Release the 8x8 allocations.
    for (int y = 0; y < 16; ++y)
        for (int x = 0; x < 16; ++x)
            alloc1.release(QRect(x * 8, y * 8, 8, 8));
    rect = alloc1.allocate(QSize(8, 8));
    QVERIFY(!rect.isNull());
    alloc1.release(rect);

    // Allocate some other sizes into the gap we've just made.
    rect = alloc1.allocate(QSize(256, 256));    // Too big.
    QVERIFY(rect.isNull());
    rect = alloc1.allocate(QSize(64, 128));
    QVERIFY(!rect.isNull());
    rect = alloc1.allocate(QSize(64, 64));
    QVERIFY(!rect.isNull());
    rect = alloc1.allocate(QSize(64, 48));      // Over-allocates to 64x64.
    QVERIFY(!rect.isNull());
    rect = alloc1.allocate(QSize(8, 8));        // Will fail.
    QVERIFY(rect.isNull());
}

void tst_QAreaAllocator::allocateUniform()
{
    // The uniform allocator initially returns regions in a
    // left-to-right, top-to-bottom order across the image extents.
    QUniformAreaAllocator alloc1(QSize(100, 100), QSize(10, 10));
    QRect rect;
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            rect = alloc1.allocate(QSize(10, 10));
            QCOMPARE(rect, QRect(x * 10, y * 10, 10, 10));
        }
    }
    rect = alloc1.allocate(QSize(10, 10));
    QVERIFY(rect.isNull());

    // Release some areas and then allocate again.  Should act like a stack.
    rect = QRect(60, 50, 10, 10);
    QRect rect2 = QRect(50, 40, 10, 10);
    alloc1.release(rect);
    alloc1.release(rect2);
    QCOMPARE(alloc1.allocate(QSize(10, 10)), rect2);
    QCOMPARE(alloc1.allocate(QSize(10, 10)), rect);
    QVERIFY(alloc1.allocate(QSize(10, 10)).isNull());

    // Expand the allocation area.
    alloc1.release(rect);   // Copy across at least 1 free list entry.
    alloc1.expandBy(QSize(10, 10));
    for (int count = 0; count < 22; ++count) {
        rect = alloc1.allocate(QSize(10, 10));
        QVERIFY(!rect.isNull());
    }
    rect = alloc1.allocate(QSize(10, 10));
    QVERIFY(rect.isNull());

    // Release everything and then reallocate.
    for (int y = 0; y < 11; ++y) {
        for (int x = 0; x < 11; ++x)
            alloc1.release(QRect(x * 10, y * 10, 10, 10));
    }
    for (int count = 0; count < (11 * 11); ++count) {
        rect = alloc1.allocate(QSize(10, 10));
        QVERIFY(!rect.isNull());
    }
    rect = alloc1.allocate(QSize(10, 10));
    QVERIFY(rect.isNull());
}

QTEST_APPLESS_MAIN(tst_QAreaAllocator)

#include "tst_qareaallocator.moc"
