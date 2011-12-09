/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "qglsphere.h"
#include "qglbuilder.h"

class tst_QGLSphere : public QObject
{
    Q_OBJECT
public:
    tst_QGLSphere() {}
    ~tst_QGLSphere() {}

private slots:
    void create();
    void modify();
    void build();

private:
    bool validateSphere(QGLSceneNode *node, qreal size);
};

void tst_QGLSphere::create()
{
    QGLSphere sphere1;
    QCOMPARE(sphere1.diameter(), qreal(1.0f));
    QCOMPARE(sphere1.subdivisionDepth(), 5);

    QGLSphere sphere2(27.5f);
    QCOMPARE(sphere2.diameter(), qreal(27.5f));
    QCOMPARE(sphere2.subdivisionDepth(), 5);

    QGLSphere sphere3(20.5f, 4);
    QCOMPARE(sphere3.diameter(), qreal(20.5f));
    QCOMPARE(sphere3.subdivisionDepth(), 4);
}

void tst_QGLSphere::modify()
{
    QGLSphere sphere1;
    sphere1.setDiameter(5.0f);
    sphere1.setSubdivisionDepth(4);
    QCOMPARE(sphere1.diameter(), qreal(5.0f));
    QCOMPARE(sphere1.subdivisionDepth(), 4);
}

// Validates the geometry in a sphere node to check that all vertices
// are on the surface of a sphere of diameter "size" and all normals
// are of length 1 and point directly out from the center.
bool tst_QGLSphere::validateSphere(QGLSceneNode *node, qreal size)
{
    QGeometryData data = node->geometry();
    int start = node->start();
    int count = node->count();
    if (count <= 0)
        return false;
    QGL::IndexArray indices = data.indices();
    for (int index = 0; index < count; ++index) {
        QVector3D v = data.vertexAt(indices.at(index + start));
        QVector3D n = data.normalAt(indices.at(index + start));
        if (qAbs(n.length() - 1.0f) >= 0.0001)
            return false;
        QVector3D v2 = n * size / 2.0f;
        if (qAbs(v.x() - v2.x()) >= 0.0001 ||
                qAbs(v.y() - v2.y()) >= 0.0001 ||
                qAbs(v.z() - v2.z()) >= 0.0001)
            return false;
    }
    return true;
}

void tst_QGLSphere::build()
{
    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    QGLSceneNode *node1 = builder.newNode();
    builder << QGLSphere(5.0f, 5);
    QGLSceneNode *root = builder.finalizedSceneNode();
    QVERIFY(validateSphere(node1, 5.0f));
    delete root;
}

QTEST_APPLESS_MAIN(tst_QGLSphere)

#include "tst_qglsphere.moc"
