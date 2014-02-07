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
#include <QtCore/qpointer.h>

#include "qgeometrydata.h"
#include "qvector_utils_p.h"
#include "qtest_helpers.h"
#include "qglpainter.h"
#include "qglmockview.h"

#include <QtCore/qdebug.h>

class tst_QGeometryData : public QObject
{
    Q_OBJECT
public:
    tst_QGeometryData() {}
    ~tst_QGeometryData() {}

private slots:
    void createDefault();
    void appendVertex();
    void appendNormal();
    void appendVertexNormal();
    void copy();
    void interleaveWith();
    void boundingBox();
    void center();
    void normalizeNormals();
    void reversed();
    void translated();
    void generateTextureCoordinates();
    void clear();
    void draw();
};

void tst_QGeometryData::createDefault()
{
    QGeometryData data;
    QCOMPARE(data.count(), 0);
    QCOMPARE(data.attributes().count(), 0);
    QCOMPARE(data.fields(), quint32(0));
    QCOMPARE(data.vertices().count(), 0);
    data.normalizeNormals();
    QCOMPARE(data.boundingBox(), QBox3D());

    // copy constructor on initialization - null default
    QGeometryData other = data;
    QCOMPARE(other.count(), 0);
    QCOMPARE(other.attributes().count(), 0);
    QCOMPARE(other.fields(), quint32(0));

    // copy constructor on default
    QGeometryData copy(data);
    QCOMPARE(copy.count(), 0);
    QCOMPARE(copy.attributes().count(), 0);
    QCOMPARE(copy.fields(), quint32(0));
}

void tst_QGeometryData::appendVertex()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    {
        QGeometryData data;
        data.appendVertex(a);
        QCOMPARE(data.count(), 1);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 1);
        QCOMPARE(data.vertices().at(0), a);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b);
        QCOMPARE(data.count(), 2);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 2);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c);
        QCOMPARE(data.count(), 3);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 3);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        QCOMPARE(data.count(), 4);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 4);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
        QCOMPARE(data.vertices().at(3), d);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        data.appendVertex(a, b, c, d);
        data.appendVertex(a);
        QCOMPARE(data.count(), 9);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 9);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(5), b);
        QCOMPARE(data.vertices().at(8), a);
    }
}

void tst_QGeometryData::appendNormal()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);
    {
        QGeometryData data;
        data.appendNormal(a);
        QCOMPARE(data.count(), 1);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 1);
        QCOMPARE(data.normals().at(0), a);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b);
        QCOMPARE(data.count(), 2);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 2);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b, c);
        QCOMPARE(data.count(), 3);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 3);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
        QCOMPARE(data.normals().at(2), c);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b, c, d);
        QCOMPARE(data.count(), 4);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 4);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
        QCOMPARE(data.normals().at(2), c);
        QCOMPARE(data.normals().at(3), d);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b, c, d);
        data.appendNormal(a, b, c, d);
        data.appendNormal(a);
        QCOMPARE(data.count(), 9);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 9);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
        QCOMPARE(data.normals().at(5), b);
        QCOMPARE(data.normals().at(8), a);
    }
}

void tst_QGeometryData::appendVertexNormal()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    QVector3D an(5.1f, 5.2f, 5.3f);
    QVector3D bn(6.1f, 6.2f, 6.3f);
    QVector3D cn(7.1f, 7.2f, 7.3f);
    QVector3D dn(8.1f, 8.2f, 8.3f);
    {
        QGeometryData data;
        data.appendVertex(a);
        data.appendNormal(an);
        QCOMPARE(data.count(), 1);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 1);
        QCOMPARE(data.vertices().at(0), a);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b);
        data.appendNormal(an, bn);
        QCOMPARE(data.count(), 2);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 2);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertex(1), b);
        QCOMPARE(data.normals().count(), 2);
        QCOMPARE(data.normal(0), an);
        QCOMPARE(data.normals().at(1), bn);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c);
        data.appendNormal(an, bn, cn);
        QCOMPARE(data.count(), 3);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 3);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
        QCOMPARE(data.normals().count(), 3);
        QCOMPARE(data.normal(0), an);
        QCOMPARE(data.normals().at(1), bn);
        QCOMPARE(data.normal(2), cn);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        data.appendNormal(an, bn, cn, dn);
        QCOMPARE(data.count(), 4);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 4);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
        QCOMPARE(data.vertices().at(3), d);
        QCOMPARE(data.normals().count(), 4);
        QCOMPARE(data.normals().at(0), an);
        QCOMPARE(data.normals().at(1), bn);
        QCOMPARE(data.normals().at(2), cn);
        QCOMPARE(data.normals().at(3), dn);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        data.appendNormal(an, bn, cn, dn);
        data.appendVertex(a, b, c, d);
        data.appendNormal(an, bn, cn, dn);
        data.appendVertex(a);
        data.appendNormal(an);
        QCOMPARE(data.count(), 9);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 9);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(5), b);
        QCOMPARE(data.vertices().at(8), a);
        QCOMPARE(data.normals().count(), 9);
        QCOMPARE(data.normals().at(0), an);
        QCOMPARE(data.normals().at(1), bn);
        QCOMPARE(data.normals().at(5), bn);
        QCOMPARE(data.normals().at(8), an);
    }
}


