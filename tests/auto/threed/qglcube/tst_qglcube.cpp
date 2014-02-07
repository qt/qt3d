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

#include "qglcube.h"
#include "qglbuilder.h"
#include "qtest_helpers.h"

class tst_QGLCube : public QObject
{
    Q_OBJECT
public:
    tst_QGLCube() {}
    ~tst_QGLCube() {}

private slots:
    void create();
    void size();
};

#define QGL_CUBE_SIZE (6 * 6 * (3 + 3 + 2))
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f
};

static int findVertex(const QGeometryData &ary, const QVector3D &v, const QVector3D &n)
{
    int result = -1;
    for (int i = 0; i < ary.count() && result == -1; ++i)
    {
        if (qFuzzyCompare(v, ary.vertexAt(i)) && qFuzzyCompare(n, ary.normalAt(i)))
            result = i;
    }
    if (result == -1)
    {
        qDebug() << "Could not find expected vertex" << v << "with normal" << n;
    }
    return result;
}

void tst_QGLCube::create()
{
    QGLCube cube;
    QGLBuilder list;
    list.newSection(QGL::Faceted);
    QGLSceneNode *node = list.currentNode();
    list << cube;
    QGLSceneNode *root = list.finalizedSceneNode();
    QGeometryData ary = node->geometry();
    for (int i = 0; i < QGL_CUBE_SIZE; i += 8)
    {
        const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
        int ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), tdata[0]);
    }
    delete root;
}

void tst_QGLCube::size()
{
    float size0 = 3.2f;
    float size1 = 5.2f;
    QGLCube cube(size0);
    QCOMPARE(cube.size(), size0);
    {
        QGLBuilder list;
        list.newSection(QGL::Faceted);
    QGLSceneNode *node = list.currentNode();
        list << cube;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();
        for (int i = 0, v = 0; i < QGL_CUBE_SIZE; ++v, i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size0, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.texCoordAt(ix), tdata[0]);
        }
        delete root;
    }
    cube.setSize(size1);
    {
        QGLBuilder list;
        list.newSection(QGL::Faceted);
    QGLSceneNode *node = list.currentNode();
        list << cube;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();
        for (int i = 0, v = 0; i < QGL_CUBE_SIZE; ++v, i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size1, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.texCoordAt(ix), tdata[0]);
        }
        delete root;
    }
}

QTEST_APPLESS_MAIN(tst_QGLCube)

#include "tst_qglcube.moc"
