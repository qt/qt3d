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

#include "qglcylinder.h"
#include "qglbuilder.h"

class tst_QGLCylinder : public QObject
{
    Q_OBJECT
public:
    tst_QGLCylinder() {}
    ~tst_QGLCylinder() {}

private slots:
    void create();
    void modify();
    void build();

private:
    bool validateCylinder(QGLSceneNode *node, float topDiameter, float baseDiameter, float height, bool top, bool base);
};

void tst_QGLCylinder::create()
{
    //Test all possible constructor parameters to ensure they are actually
    //setting the correct values.

    //Defaults
    QGLCylinder cylinder1;
    QCOMPARE(cylinder1.diameterTop(), 1.0f);
    QCOMPARE(cylinder1.diameterBottom(), 1.0f);
    QCOMPARE(cylinder1.height(), 1.0f);
    QCOMPARE(cylinder1.slices(), 6);
    QCOMPARE(cylinder1.layers(), 3);
    QCOMPARE(cylinder1.topEnabled(), true);
    QCOMPARE(cylinder1.baseEnabled(), true);

    //Top diameter
    QGLCylinder cylinder2(0.5f);
    QCOMPARE(cylinder2.diameterTop(), 0.5f);
    QCOMPARE(cylinder2.diameterBottom(), 1.0f);
    QCOMPARE(cylinder2.height(), 1.0f);
    QCOMPARE(cylinder2.slices(), 6);
    QCOMPARE(cylinder2.layers(), 3);
    QCOMPARE(cylinder2.topEnabled(), true);
    QCOMPARE(cylinder2.baseEnabled(), true);

    //Top and Bottom diameter
    QGLCylinder cylinder3(0.5f, 1.5f);
    QCOMPARE(cylinder3.diameterTop(), 0.5f);
    QCOMPARE(cylinder3.diameterBottom(), 1.5f);
    QCOMPARE(cylinder3.height(), 1.0f);
    QCOMPARE(cylinder3.slices(), 6);
    QCOMPARE(cylinder3.layers(), 3);
    QCOMPARE(cylinder3.topEnabled(), true);
    QCOMPARE(cylinder3.baseEnabled(), true);

    //Top & Bottom diameter, Height
    QGLCylinder cylinder4(0.5f, 1.5f, 3.0f);
    QCOMPARE(cylinder4.diameterTop(), 0.5f);
    QCOMPARE(cylinder4.diameterBottom(), 1.5f);
    QCOMPARE(cylinder4.height(), 3.0f);
    QCOMPARE(cylinder4.slices(), 6);
    QCOMPARE(cylinder4.layers(), 3);
    QCOMPARE(cylinder4.topEnabled(), true);
    QCOMPARE(cylinder4.baseEnabled(), true);

    //Top & Bottom diameter, Height, Slices
    QGLCylinder cylinder5(0.5f, 1.5f, 3.0f, 10);
    QCOMPARE(cylinder5.diameterTop(), 0.5f);
    QCOMPARE(cylinder5.diameterBottom(), 1.5f);
    QCOMPARE(cylinder5.height(), 3.0f);
    QCOMPARE(cylinder5.slices(), 10);
    QCOMPARE(cylinder5.layers(), 3);
    QCOMPARE(cylinder5.topEnabled(), true);
    QCOMPARE(cylinder5.baseEnabled(), true);

    //Top & Bottom diameter, Height, Slices, Layers
    QGLCylinder cylinder6(0.5f, 1.5f, 3.0f, 10, 5);
    QCOMPARE(cylinder6.diameterTop(), 0.5f);
    QCOMPARE(cylinder6.diameterBottom(), 1.5f);
    QCOMPARE(cylinder6.height(), 3.0f);
    QCOMPARE(cylinder6.slices(), 10);
    QCOMPARE(cylinder6.layers(), 5);
    QCOMPARE(cylinder6.topEnabled(), true);
    QCOMPARE(cylinder6.baseEnabled(), true);

    //Top & Bottom diameter, Height, Slices, Layers, Top disabled
    QGLCylinder cylinder7(0.5f, 1.5f, 3.0f, 10, 5, false);
    QCOMPARE(cylinder7.diameterTop(), 0.5f);
    QCOMPARE(cylinder7.diameterBottom(), 1.5f);
    QCOMPARE(cylinder7.height(), 3.0f);
    QCOMPARE(cylinder7.slices(), 10);
    QCOMPARE(cylinder7.layers(), 5);
    QCOMPARE(cylinder7.topEnabled(), false);
    QCOMPARE(cylinder7.baseEnabled(), true);

    //Top & Bottom diameter, Height, Slices, Layers, Top disabled, Bottom disabled.
    QGLCylinder cylinder8(0.5f, 1.5f, 3.0f, 10, 5, false, false);
    QCOMPARE(cylinder8.diameterTop(), 0.5f);
    QCOMPARE(cylinder8.diameterBottom(), 1.5f);
    QCOMPARE(cylinder8.height(), 3.0f);
    QCOMPARE(cylinder8.slices(), 10);
    QCOMPARE(cylinder8.layers(), 5);
    QCOMPARE(cylinder8.topEnabled(), false);
    QCOMPARE(cylinder8.baseEnabled(), false);
}