QVector3D avec(99.1f, 99.2f, 99.3f);

static QGeometryData aFunc(const QGeometryData& g)   // not a copy but a ref
{
    QGeometryData d = g;  // copy constructor
    d.appendVertex(avec);
    return d;   // assingment operator
}

void tst_QGeometryData::copy()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    {
        QGeometryData data;
        QCOMPARE(data.count(), 0);
        QCOMPARE(data.fields(), (quint32)0);
        QGeometryData other;
        other.appendVertex(QVector3D());
        QCOMPARE(other.count(), 1);
        QCOMPARE(other.fields(), QGL::fieldMask(QGL::Position));
        other = aFunc(data);  // assignment op - throw away previous d
        QCOMPARE(other.count(), 1);
        QCOMPARE(other.fields(), QGL::fieldMask(QGL::Position));
        QVector3D res = other.vertices().at(0);
        QCOMPARE(res, avec);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        QGeometryData other;
        QCOMPARE(other.count(), 0);
        QCOMPARE(other.fields(), (quint32)0);
        other = aFunc(data);  // assignment operator
        other.appendVertex(a);
        QCOMPARE(other.count(), 6);
        QCOMPARE(other.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(other.vertices().count(), 6);
        QCOMPARE(other.vertices().at(0), a);
        QCOMPARE(other.vertices().at(1), b);
        QCOMPARE(other.vertices().at(4), avec);
        QCOMPARE(other.vertices().at(5), a);
    }
}

void tst_QGeometryData::interleaveWith()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    QVector3D vx(0.7071f, 0.7071f, 0.0f);
    QVector2D at(0.11f, 0.12f);
    QVector2D bt(0.21f, 0.22f);
    QVector2D ct(0.31f, 0.32f);
    QVector2D dt(0.41f, 0.42f);
    QVector2D tx(1.0f, 1.0f);

    QGeometryData data;
    data.appendVertex(a, b, c, d);
    data.appendTexCoord(at, bt, ct, dt);
    QGeometryData dat2;

    // count is the smaller of the two - nothing in this null case
    // also make sure the argument does not somehow change - its a const
    // so it shouldn't...
    dat2.interleaveWith(data);
    QCOMPARE(data.count(), 4);
    QCOMPARE(data.vertex(0), a);
    QCOMPARE(dat2.count(), 0);
    QCOMPARE(dat2.count(QGL::Position), 0);
    QCOMPARE(dat2.fields(), quint32(0));

    // dat2 is smaller and has less fields
    dat2.appendVertex(a + vx, b + vx);
    dat2.interleaveWith(data);
    QCOMPARE(data.count(), 4);
    QCOMPARE(data.vertex(0), a);
    QCOMPARE(dat2.count(), 4);
    QCOMPARE(dat2.count(QGL::Position), 4);
    QCOMPARE(dat2.count(QGL::TextureCoord0), 0);
    QCOMPARE(dat2.fields(), QGL::fieldMask(QGL::Position));
    QCOMPARE(dat2.vertex(0), a + vx);
    QCOMPARE(dat2.vertex(1), a);
    QCOMPARE(dat2.vertex(2), b + vx);
    QCOMPARE(dat2.vertex(3), b);

    // full zip with both sides have 4 verts & textures
    dat2.clear();
    for (int i = 0; i < data.count(); ++i)
    {
        dat2.appendVertex(data.vertex(i) + vx);
        dat2.appendTexCoord(data.texCoord(i) + tx);
    }
    dat2.interleaveWith(data);
    QCOMPARE(dat2.count(), 8);
    QCOMPARE(dat2.count(QGL::Position), 8);
    QCOMPARE(dat2.count(QGL::TextureCoord0), 8);
    QCOMPARE(dat2.fields(), QGL::fieldMask(QGL::Position) |
             QGL::fieldMask(QGL::TextureCoord0));
    QCOMPARE(dat2.vertex(0), a + vx);
    QCOMPARE(dat2.vertex(1), a);
    QCOMPARE(dat2.vertex(4), c + vx);
    QCOMPARE(dat2.vertex(7), d);
    QCOMPARE(dat2.texCoord(0), at + tx);
    QCOMPARE(dat2.texCoord(3), bt);
    QCOMPARE(dat2.texCoord(7), dt);
}

