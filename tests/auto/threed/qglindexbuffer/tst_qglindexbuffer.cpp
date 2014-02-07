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
#include "qglindexbuffer.h"

#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>

class tst_QGLIndexBuffer : public QObject
{
    Q_OBJECT
public:
    tst_QGLIndexBuffer() {}
    ~tst_QGLIndexBuffer() {}

private slots:
    void initTestCase();
    void create();
    void replaceIndexes();
    void appendSimple_data();
    void appendSimple();
    void appendCombined_data();
    void appendCombined();

private:
    QArray<ushort> indexesUShort1024;
    QArray<uint> indexesUInt1024;
    QArray<ushort> indexesUShort1024Rev;
    QArray<uint> indexesUInt1024Rev;
};

void tst_QGLIndexBuffer::initTestCase()
{
    for (uint index = 0; index < 1024; ++index) {
        indexesUShort1024.append(ushort(index));
        indexesUInt1024.append(index);
        indexesUShort1024Rev.append(ushort(1024 - index));
        indexesUInt1024Rev.append(1024 - index);
    }
}

void tst_QGLIndexBuffer::create()
{
    QGLIndexBuffer buf1;
    QVERIFY(buf1.usagePattern() == QOpenGLBuffer::StaticDraw);
    QVERIFY(buf1.elementType() == GL_UNSIGNED_SHORT);
    QVERIFY(buf1.indexesUShort().isEmpty());
    QVERIFY(buf1.indexesUInt().isEmpty());
    QVERIFY(buf1.isEmpty());
    QCOMPARE(buf1.indexCount(), 0);
    QVERIFY(!buf1.buffer().isCreated());
    QVERIFY(!buf1.isUploaded());

    // Setting an index array as ushort must stay ushort.
    buf1.setIndexes(indexesUShort1024);
    QVERIFY(buf1.usagePattern() == QOpenGLBuffer::StaticDraw);
    QVERIFY(buf1.elementType() == GL_UNSIGNED_SHORT);
    QVERIFY(buf1.indexesUShort() == indexesUShort1024);
    QVERIFY(buf1.indexesUInt().isEmpty());
    QVERIFY(!buf1.isEmpty());
    QCOMPARE(buf1.indexCount(), 1024);
    QVERIFY(!buf1.buffer().isCreated());
    QVERIFY(!buf1.isUploaded());

    // Setting an index array as uint may convert to ushort on
    // embedded systems that don't have GL_OES_element_index_uint.
    buf1.setIndexes(indexesUInt1024);
    QVERIFY(buf1.usagePattern() == QOpenGLBuffer::StaticDraw);
    if (buf1.elementType() == GL_UNSIGNED_SHORT) {
        QVERIFY(buf1.elementType() == GL_UNSIGNED_SHORT);
        QVERIFY(buf1.indexesUShort() == indexesUShort1024);
        QVERIFY(buf1.indexesUInt().isEmpty());
    } else {
        QVERIFY(buf1.elementType() == GL_UNSIGNED_INT);
        QVERIFY(buf1.indexesUShort().isEmpty());
        QVERIFY(buf1.indexesUInt() == indexesUInt1024);
    }
    QVERIFY(!buf1.isEmpty());
    QCOMPARE(buf1.indexCount(), 1024);
    QVERIFY(!buf1.buffer().isCreated());
    QVERIFY(!buf1.isUploaded());

    // Set back to ushort again.
    buf1.setIndexes(indexesUShort1024);
    QVERIFY(buf1.usagePattern() == QOpenGLBuffer::StaticDraw);
    QVERIFY(buf1.elementType() == GL_UNSIGNED_SHORT);
    QVERIFY(buf1.indexesUShort() == indexesUShort1024);
    QVERIFY(buf1.indexesUInt().isEmpty());
    QVERIFY(!buf1.isEmpty());
    QCOMPARE(buf1.indexCount(), 1024);
    QVERIFY(!buf1.buffer().isCreated());
    QVERIFY(!buf1.isUploaded());
}

