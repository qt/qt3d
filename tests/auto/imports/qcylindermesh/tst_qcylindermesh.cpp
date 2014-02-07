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
#include <Qt3DQuick/cylindermesh.h>

class tst_QCylinderMesh : public QObject
{
    Q_OBJECT
public:
    tst_QCylinderMesh() {}
    ~tst_QCylinderMesh() {}

private slots:
    void testSignalsAndProperties();
    void testGeometry();
};

void tst_QCylinderMesh::testSignalsAndProperties()
{
    CylinderMesh cylinder;
    {
        QCOMPARE(cylinder.radius(),0.5);
        QCOMPARE(cylinder.length(),1.0);
        QCOMPARE(cylinder.levelOfDetail(),5);
    }
    {
        QSignalSpy spyRadius(&cylinder,SIGNAL(radiusChanged()));
        cylinder.setRadius(2.0);
        QCOMPARE(spyRadius.size(), 1);
        QCOMPARE(cylinder.radius(),2.0);
    }
    {
        QSignalSpy spyLength(&cylinder,SIGNAL(lengthChanged()));
        cylinder.setLength(3.0);
        QCOMPARE(spyLength.size(), 1);
        QCOMPARE(cylinder.length(),3.0);
    }
    {
        QSignalSpy spyLOD(&cylinder,SIGNAL(levelOfDetailChanged()));
        cylinder.setLevelOfDetail(6);
        QCOMPARE(spyLOD.size(), 1);
        QCOMPARE(cylinder.levelOfDetail(),6);

        cylinder.setLevelOfDetail(9999);
        QCOMPARE(spyLOD.size(), 2);
        QCOMPARE(cylinder.levelOfDetail(),10);

        cylinder.setLevelOfDetail(-9999);
        QCOMPARE(spyLOD.size(), 3);
        QCOMPARE(cylinder.levelOfDetail(),1);
    }
}

