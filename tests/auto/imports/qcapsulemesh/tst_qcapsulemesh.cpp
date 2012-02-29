/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DQuick/capsulemesh.h>

class tst_QCapsuleMesh : public QObject
{
    Q_OBJECT
public:
    tst_QCapsuleMesh() {}
    ~tst_QCapsuleMesh() {}

private slots:
    void testSignalsAndProperties();
    void testGeometry();
};

void tst_QCapsuleMesh::testSignalsAndProperties()
{
    CapsuleMesh capsule;
    {
        QCOMPARE(capsule.radius(),1.0);
        QCOMPARE(capsule.length(),2.0);
        QCOMPARE(capsule.levelOfDetail(),5);
    }
    {
        QSignalSpy spyRadius(&capsule,SIGNAL(radiusChanged()));
        capsule.setRadius(2.0);
        QCOMPARE(spyRadius.size(), 1);
        QCOMPARE(capsule.radius(),2.0);
        QCOMPARE(capsule.length(),4.0);
        capsule.setRadius(2.0);
        QCOMPARE(spyRadius.size(), 1);
    }
    {
        QSignalSpy spyLength(&capsule,SIGNAL(lengthChanged()));
        capsule.setLength(3.0);
        QCOMPARE(spyLength.size(), 1);
        QCOMPARE(capsule.length(),4.0);
        capsule.setLength(7.0);
        QCOMPARE(spyLength.size(), 2);
        QCOMPARE(capsule.length(),7.0);
        capsule.setLength(7.0);
        QCOMPARE(spyLength.size(), 2);
    }
    {
        QSignalSpy spyLOD(&capsule,SIGNAL(levelOfDetailChanged()));
        capsule.setLevelOfDetail(6);
        QCOMPARE(spyLOD.size(), 1);
        QCOMPARE(capsule.levelOfDetail(),6);

        capsule.setLevelOfDetail(9999);
        QCOMPARE(spyLOD.size(), 2);
        QCOMPARE(capsule.levelOfDetail(),10);

        capsule.setLevelOfDetail(-9999);
        QCOMPARE(spyLOD.size(), 3);
        QCOMPARE(capsule.levelOfDetail(),1);

        capsule.setLevelOfDetail(1);
        QCOMPARE(spyLOD.size(), 3);
    }
}

