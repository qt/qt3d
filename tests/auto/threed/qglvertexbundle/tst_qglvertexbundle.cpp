/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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
#include <QtOpenGL/qgl.h>
#include "qglvertexbundle.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include "qvector4darray.h"
#include "qcolor4ub.h"

class tst_QGLVertexBundle : public QObject
{
    Q_OBJECT
public:
    tst_QGLVertexBundle() {}
    ~tst_QGLVertexBundle() {}

private slots:
    void interleaved();
    void singleAttribute();
    void large();
    void otherAttributes();
};

void tst_QGLVertexBundle::interleaved()
{
    QSKIP("QWidget: Cannot create a QWidget when no GUI is being used", SkipSingle);
    // Interleave 4 positions and texture co-ordinates, which will
    // result in the numbers 1..20 ending up in the GL server's buffer.
    QVector3DArray positions;
    positions.append(1.0f, 2.0f, 3.0f);
    positions.append(6.0f, 7.0f, 8.0f);
    positions.append(11.0f, 12.0f, 13.0f);
    positions.append(16.0f, 17.0f, 18.0f);

    QVector2DArray texCoords;
    texCoords.append(4.0f, 5.0f);
    texCoords.append(9.0f, 10.0f);
    texCoords.append(14.0f, 15.0f);
    texCoords.append(19.0f, 20.0f);

    QGLVertexBundle bundle;

    // Cannot upload an empty bundle.
    QVERIFY(!bundle.upload());

    QVERIFY(bundle.attributes().isEmpty());

    bundle.addAttribute(QGL::Position, positions);
    bundle.addAttribute(QGL::TextureCoord0, texCoords);

    QGLAttributeSet set = bundle.attributes();
    QVERIFY(set.contains(QGL::Position));
    QVERIFY(set.contains(QGL::TextureCoord0));
    QVERIFY(!set.contains(QGL::TextureCoord1));

    QVERIFY(!bundle.isUploaded());
    QCOMPARE(bundle.vertexCount(), 4);

    // Create a context to upload into.  We populate the QGLVertexBundle
    // with data before doing this to ensure that the client-side part of
    // the buffers can be created at application startup time before an
    // actual OpenGL context exists.
    QGLWidget glw;
    glw.makeCurrent();

    // Upload the bundle and bail out if we couldn't upload it
    // (i.e. vertex buffers are not supported in the GL server).
    if (!bundle.upload()) {
        QVERIFY(!bundle.isUploaded());
        return;
    }
    QVERIFY(bundle.isUploaded());

    // Uploading again should do nothing - just succeed.
    QVERIFY(bundle.upload());

    // Verify the contents of the buffer in the GL server if we can map it.
    QVERIFY(bundle.bind());
    QCOMPARE(bundle.buffer().size(), int(sizeof(float) * 20));
    float *mapped = reinterpret_cast<float *>
        (bundle.buffer().map(QGLBuffer::ReadOnly));
    if (mapped) {
        for (int index = 0; index < 20; ++index)
            QCOMPARE(mapped[index], float(index + 1));
    }
    bundle.release();

    // Make copies of the bundle, to test ref-counting.
    QGLVertexBundle bundle2(bundle);
    QGLVertexBundle bundle3;
    QCOMPARE(bundle2.buffer().bufferId(), bundle.buffer().bufferId());
    QVERIFY(!bundle3.buffer().bufferId());
    bundle3 = bundle;
    QCOMPARE(bundle3.buffer().bufferId(), bundle.buffer().bufferId());
}

// Single attribute uploads take a quicker path, which this function tests.
void tst_QGLVertexBundle::singleAttribute()
{
    QVector3DArray positions;
    positions.append(1.0f, 2.0f, 3.0f);
    positions.append(4.0f, 5.0f, 6.0f);
    positions.append(7.0f, 8.0f, 9.0f);
    positions.append(10.0f, 11.0f, 12.0f);

    QGLVertexBundle bundle;
    bundle.addAttribute(QGL::Position, positions);

    QVERIFY(!bundle.isUploaded());
    QCOMPARE(bundle.vertexCount(), 4);

    QGLWidget glw;
    glw.makeCurrent();

    if (!bundle.upload()) {
        QVERIFY(!bundle.isUploaded());
        return;
    }
    QVERIFY(bundle.isUploaded());

    QVERIFY(bundle.bind());
    QCOMPARE(bundle.buffer().size(), int(sizeof(float) * 12));
    float *mapped = reinterpret_cast<float *>
        (bundle.buffer().map(QGLBuffer::ReadOnly));
    if (mapped) {
        for (int index = 0; index < 12; ++index)
            QCOMPARE(mapped[index], float(index + 1));
    }
    bundle.release();
}