void tst_QCylinderMesh::testGeometry()
{
    CylinderMesh cylinder;
    cylinder.setLevelOfDetail(1);

    QGLSceneNode* pRootNode = cylinder.getSceneObject();
    QVERIFY(pRootNode!=0);
    {
        QCOMPARE(pRootNode->objectName(),QString("CylinderMesh"));
        QGeometryData rootGeom = pRootNode->geometry();
        QVector3DArray rootVerts = rootGeom.vertices();
        QCOMPARE(rootVerts.size(), 0);
        QVector3DArray rootNorms = rootGeom.normals();
        QCOMPARE(rootNorms.size(), 0);
    }

    const QObjectList& rChildren = cylinder.children();
    QCOMPARE(rChildren.size(),1);
    QObject* pObj = rChildren.at(0);
    QVERIFY(pObj!=0);
    QGLSceneNode* pSceneNode = qobject_cast<QGLSceneNode*>(pObj);
    QVERIFY(pSceneNode!=0);
    {
        QGeometryData geom = pSceneNode->geometry();
        QVector3DArray verts = geom.vertices();
        QCOMPARE(verts.size(),0);
        QVector3DArray norms = geom.normals();
        QCOMPARE(norms.size(),0);
    }
    QList<QGLSceneNode *> sceneNodeChildren = pSceneNode->children();
    QCOMPARE(sceneNodeChildren.size(),3);
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(0);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("Cylinder Top"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),36);
        QGL::IndexArray ib = geom.indices();
        for (int i=pSceneNodeLevel2->start(); i<pSceneNodeLevel2->start()+pSceneNodeLevel2->count(); i+=3) {
            unsigned int ii0 = ib.at(i);
            QVERIFY(ii0<(unsigned int)verts.size());
            QVector3D vv0 = verts.at(ii0);
            float vv0sq = (vv0-QVector3D(0,0,0.5)).lengthSquared();
            QVERIFY(fabs(vv0sq)<0.0001 || fabs(vv0sq-1.0)<0.0001); // vertex is either in center or on circle
            QVector3D nn0 = norms.at(ii0);
            QVERIFY((fabs(QVector3D::dotProduct(nn0,QVector3D(0,0,1)))-1.0) < 0.0001); // vertex normal == {0,0,1}

            unsigned int ii1 = ib.at(i+1);
            QVERIFY(ii1<(unsigned int)verts.size());
            QVector3D vv1 = verts.at(ii1);
            float vv1sq = (vv1-QVector3D(0,0,0.5)).lengthSquared();
            QVERIFY(fabs(vv1sq)<0.0001 || fabs(vv1sq-1.0)<0.0001); // vertex is either in center or on circle
            QVector3D nn1 = norms.at(ii1);
            QVERIFY((fabs(QVector3D::dotProduct(nn1,QVector3D(0,0,1)))-1.0) < 0.0001); // vertex normal == {0,0,1}

            unsigned int ii2 = ib.at(i+2);
            QVERIFY(ii2<(unsigned int)verts.size());
            QVector3D vv2 = verts.at(ii2);
            float vv2sq = (vv2-QVector3D(0,0,0.5)).lengthSquared();
            QVERIFY(fabs(vv2sq)<0.0001 || fabs(vv2sq-1.0)<0.0001); // vertex is either in center or on circle
            QVector3D nn2 = norms.at(ii2);
            QVERIFY((fabs(QVector3D::dotProduct(nn2,QVector3D(0,0,1)))-1.0) < 0.0001); // vertex normal == {0,0,1}

            QVector3D triangleNormal = QVector3D::crossProduct(vv1-vv0,vv2-vv0);
            triangleNormal.normalize();
            QVERIFY((fabs(QVector3D::dotProduct(triangleNormal,QVector3D(0,0,1)))-1.0) < 0.0001); // triangle normal == {0,0,1}
        }
    }
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(1);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("Cylinder Sides"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),36);
        QGL::IndexArray ib = geom.indices();
        for (int i=pSceneNodeLevel2->start(); i<pSceneNodeLevel2->start()+pSceneNodeLevel2->count(); i+=3) {
            unsigned int ii0 = ib.at(i);
            QVERIFY(ii0<(unsigned int)verts.size());
            QVector3D vv0 = verts.at(ii0);
            float vv0sq = (vv0-QVector3D(0,0,vv0.z())).lengthSquared();
            QVERIFY(fabs(vv0sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn0 = norms.at(ii0);
            QVERIFY((fabs(QVector3D::dotProduct(nn0,(vv0-QVector3D(0,0,vv0.z()))))-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii1 = ib.at(i+1);
            QVERIFY(ii1<(unsigned int)verts.size());
            QVector3D vv1 = verts.at(ii1);
            float vv1sq = (vv1-QVector3D(0,0,vv1.z())).lengthSquared();
            QVERIFY(fabs(vv1sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn1 = norms.at(ii1);
            QVERIFY((fabs(QVector3D::dotProduct(nn1,(vv1-QVector3D(0,0,vv1.z()))))-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii2 = ib.at(i+2);
            QVERIFY(ii2<(unsigned int)verts.size());
            QVector3D vv2 = verts.at(ii2);
            float vv2sq = (vv2-QVector3D(0,0,vv2.z())).lengthSquared();
            QVERIFY(fabs(vv2sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn2 = norms.at(ii2);
            QVERIFY((fabs(QVector3D::dotProduct(nn2,(vv2-QVector3D(0,0,vv2.z()))))-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            QVector3D triangleNormal = QVector3D::crossProduct(vv1-vv0,vv2-vv0);
            triangleNormal.normalize();
            float dp = QVector3D::dotProduct(triangleNormal,nn0);
            const float epsilon = 1.0e-5f;
            QVERIFY(dp - 0.9238f > -epsilon && dp - 1.0f <= epsilon); // triangle normal points in approx the same dir as one of vertex normals. Max deviation is 22.5 degrees.
        }
    }
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(2);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("Cylinder Base"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),36);
        QGL::IndexArray ib = geom.indices();
        for (int i=pSceneNodeLevel2->start(); i<pSceneNodeLevel2->start()+pSceneNodeLevel2->count(); i+=3) {
            unsigned int ii0 = ib.at(i);
            QVERIFY(ii0<(unsigned int)verts.size());
            QVector3D vv0 = verts.at(ii0);
            float vv0sq = (vv0-QVector3D(0,0,-0.5)).lengthSquared();
            QVERIFY(fabs(vv0sq)<0.0001 || fabs(vv0sq-1.0)<0.0001); // vertex is either in center or on circle
            QVector3D nn0 = norms.at(ii0);
            QVERIFY((fabs(QVector3D::dotProduct(nn0,QVector3D(0,0,-1)))-1.0) < 0.0001); // vertex normal == {0,0,-1}

            unsigned int ii1 = ib.at(i+1);
            QVERIFY(ii1<(unsigned int)verts.size());
            QVector3D vv1 = verts.at(ii1);
            float vv1sq = (vv1-QVector3D(0,0,-0.5)).lengthSquared();
            QVERIFY(fabs(vv1sq)<0.0001 || fabs(vv1sq-1.0)<0.0001); // vertex is either in center or on circle
            QVector3D nn1 = norms.at(ii1);
            QVERIFY((fabs(QVector3D::dotProduct(nn1,QVector3D(0,0,-1)))-1.0) < 0.0001); // vertex normal == {0,0,-1}

            unsigned int ii2 = ib.at(i+2);
            QVERIFY(ii2<(unsigned int)verts.size());
            QVector3D vv2 = verts.at(ii2);
            float vv2sq = (vv2-QVector3D(0,0,-0.5)).lengthSquared();
            QVERIFY(fabs(vv2sq)<0.0001 || fabs(vv2sq-1.0)<0.0001); // vertex is either in center or on circle
            QVector3D nn2 = norms.at(ii2);
            QVERIFY((fabs(QVector3D::dotProduct(nn2,QVector3D(0,0,-1)))-1.0) < 0.0001); // vertex normal == {0,0,-1}

            QVector3D triangleNormal = QVector3D::crossProduct(vv1-vv0,vv2-vv0);
            triangleNormal.normalize();
            QVERIFY((fabs(QVector3D::dotProduct(triangleNormal,QVector3D(0,0,-1)))-1.0) < 0.0001); // triangle normal == {0,0,-1}
        }
    }
}

QTEST_APPLESS_MAIN(tst_QCylinderMesh)

#include "tst_qcylindermesh.moc"