void tst_QCapsuleMesh::testGeometry()
{
    CapsuleMesh capsule;
    capsule.setLength(4);
    capsule.setLevelOfDetail(1);

    QGLSceneNode* pRootNode = capsule.getSceneObject();
    QVERIFY(pRootNode!=0);
    {
        QCOMPARE(pRootNode->objectName(),QString("CapsuleMesh"));
        QGeometryData rootGeom = pRootNode->geometry();
        QVector3DArray rootVerts = rootGeom.vertices();
        QCOMPARE(rootVerts.size(), 0);
        QVector3DArray rootNorms = rootGeom.normals();
        QCOMPARE(rootNorms.size(), 0);
    }

    const QObjectList& rChildren = capsule.children();
    QCOMPARE(rChildren.size(),2);
    QObject* pObj = rChildren.at(1);
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
    QCOMPARE(sceneNodeChildren.size(),6);

    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(0);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("Cylinder"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),0);
        QCOMPARE(pSceneNodeLevel2->count(),0);
    }
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(1);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("Cylinder Sides"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),72);
        QGL::IndexArray ib = geom.indices();
        for (int i=pSceneNodeLevel2->start(); i<pSceneNodeLevel2->start()+pSceneNodeLevel2->count(); i+=3) {
            unsigned int ii0 = ib.at(i);
            QVERIFY(ii0<verts.size());
            QVector3D vv0 = verts.at(ii0);
            qreal vv0sq = (vv0-QVector3D(0,0,vv0.z())).lengthSquared();
            QVERIFY(fabs(vv0sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn0 = norms.at(ii0);
            QVERIFY((fabs(QVector3D::dotProduct(nn0,(vv0-QVector3D(0,0,vv0.z()))))-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii1 = ib.at(i+1);
            QVERIFY(ii1<verts.size());
            QVector3D vv1 = verts.at(ii1);
            qreal vv1sq = (vv1-QVector3D(0,0,vv1.z())).lengthSquared();
            QVERIFY(fabs(vv1sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn1 = norms.at(ii1);
            QVERIFY((fabs(QVector3D::dotProduct(nn1,(vv1-QVector3D(0,0,vv1.z()))))-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii2 = ib.at(i+2);
            QVERIFY(ii2<verts.size());
            QVector3D vv2 = verts.at(ii2);
            qreal vv2sq = (vv2-QVector3D(0,0,vv2.z())).lengthSquared();
            QVERIFY(fabs(vv2sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn2 = norms.at(ii2);
            QVERIFY((fabs(QVector3D::dotProduct(nn2,(vv2-QVector3D(0,0,vv2.z()))))-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            QVector3D triangleNormal = QVector3D::crossProduct(vv1-vv0,vv2-vv0);
            triangleNormal.normalize();
            qreal dp = QVector3D::dotProduct(triangleNormal,nn0);
            QVERIFY(dp>0.9238 && dp<=1.0); // triangle normal points in approx the same dir as one of vertex normals. Max deviation is 22.5 degrees.
        }
    }
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(2);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("LeftEndCap"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),72);
        QCOMPARE(pSceneNodeLevel2->count(),0);
    }
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(3);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("Dome"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),72);
        QGL::IndexArray ib = geom.indices();
        for (int i=pSceneNodeLevel2->start(); i<pSceneNodeLevel2->start()+pSceneNodeLevel2->count(); i+=3) {
            unsigned int ii0 = ib.at(i);
            QVERIFY(ii0<verts.size());
            QVector3D vv0 = verts.at(ii0);
            qreal vv0sq = vv0.lengthSquared();
            QVERIFY(fabs(vv0sq-1.0)<0.0001); // vertex is on dome
            QVector3D nn0 = norms.at(ii0);
            QVERIFY(fabs(QVector3D::dotProduct(nn0,vv0))-1.0 < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii1 = ib.at(i+1);
            QVERIFY(ii1<verts.size());
            QVector3D vv1 = verts.at(ii1);
            qreal vv1sq = vv1.lengthSquared();
            QVERIFY(fabs(vv1sq-1.0)<0.0001); // vertex is on dome
            QVector3D nn1 = norms.at(ii1);
            QVERIFY(fabs(QVector3D::dotProduct(nn1,vv1))-1.0 < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii2 = ib.at(i+2);
            QVERIFY(ii2<verts.size());
            QVector3D vv2 = verts.at(ii2);
            qreal vv2sq = vv2.lengthSquared();
            QVERIFY(fabs(vv2sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn2 = norms.at(ii2);
            QVERIFY(fabs(QVector3D::dotProduct(nn2,vv2)-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            QVector3D vvv0 = vv1-vv0;
            if (vvv0.lengthSquared()>0.001) {
                QVector3D vvv1 = vv2-vv0;
                if (vvv1.lengthSquared()>0.001) {
                    vvv0.normalize();
                    vvv1.normalize();
                    QVector3D triangleNormal = QVector3D::crossProduct(vvv0,vvv1);
                    if (triangleNormal.lengthSquared()>0.001) {
                        triangleNormal.normalize();
                        qreal dp = QVector3D::dotProduct(triangleNormal,nn0);
                        QVERIFY(dp<-0.8628);
                    }
                }
            }
        }
    }
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(4);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("RightEndCap"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),72);
        QCOMPARE(pSceneNodeLevel2->count(),0);
    }
    {
        QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(5);
        QVERIFY(pSceneNodeLevel2!=0);
        QCOMPARE(pSceneNodeLevel2->objectName(),QString("Dome"));
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),72);
        QGL::IndexArray ib = geom.indices();
        for (int i=pSceneNodeLevel2->start(); i<pSceneNodeLevel2->start()+pSceneNodeLevel2->count(); i+=3) {
            unsigned int ii0 = ib.at(i);
            QVERIFY(ii0<verts.size());
            QVector3D vv0 = verts.at(ii0);
            qreal vv0sq = vv0.lengthSquared();
            QVERIFY(fabs(vv0sq-1.0)<0.0001); // vertex is on dome
            QVector3D nn0 = norms.at(ii0);
            QVERIFY(fabs(QVector3D::dotProduct(nn0,vv0))-1.0 < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii1 = ib.at(i+1);
            QVERIFY(ii1<verts.size());
            QVector3D vv1 = verts.at(ii1);
            qreal vv1sq = vv1.lengthSquared();
            QVERIFY(fabs(vv1sq-1.0)<0.0001); // vertex is on dome
            QVector3D nn1 = norms.at(ii1);
            QVERIFY(fabs(QVector3D::dotProduct(nn1,vv1))-1.0 < 0.0001); // vertex normal points to the same direction as vertex

            unsigned int ii2 = ib.at(i+2);
            QVERIFY(ii2<verts.size());
            QVector3D vv2 = verts.at(ii2);
            qreal vv2sq = vv2.lengthSquared();
            QVERIFY(fabs(vv2sq-1.0)<0.0001); // vertex is on cylinder
            QVector3D nn2 = norms.at(ii2);
            QVERIFY(fabs(QVector3D::dotProduct(nn2,vv2)-1.0) < 0.0001); // vertex normal points to the same direction as vertex

            QVector3D vvv0 = vv1-vv0;
            if (vvv0.lengthSquared()>0.001) {
                QVector3D vvv1 = vv2-vv0;
                if (vvv1.lengthSquared()>0.001) {
                    vvv0.normalize();
                    vvv1.normalize();
                    QVector3D triangleNormal = QVector3D::crossProduct(vvv0,vvv1);
                    if (triangleNormal.lengthSquared()>0.001) {
                        triangleNormal.normalize();
                        qreal dp = QVector3D::dotProduct(triangleNormal,nn0);
                        QVERIFY(dp<-0.8628);
                    }
                }
            }
        }
    }
}

QTEST_APPLESS_MAIN(tst_QCapsuleMesh)

#include "tst_qcapsulemesh.moc"




