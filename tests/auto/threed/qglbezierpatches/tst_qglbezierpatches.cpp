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
#include "qglbezierpatches.h"
#include "qglbuilder.h"
#include "qglteapot.h"

class tst_QGLBezierPatches : public QObject
{
    Q_OBJECT
public:
    tst_QGLBezierPatches() {}
    ~tst_QGLBezierPatches() {}

private slots:
    void defaultValue();
    void modify();
    void teapot();
    void build();
    void transform();
};

void tst_QGLBezierPatches::defaultValue()
{
    QGLBezierPatches patches;
    QVERIFY(patches.positions().isEmpty());
    QVERIFY(patches.textureCoords().isEmpty());
    QCOMPARE(patches.subdivisionDepth(), 4);
}

void tst_QGLBezierPatches::modify()
{
    QVector3DArray positions;
    for (int index = 0; index < 32; ++index)
        positions.append(index, index + 1, index + 2);

    QVector2DArray texCoords;
    texCoords.append(0.0f, 0.5f);
    texCoords.append(0.5f, 1.0f);
    texCoords.append(0.5f, 0.0f);
    texCoords.append(1.0f, 0.5f);

    QGLBezierPatches patches;
    patches.setPositions(positions);
    patches.setTextureCoords(texCoords);
    patches.setSubdivisionDepth(23);

    QCOMPARE(patches.positions().size(), positions.size());
    QVERIFY(patches.positions() == positions);
    QCOMPARE(patches.textureCoords().size(), texCoords.size());
    QVERIFY(patches.textureCoords() == texCoords);
    QCOMPARE(patches.subdivisionDepth(), 23);

    QGLBezierPatches patches2(patches);
    QCOMPARE(patches2.positions().size(), positions.size());
    QVERIFY(patches2.positions() == positions);
    QCOMPARE(patches2.textureCoords().size(), texCoords.size());
    QVERIFY(patches2.textureCoords() == texCoords);
    QCOMPARE(patches2.subdivisionDepth(), 23);

    patches2.setPositions(QVector3DArray());
    patches2.setTextureCoords(QVector2DArray());
    patches2.setSubdivisionDepth(18);

    QCOMPARE(patches.positions().size(), positions.size());
    QVERIFY(patches.positions() == positions);
    QCOMPARE(patches.textureCoords().size(), texCoords.size());
    QVERIFY(patches.textureCoords() == texCoords);
    QCOMPARE(patches.subdivisionDepth(), 23);

    QVERIFY(patches2.positions().isEmpty());
    QVERIFY(patches2.textureCoords().isEmpty());
    QCOMPARE(patches2.subdivisionDepth(), 18);

    QGLBezierPatches patches3;
    patches3 = patches;

    QCOMPARE(patches3.positions().size(), positions.size());
    QVERIFY(patches3.positions() == positions);
    QCOMPARE(patches3.textureCoords().size(), texCoords.size());
    QVERIFY(patches3.textureCoords() == texCoords);
    QCOMPARE(patches3.subdivisionDepth(), 23);

    patches3.setPositions(QVector3DArray());
    patches3.setTextureCoords(QVector2DArray());
    patches3.setSubdivisionDepth(18);

    QCOMPARE(patches.positions().size(), positions.size());
    QVERIFY(patches.positions() == positions);
    QCOMPARE(patches.textureCoords().size(), texCoords.size());
    QVERIFY(patches.textureCoords() == texCoords);
    QCOMPARE(patches.subdivisionDepth(), 23);

    QVERIFY(patches3.positions().isEmpty());
    QVERIFY(patches3.textureCoords().isEmpty());
    QCOMPARE(patches3.subdivisionDepth(), 18);

    patches = QGLBezierPatches();
    QVERIFY(patches.positions().isEmpty());
    QVERIFY(patches.textureCoords().isEmpty());
    QCOMPARE(patches.subdivisionDepth(), 4);
}

void tst_QGLBezierPatches::teapot()
{
    // Verify that patch data is added by QGLTeapot's constructor,
    // but not what that patch data is.
    QGLTeapot teapot;
    QCOMPARE(teapot.positions().size(), 32 * 16);
    QCOMPARE(teapot.textureCoords().size(), 0);
    QCOMPARE(teapot.subdivisionDepth(), 4);
}

void tst_QGLBezierPatches::build()
{
    // Call the build routines with different options.  It isn't really
    // easy to determine if the correct triangles were generated.  Leave
    // that up to visual acceptance tests.

    QGLBuilder builder;

    QGLTeapot teapot1;
    builder << teapot1;

    QGLTeapot teapot2;
    QVector2DArray texCoords;
    for (int index = 0; index < 32; ++index) {
        texCoords.append(0.0f, 0.5f);
        texCoords.append(0.5f, 1.0f);
    }
    teapot2.setTextureCoords(texCoords);
    builder << teapot2;

    delete builder.finalizedSceneNode();
}

void tst_QGLBezierPatches::transform()
{
    QVector3DArray positions;
    for (int index = 0; index < 32; ++index)
        positions.append(index, index + 1, index + 2);

    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);

    QVector3DArray positions2 = positions.transformed(m);

    QVector2DArray texCoords;
    texCoords.append(0.0f, 0.5f);
    texCoords.append(0.5f, 1.0f);
    texCoords.append(0.5f, 0.0f);
    texCoords.append(1.0f, 0.5f);

    QGLBezierPatches patches;
    patches.setPositions(positions);
    patches.setTextureCoords(texCoords);
    patches.setSubdivisionDepth(18);

    QGLBezierPatches patches2(patches);
    patches2.transform(m);
    QVERIFY(patches2.positions() == positions2);
    QVERIFY(patches2.textureCoords() == texCoords);
    QCOMPARE(patches2.subdivisionDepth(), 18);

    QGLBezierPatches patches3 = patches.transformed(m);
    QVERIFY(patches3.positions() == positions2);
    QVERIFY(patches3.textureCoords() == texCoords);
    QCOMPARE(patches3.subdivisionDepth(), 18);
}

QTEST_APPLESS_MAIN(tst_QGLBezierPatches)

#include "tst_qglbezierpatches.moc"
