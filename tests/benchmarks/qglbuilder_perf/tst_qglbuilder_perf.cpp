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

#include <ctime>
#include <QtTest/QtTest>
#include "qglbuilder.h"
#include "qglteapot.h"
#include "qglsection_p.h"
#include "qgeometrydata.h"

class TestBuilder : public QGLBuilder
{
public:
    QGLSection *section() { return currentSection(); }
    void setDefThreshold(int t) { setDefaultThreshold(t); }
};

class tst_QGLBuilder : public QObject
{
    Q_OBJECT
public:
    tst_QGLBuilder() {}
    virtual ~tst_QGLBuilder() {}
    void addQuadBenchMarks(const QVector3DArray &data, int type);

private slots:
    void addQuadRandom_data();
    void addQuadRandom();
    void addQuadOrdered_data();
    void addQuadOrdered();
    void teapot();
};

enum {
    Test_3,
    Test_7,
    Test_10,
    Test_20
};

void tst_QGLBuilder::addQuadRandom_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 10; size < 10000; size += 10)
    {
        name = "T3--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_3);

        name = "T7--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_7);

        name = "T10--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_10);

        name = "T20--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_20);
    }
}

static inline float randCoord()
{
    return (200.0f * (float(qrand()) / float(RAND_MAX))) - 100.0f;
}

QVector3D randVector()
{
    static bool seeded = false;
    if (!seeded)
    {
        qsrand(time(0));
        seeded = true;
    }
    return QVector3D(randCoord(), randCoord(), randCoord());
}

void tst_QGLBuilder::addQuadRandom()
{
    QFETCH(int, size);
    QFETCH(int, type);

    int n = qSqrt(size);
    size = n * n;
    QVector3DArray data;
    data.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        // make sure (in face of randomness) we get a planar quad
        QVector3D origin = randVector();
        QVector3D a;
        while (a.isNull())
            a = randVector();
        QVector3D b;
        while (b.isNull())
            b = randVector();
        data.append(origin, a, a+b, b);
    }
    addQuadBenchMarks(data, type);
}

void tst_QGLBuilder::addQuadBenchMarks(const QVector3DArray &data, int type)
{
    int size = data.size();
    if (type == Test_3)
    {
        QBENCHMARK {
            TestBuilder builder;
            builder.newSection(QGL::Smooth);
            builder.section()->setMapThreshold(3);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGeometryData op;
                op.appendVertex(data[i], data[i+1], data[i+2], data[i+3]);
                builder.addQuads(op);
            }
            builder.finalizedSceneNode();
        }
    }
    else if (type == Test_7)
    {
        QBENCHMARK {
            TestBuilder builder;
            builder.newSection(QGL::Smooth);
            builder.section()->setMapThreshold(7);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGeometryData op;
                op.appendVertex(data[i], data[i+1], data[i+2], data[i+3]);
                builder.addQuads(op);
            }
            builder.finalizedSceneNode();
        }
    }
    else if (type == Test_10)
    {
        QBENCHMARK {
            TestBuilder builder;
            builder.newSection(QGL::Smooth);
            builder.section()->setMapThreshold(10);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGeometryData op;
                op.appendVertex(data[i], data[i+1], data[i+2], data[i+3]);
                builder.addQuads(op);
            }
            builder.finalizedSceneNode();
        }
    }
    else if (type == Test_20)
    {
        QBENCHMARK {
            TestBuilder builder;
            builder.newSection(QGL::Smooth);
            builder.section()->setMapThreshold(20);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGeometryData op;
                op.appendVertex(data[i], data[i+1], data[i+2], data[i+3]);
                builder.addQuads(op);
            }
            builder.finalizedSceneNode();
        }
    }
}

void tst_QGLBuilder::addQuadOrdered_data()
{
    addQuadRandom_data();
}

void tst_QGLBuilder::addQuadOrdered()
{
    QFETCH(int, size);
    QFETCH(int, type);

    int n = qSqrt(size);
    size = n * n;
    QVector3DArray data;
    data.reserve(size);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            data.append(QVector3D(1.0f * i, 1.0f * j, 0.0f),
                        QVector3D(1.0f * (i+1), 1.0f * j, 0.0f),
                        QVector3D(1.0f * (i+1), 1.0f * (j+1), 0.0f),
                        QVector3D(1.0f * i, 1.0f * (j+1), 0.0f));
    addQuadBenchMarks(data, type);
}

void tst_QGLBuilder::teapot()
{
    QBENCHMARK {
        QGLBuilder builder;
        builder << QGLTeapot();
        builder.finalizedSceneNode();
    }
}


QTEST_MAIN(tst_QGLBuilder)

#include "tst_qglbuilder_perf.moc"