void tst_QGLIndexBuffer::replaceIndexes()
{
    QGLIndexBuffer buf1;
    buf1.setIndexes(indexesUShort1024);
    buf1.replaceIndexes(0, indexesUShort1024Rev);
    QCOMPARE(buf1.indexCount(), indexesUShort1024Rev.size());
    QVERIFY(buf1.indexesUShort() == indexesUShort1024Rev);

    QArray<ushort> result1(indexesUShort1024Rev);
    result1.replace(512, indexesUShort1024.constData(), indexesUShort1024.size());
    buf1.replaceIndexes(512, indexesUShort1024);
    QVERIFY(buf1.indexesUShort() == result1);
    QCOMPARE(buf1.indexCount(), result1.size());

    QGLIndexBuffer buf2;
    buf2.setIndexes(indexesUInt1024);
    buf2.replaceIndexes(0, indexesUInt1024Rev);
    QCOMPARE(buf2.indexCount(), indexesUShort1024Rev.size());
    if (buf2.elementType() == GL_UNSIGNED_SHORT) {
        QVERIFY(buf2.indexesUShort() == indexesUShort1024Rev);
        buf2.replaceIndexes(512, indexesUInt1024);
        QVERIFY(buf2.indexesUShort() == result1);
        QCOMPARE(buf2.indexCount(), result1.size());
    } else {
        QVERIFY(buf2.indexesUInt() == indexesUInt1024Rev);
        QArray<uint> result2(indexesUInt1024Rev);
        result2.replace(512, indexesUInt1024.constData(), indexesUInt1024.size());
        buf2.replaceIndexes(512, indexesUInt1024);
        QVERIFY(buf2.indexesUInt() == result2);
        QCOMPARE(buf2.indexCount(), result2.size());
    }
}

void tst_QGLIndexBuffer::appendSimple_data()
{
    QTest::addColumn<int>("buf1Type");
    QTest::addColumn<int>("buf2Type");

    QTest::newRow("ushort, ushort")
        << int(GL_UNSIGNED_SHORT) << int(GL_UNSIGNED_SHORT);
    QTest::newRow("ushort, uint")
        << int(GL_UNSIGNED_SHORT) << int(GL_UNSIGNED_INT);
    QTest::newRow("uint, ushort")
        << int(GL_UNSIGNED_INT) << int(GL_UNSIGNED_SHORT);
    QTest::newRow("uint, uint")
        << int(GL_UNSIGNED_INT) << int(GL_UNSIGNED_INT);
}

void tst_QGLIndexBuffer::appendSimple()
{
    QFETCH(int, buf1Type);
    QFETCH(int, buf2Type);

    QGLIndexBuffer buf1;
    QGLIndexBuffer buf2;
    if (buf1Type == GL_UNSIGNED_SHORT)
        buf1.setIndexes(indexesUShort1024);
    else
        buf1.setIndexes(indexesUInt1024);
    if (buf2Type == GL_UNSIGNED_SHORT)
        buf2.setIndexes(indexesUShort1024Rev);
    else
        buf2.setIndexes(indexesUInt1024Rev);
    buf1.append(buf2, 23);

    if (buf1.elementType() == GL_UNSIGNED_SHORT) {
        QArray<ushort> result1(indexesUShort1024);
        for (int index = 0; index < indexesUShort1024Rev.size(); ++index)
            result1.append(indexesUShort1024Rev[index] + 23);
        QVERIFY(buf1.indexesUShort() == result1);
        QCOMPARE(buf1.indexCount(), result1.size());
    } else {
        QArray<uint> result2(indexesUInt1024);
        for (int index = 0; index < indexesUInt1024Rev.size(); ++index)
            result2.append(indexesUInt1024Rev[index] + 23);
        QVERIFY(buf1.indexesUInt() == result2);
        QCOMPARE(buf1.indexCount(), result2.size());
    }
}