void tst_QGLCylinder::modify()
{
    //Modify all sphere parameters to ensure they're working.
    QGLCylinder cylinder1;

    cylinder1.setDiameterTop(0.5f);
    QCOMPARE(cylinder1.diameterTop(), 0.5f);

    cylinder1.setDiameterBottom(1.5f);
    QCOMPARE(cylinder1.diameterBottom(), 1.5f);

    cylinder1.setHeight(3.0f);
    QCOMPARE(cylinder1.height(), 3.0f);

    cylinder1.setSlices(10);
    QCOMPARE(cylinder1.slices(), 10);

    cylinder1.setLayers(5);
    QCOMPARE(cylinder1.layers(), 5);

    cylinder1.setTopEnabled(false);
    QCOMPARE(cylinder1.topEnabled(), false);

    cylinder1.setBaseEnabled(false);
    QCOMPARE(cylinder1.baseEnabled(), false);
}

// Validates the geometry in a cylinder node to check that all vertices
// on the top and bottom are at the correct height, and that all normals
// are of length 1.  It also ensures that all points are within the defined
// top and bottom diameters for the cylinder.
bool tst_QGLCylinder::validateCylinder(QGLSceneNode *node, float topDiameter, float baseDiameter, float height, bool topAttached, bool baseAttached)
{
    QGLSceneNode *top = node->findChild<QGLSceneNode *>("Cylinder Top");
    QGLSceneNode *sides = node->findChild<QGLSceneNode *>("Cylinder Sides");
    QGLSceneNode *base = node->findChild<QGLSceneNode *>("Cylinder Base");

    //Top of the cylinder - test the normals and vertical position
    if (topAttached)
    {
        if (!top) {
            return false;
        }

        QGeometryData dataTop = top->geometry();
        int start = top->start();
        int count = top->count();

        if (count <= 0) {
            return false;
        }
        QGL::IndexArray indices = dataTop.indices();

        for (int index = 0; index < count; ++index) {
            QVector3D v = dataTop.vertexAt(indices.at(index + start));
            QVector3D n = dataTop.normalAt(indices.at(index + start));
            if (v.z()!=0.5*height) {
                return false;
            }
            QVector2D p(v.x(), v.y());
            if (qAbs(p.length())>(topDiameter/2.0)) {
                return false;
            }
            if (qAbs(n.length() - 1.0f) >= 0.0001) {
                return false;
            }
        }
    }

    //Base of the cylinder - test the normals and vertical position
    if (baseAttached)
    {
        if (!base) {
            return false;
        }
        QGeometryData dataBase = base->geometry();
        int start = base->start();
        int count = base->count();
        if (count <= 0) {
            return false;
        }
        QGL::IndexArray indices = dataBase.indices();
        for (int index = 0; index < count; ++index) {
            QVector3D v = dataBase.vertexAt(indices.at(index + start));
            QVector3D n = dataBase.normalAt(indices.at(index + start));
            if (v.z()!=-0.5*height) {
                return false;
            }
            QVector2D p(v.x(), v.y());
            if (qAbs(p.length())>(baseDiameter/2.0)) {
                return false;
            }
            if (qAbs(n.length() - 1.0f) >= 0.0001) {
                return false;
            }
        }
    }

    //Sides of the cylinder - just a basic test of the normals here.
    if (!sides) {
        return false;
    }
    QGeometryData dataSides = sides->geometry();
    int start = sides->start();
    int count = sides->count();

    if (count <= 0) {
        return false;
    }
    QGL::IndexArray indices = dataSides.indices();
    for (int index = 0; index < count; ++index) {
        QVector3D n = dataSides.normalAt(indices.at(index + start));
        if (qAbs(n.length() - 1.0f) >= 0.0001) {
            return false;
        }
    }

    //If we've made it this far, we must have passed the test.
    return true;

}

void tst_QGLCylinder::build()
{
    QGLBuilder builder1;
    builder1 << QGLCylinder(0.5f, 1.5f, 3.0f);
    QGLSceneNode *root1 = builder1.finalizedSceneNode();
    QVERIFY(validateCylinder(root1, 0.5f, 1.5f, 3.0f, true, true));
    delete root1;

    QGLBuilder builder2;
    builder2 << QGLCylinder(0.5f, 1.5f, 3.0f, 6,3,false, false);
    QGLSceneNode *root2 = builder2.finalizedSceneNode();
    QVERIFY(validateCylinder(root2, 0.5f, 1.5f, 3.0f, false, false));
    delete root2;
}

QTEST_APPLESS_MAIN(tst_QGLCylinder)

#include "tst_qglcylinder.moc"