void tst_QGLVertexBundle::large()
{
    QVector3DArray positions;
    QVector2DArray texCoords;
    for (int index = 0; index < 2048; ++index) {
        positions.append(index * 5, index * 5 + 1, index * 5 + 2);
        texCoords.append(index * 5 + 3, index * 5 + 4);
    }
    QGLWidget glw;
    glw.makeCurrent();
    QGLVertexBundle bundle;
    bundle.addAttribute(QGL::Position, positions);
    bundle.addAttribute(QGL::TextureCoord0, texCoords);
    if (!bundle.upload()) {
        QVERIFY(!bundle.isUploaded());
        return;
    }
    QCOMPARE(bundle.vertexCount(), 2048);
    QVERIFY(bundle.bind());
    QCOMPARE(bundle.buffer().size(), int(sizeof(float) * 2048 * 5));
    float *mapped = reinterpret_cast<float *>
        (bundle.buffer().map(QGLBuffer::ReadOnly));
    if (mapped) {
        for (int index = 0; index < 2048 * 5; ++index)
            QCOMPARE(mapped[index], float(index));
    }
    bundle.release();
}

void tst_QGLVertexBundle::otherAttributes()
{
    QVector4DArray positions;
    positions.append(1.0f, 2.0f, 3.0f, 4.0f);
    positions.append(6.0f, 7.0f, 8.0f, 9.0f);
    positions.append(11.0f, 12.0f, 13.0f, 14.0f);
    positions.append(16.0f, 17.0f, 18.0f, 19.0f);

    QArray<float> texCoords;
    texCoords.append(5.0f);
    texCoords.append(10.0f);
    texCoords.append(15.0f);
    texCoords.append(20.0f);

    QGLWidget glw;
    glw.makeCurrent();

    QGLVertexBundle bundle;
    bundle.addAttribute(QGL::Position, positions);
    bundle.addAttribute(QGL::TextureCoord0, texCoords);
    if (!bundle.upload()) {
        QVERIFY(!bundle.isUploaded());
        return;
    }
    QCOMPARE(bundle.vertexCount(), 4);
    QVERIFY(bundle.bind());
    QCOMPARE(bundle.buffer().size(), int(sizeof(float) * 20));
    float *mapped = reinterpret_cast<float *>
        (bundle.buffer().map(QGLBuffer::ReadOnly));
    if (mapped) {
        for (int index = 0; index < 20; ++index)
            QCOMPARE(mapped[index], float(index + 1));
    }
    bundle.release();

    bundle = QGLVertexBundle();
    QVERIFY(!bundle.isUploaded());

    QArray<QColor4ub> colors;
    for (int index = 0; index < 64; ++index) {
        colors.append(QColor4ub(index * 4, index * 4 + 1,
                                index * 4 + 2, index * 4 + 3));
    }
    bundle.addAttribute(QGL::Color, colors);
    if (!bundle.upload()) {
        QVERIFY(!bundle.isUploaded());
        return;
    }
    QCOMPARE(bundle.vertexCount(), 64);
    QVERIFY(bundle.bind());
    QCOMPARE(bundle.buffer().size(), int(sizeof(uchar) * 256));
    uchar *mapuchar = reinterpret_cast<uchar *>
        (bundle.buffer().map(QGLBuffer::ReadOnly));
    if (mapuchar) {
        for (int index = 0; index < 256; ++index)
            QCOMPARE(mapuchar[index], uchar(index));
    }
    bundle.release();

    bundle = QGLVertexBundle();
    QVERIFY(!bundle.isUploaded());

    QCustomDataArray custom(texCoords);
    bundle.addAttribute(QGL::TextureCoord1, custom);
    if (!bundle.upload()) {
        QVERIFY(!bundle.isUploaded());
        return;
    }
    QCOMPARE(bundle.vertexCount(), 4);
    QVERIFY(bundle.bind());
    QCOMPARE(bundle.buffer().size(), int(sizeof(float) * 4));
    mapped = reinterpret_cast<float *>
        (bundle.buffer().map(QGLBuffer::ReadOnly));
    if (mapped) {
        for (int index = 0; index < 4; ++index)
            QCOMPARE(mapped[index], float((index + 1) * 5));
    }
    bundle.release();
}

QTEST_MAIN(tst_QGLVertexBundle)

#include "tst_qglvertexbundle.moc"