void tst_QGLIndexBuffer::appendCombined_data()
{
    QTest::addColumn<int>("combineMode");
    QTest::addColumn<int>("offset");
    QTest::addColumn<QString>("buf1Values");
    QTest::addColumn<QString>("buf2Values");
    QTest::addColumn<QString>("resultValues");

    QTest::newRow("triangles")
        << int(QGL::Triangles) << 0
        << "0 1 2 3 4 5"
        << "4 5 6 7 8 9"
        << "0 1 2 3 4 5 4 5 6 7 8 9";

    QTest::newRow("triangles, empty A")
        << int(QGL::Triangles) << 20
        << ""
        << "4 5 6 7 8"
        << "24 25 26 27 28";

    QTest::newRow("triangles, empty B")
        << int(QGL::Triangles) << 10
        << "0 1 2 3 4 5"
        << ""
        << "0 1 2 3 4 5";

    QTest::newRow("triangle strip, even")
        << int(QGL::TriangleStrip) << 2
        << "0 1 2 3"
        << "0 1 2 3"
        << "0 1 2 3 4 5";

    QTest::newRow("triangle strip, odd")
        << int(QGL::TriangleStrip) << 0
        << "0 1 2"
        << "1 2 3 4 5"
        << "0 1 2 3 4 5";

    QTest::newRow("triangle strip, odd reversed")
        << int(QGL::TriangleStrip) << 0
        << "0 1 2"
        << "2 1 3 4 5"
        << "0 1 2 3 4 5";

    QTest::newRow("triangle strip, no common")
        << int(QGL::TriangleStrip) << 0
        << "0 1 2"
        << "3 4 5"
        << "0 1 2 3 4 5";

    QTest::newRow("triangle strip, empty A")
        << int(QGL::TriangleStrip) << 0
        << "0 1 2"
        << ""
        << "0 1 2";

    QTest::newRow("triangle strip, empty B")
        << int(QGL::TriangleStrip) << 0
        << ""
        << "0 1 2"
        << "0 1 2";

    QTest::newRow("line strip")
        << int(QGL::LineStrip) << 0
        << "0 1 2"
        << "2 3 4"
        << "0 1 2 3 4";

    QTest::newRow("line strip, offset")
        << int(QGL::LineStrip) << 2
        << "0 1 2"
        << "0 1 2"
        << "0 1 2 3 4";

    QTest::newRow("line strip, no common")
        << int(QGL::LineStrip) << 0
        << "0 1 2"
        << "3 4"
        << "0 1 2 3 4";

    QTest::newRow("line strip, empty A")
        << int(QGL::LineStrip) << 0
        << ""
        << "0 1 2"
        << "0 1 2";

    QTest::newRow("line strip, empty B")
        << int(QGL::LineStrip) << 0
        << "0 1 2"
        << ""
        << "0 1 2";

    QTest::newRow("quad strip")
        << int(0x0008) << 0
        << "0 1 2 3"
        << "2 3 4 5"
        << "0 1 2 3 4 5";

    QTest::newRow("quad strip, offset")
        << int(0x0008) << 2
        << "0 1 2 3"
        << "0 1 2 3"
        << "0 1 2 3 4 5";

    QTest::newRow("quad strip, no common")
        << int(0x0008) << 0
        << "0 1 2 3"
        << "3 2 4 5"
        << "0 1 2 3 3 2 4 5";

    QTest::newRow("triangle fan")
        << int(QGL::TriangleFan) << 0
        << "0 1 2 3"
        << "0 3 4 5"
        << "0 1 2 3 4 5";

    QTest::newRow("triangle fan, offset")
        << int(QGL::TriangleFan) << 10
        << "10 11 12 13"
        << "0 3 4 5"
        << "10 11 12 13 14 15";

    QTest::newRow("triangle fan, no common")
        << int(QGL::TriangleFan) << 0
        << "0 1 2 3"
        << "1 3 4 5"
        << "0 1 2 3 1 3 4 5";

    QTest::newRow("triangle fan, no common 2")
        << int(QGL::TriangleFan) << 0
        << "0 1 2 3"
        << "0 2 4 5"
        << "0 1 2 3 0 2 4 5";

    QTest::newRow("line strip adjacency")
        << int(QGL::LineStripAdjacency) << 0
        << "0 1 2 3"
        << "1 2 3 4"
        << "0 1 2 3 4";

    QTest::newRow("line strip adjacency, offset")
        << int(QGL::LineStripAdjacency) << 10
        << "10 11 12 13"
        << "1 2 3 4"
        << "10 11 12 13 14";

    QTest::newRow("line strip adjacency, no common")
        << int(QGL::LineStripAdjacency) << 0
        << "0 1 2 3"
        << "2 3 4 5"
        << "0 1 2 3 2 3 4 5";

    QTest::newRow("triangle strip adjacency, even")
        << int(QGL::TriangleStripAdjacency) << 0
        << "1 2 3 4 5 6"
        << "3 1 5 6 7 8"
        << "1 2 3 4 5 6 7 8";

    QTest::newRow("triangle strip adjacency, odd")
        << int(QGL::TriangleStripAdjacency) << 0
        << "1 2 3 4 5 6 7 8"
        << "5 3 7 8 9 10"
        << "1 2 3 4 5 6 7 8 9 10";

    QTest::newRow("triangle strip adjacency, even again")
        << int(QGL::TriangleStripAdjacency) << 0
        << "1 2 3 4 5 6 7 8 9 10"
        << "7 5 9 10 11 12"
        << "1 2 3 4 5 6 7 8 9 10 11 12";

    QTest::newRow("triangle strip adjacency, no common")
        << int(QGL::TriangleStripAdjacency) << 0
        << "1 2 3 4 5 6 7 8 9 10"
        << "5 7 9 10 11 12"
        << "1 2 3 4 5 6 7 8 9 10 5 7 9 10 11 12";

    QTest::newRow("triangle strip adjacency, offset")
        << int(QGL::TriangleStripAdjacency) << 100
        << "101 102 103 104 105 106 107 108 109 110"
        << "7 5 9 10 11 12"
        << "101 102 103 104 105 106 107 108 109 110 111 112";
}