void tst_QGeometryData::boundingBox()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);

    QGeometryData data;
    data.appendVertex(a, b, c, d);

    QBox3D bb = data.boundingBox();
    QCOMPARE(bb.maximum(), d);
    QCOMPARE(bb.minimum(), a);
}

void tst_QGeometryData::center()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);

    QGeometryData data;
    data.appendVertex(a, b, c, d);

    QVector3D center = data.center();
    QCOMPARE(center, QVector3D(2.6, 2.7, 2.8));
}

void tst_QGeometryData::normalizeNormals()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    QVector3D an(5.1f, 5.2f, 5.3f);
    QVector3D bn(6.1f, 6.2f, 6.3f);
    QVector3D cn(7.1f, 7.2f, 7.3f);
    QVector3D dn(8.1f, 8.2f, 8.3f);

    QGeometryData data;
    data.appendVertex(a, b, c, d);
    data.appendNormal(an, bn, cn, dn);

    data.normalizeNormals();
    QVERIFY(qFskCompare(data.normalAt(0), QVector3D(0.566178f, 0.577279f, 0.588381f)));
    QVERIFY(qFskCompare(data.normalAt(1), QVector3D(0.567989f, 0.577300f, 0.586612f)));
    QVERIFY(qFskCompare(data.normalAt(2), QVector3D(0.569295f, 0.577313f, 0.585331f)));
    QVERIFY(qFskCompare(data.normalAt(3), QVector3D(0.570281f, 0.577322f, 0.584362f)));
    QVERIFY(qFskCompare(data.normalAt(0).lengthSquared(), 1.0f));
    QVERIFY(qFskCompare(data.normalAt(1).lengthSquared(), 1.0f));
    QVERIFY(qFskCompare(data.normalAt(2).lengthSquared(), 1.0f));
    QVERIFY(qFskCompare(data.normalAt(3).lengthSquared(), 1.0f));
}

void tst_QGeometryData::reversed()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    QVector3D an(5.1f, 5.2f, 5.3f);
    QVector3D bn(6.1f, 6.2f, 6.3f);
    QVector3D cn(7.1f, 7.2f, 7.3f);
    QVector3D dn(8.1f, 8.2f, 8.3f);

    QGeometryData data;
    data.appendVertex(a, b, c, d);
    data.appendNormal(an, bn, cn, dn);

    QGeometryData reversed = data.reversed();

    QCOMPARE(data.vertexAt(0), reversed.vertexAt(3));
    QCOMPARE(data.vertexAt(1), reversed.vertexAt(2));
    QCOMPARE(data.vertexAt(2), reversed.vertexAt(1));
    QCOMPARE(data.vertexAt(3), reversed.vertexAt(0));
    QCOMPARE(data.normalAt(0), reversed.normalAt(3));
    QCOMPARE(data.normalAt(1), reversed.normalAt(2));
    QCOMPARE(data.normalAt(2), reversed.normalAt(1));
    QCOMPARE(data.normalAt(3), reversed.normalAt(0));
}

void tst_QGeometryData::translated()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    QVector3D an(5.1f, 5.2f, 5.3f);
    QVector3D bn(6.1f, 6.2f, 6.3f);
    QVector3D cn(7.1f, 7.2f, 7.3f);
    QVector3D dn(8.1f, 8.2f, 8.3f);

    QGeometryData data;
    data.appendVertex(a, b, c, d);
    data.appendNormal(an, bn, cn, dn);

    QVector3D t(0.5, -0.5, -0.5);
    QGeometryData translated = data.translated(t);

    QVector3D at = a + t;
    QVector3D bt = b + t;
    QVector3D ct = c + t;
    QVector3D dt = d + t;

    QCOMPARE(at, translated.vertexAt(0));
    QCOMPARE(bt, translated.vertexAt(1));
    QCOMPARE(ct, translated.vertexAt(2));
    QCOMPARE(dt, translated.vertexAt(3));
    QCOMPARE(an, translated.normalAt(0));
    QCOMPARE(bn, translated.normalAt(1));
    QCOMPARE(cn, translated.normalAt(2));
    QCOMPARE(dn, translated.normalAt(3));
}