static QArray<ushort> stringToUShortArray(const QString &str)
{
    QStringList list = str.split(QLatin1String(" "));
    QArray<ushort> array;
    if (str.isEmpty())
        return array;
    foreach (QString s, list)
        array.append(ushort(s.toInt()));
    return array;
}

static QArray<uint> stringToUIntArray(const QString &str)
{
    QStringList list = str.split(QLatin1String(" "));
    QArray<uint> array;
    if (str.isEmpty())
        return array;
    foreach (QString s, list)
        array.append(s.toUInt());
    return array;
}

static bool sameIndexes(const QGLIndexBuffer &buf, const QArray<ushort> &result)
{
    if (buf.elementType() == GL_UNSIGNED_SHORT) {
        return buf.indexesUShort() == result;
    } else {
        QArray<uint> result2;
        for (int index = 0; index < result.size(); ++index)
            result2.append(result[index]);
        return buf.indexesUInt() == result2;
    }
}

void tst_QGLIndexBuffer::appendCombined()
{
    QFETCH(int, combineMode);
    QFETCH(int, offset);
    QFETCH(QString, buf1Values);
    QFETCH(QString, buf2Values);
    QFETCH(QString, resultValues);

    // Test ushort, ushort appends.
    QGLIndexBuffer buf1, buf2;
    buf1.setIndexes(stringToUShortArray(buf1Values));
    buf2.setIndexes(stringToUShortArray(buf2Values));
    buf1.append(buf2, uint(offset), QGL::DrawingMode(combineMode));

    QArray<ushort> result = stringToUShortArray(resultValues);
    QVERIFY(buf1.indexesUShort() == result);
    QCOMPARE(buf1.indexCount(), result.size());

    // Test ushort, uint appends.
    QGLIndexBuffer buf3, buf4;
    buf3.setIndexes(stringToUShortArray(buf1Values));
    buf4.setIndexes(stringToUIntArray(buf2Values));
    buf3.append(buf4, uint(offset), QGL::DrawingMode(combineMode));

    QVERIFY(sameIndexes(buf3, result));
    QCOMPARE(buf3.indexCount(), result.size());

    // Test uint, ushort appends.
    QGLIndexBuffer buf5, buf6;
    buf5.setIndexes(stringToUIntArray(buf1Values));
    buf6.setIndexes(stringToUShortArray(buf2Values));
    buf5.append(buf6, uint(offset), QGL::DrawingMode(combineMode));

    QVERIFY(sameIndexes(buf5, result));
    QCOMPARE(buf5.indexCount(), result.size());

    // Test uint, uint appends.
    QGLIndexBuffer buf7, buf8;
    buf7.setIndexes(stringToUIntArray(buf1Values));
    buf8.setIndexes(stringToUIntArray(buf2Values));
    buf7.append(buf8, uint(offset), QGL::DrawingMode(combineMode));

    QVERIFY(sameIndexes(buf7, result));
    QCOMPARE(buf7.indexCount(), result.size());
}

QTEST_MAIN(tst_QGLIndexBuffer)

#include "tst_qglindexbuffer.moc"