void tst_QGeometryData::generateTextureCoordinates()
{
    QGeometryData top;

    top.appendVertex(QVector3D(0.0, 0.0, 0.0));
    top.appendVertex(QVector3D(6.0, 3.6, 0.0));    // (v1 - v0).length() = 7.0
    top.appendVertex(QVector3D(10.0, 0.6, 0.0));   // (v2 - v1).length() = 5.0
    top.appendVertex(QVector3D(13.0, 3.24, 0.0));  // (v3 - v2).length() = 4.0

    // generate x (Qt::Horizontal) texture coordinates
    top.generateTextureCoordinates();              // spread over 7 + 5 + 4 = 16

    QVERIFY(qFskCompare(top.texCoordAt(0), QVector2D()));
    QVERIFY(qFskCompare(top.texCoordAt(1), QVector2D(0.43750349, 0.0)));
    QVERIFY(qFskCompare(top.texCoordAt(2), QVector2D(0.75013363, 0.0)));
    QVERIFY(qFskCompare(top.texCoordAt(3), QVector2D(1.0, 0.0)));

    QGeometryData side;

    side.appendVertex(QVector3D(0.0, 0.0, 0.0));
    side.appendVertex(QVector3D(3.6, 6.0, 0.0));    // (v1 - v0).length() = 7.0
    side.appendVertex(QVector3D(0.6, 10.0, 0.0));   // (v2 - v1).length() = 5.0
    side.appendVertex(QVector3D(3.24, 13.0, 0.0));  // (v3 - v2).length() = 4.0

    // generate x (Qt::Vertical) texture coordinates
    side.generateTextureCoordinates(Qt::Vertical);  // spread over 7 + 5 + 4 = 16

    QVERIFY(qFskCompare(side.texCoordAt(0), QVector2D()));
    QVERIFY(qFskCompare(side.texCoordAt(1), QVector2D(0.0, 0.43750349)));
    QVERIFY(qFskCompare(side.texCoordAt(2), QVector2D(0.0, 0.75013363)));
    QVERIFY(qFskCompare(side.texCoordAt(3), QVector2D(0.0, 1.0)));
}

void tst_QGeometryData::clear()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    QVector3D an(5.1f, 5.2f, 5.3f);
    QVector3D bn(6.1f, 6.2f, 6.3f);
    QVector3D cn(7.1f, 7.2f, 7.3f);
    QVector3D dn(8.1f, 8.2f, 8.3f);

    QGeometryData data;
    data.appendVertex(a, b, c, d);
    data.appendNormal(an, bn, cn, dn);

    quint32 f = data.fields();
    quint32 expectedFields = QGL::fieldMask(QGL::Normal) | QGL::fieldMask(QGL::Position);
    QCOMPARE(f, expectedFields);
    QCOMPARE(data.count(), 4);
    QCOMPARE(data.count(QGL::Normal), 4);
    QCOMPARE(data.count(QGL::Position), 4);

    data.clear();
    QCOMPARE(f, expectedFields);
    QCOMPARE(data.count(), 0);
    QCOMPARE(data.count(QGL::Normal), 0);
    QCOMPARE(data.count(QGL::Position), 0);

    data.appendVertex(a, b, c, d);
    data.appendNormal(an, bn, cn, dn);

    data.clear(QGL::Normal);

    f = data.fields();
    expectedFields = QGL::fieldMask(QGL::Position);
    QCOMPARE(f, expectedFields);
    QCOMPARE(data.count(), 4);
    QCOMPARE(data.count(QGL::Normal), 0);
    QCOMPARE(data.count(QGL::Position), 4);
}

void tst_QGeometryData::draw()
{
    QVector3D a(1.1f, 1.2f, 1.3f);
    QVector3D b(2.1f, 2.2f, 2.3f);
    QVector3D c(3.1f, 3.2f, 3.3f);
    QVector3D d(4.1f, 4.2f, 4.3f);
    QVector3D an(5.1f, 5.2f, 5.3f);
    QVector3D bn(6.1f, 6.2f, 6.3f);
    QVector3D cn(7.1f, 7.2f, 7.3f);
    QVector3D dn(8.1f, 8.2f, 8.3f);

    QGeometryData data;
    data.appendVertex(a, b, c, d);
    data.appendNormal(an, bn, cn, dn);

    QGLMockView w;
    if (!w.isValid())
        QSKIP("Cannot create valid GL Context");

    QGLPainter p(&w);
    data.draw(&p, 0, 4);
}


QTEST_MAIN(tst_QGeometryData)

#include "tst_qgeometrydata.moc"
