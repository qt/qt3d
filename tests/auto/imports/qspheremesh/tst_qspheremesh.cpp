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
#include <Qt3DQuick/spheremesh.h>

class tst_QSphereMesh : public QObject
{
    Q_OBJECT
public:
    tst_QSphereMesh() {}
    ~tst_QSphereMesh() {}

private slots:
    void testSignalsAndProperties();
    void testGeometry();
};

void tst_QSphereMesh::testSignalsAndProperties()
{
    SphereMesh sphere;
    {
        QCOMPARE(sphere.radius(),0.5);
        QCOMPARE(sphere.levelOfDetail(),5);
        QCOMPARE(sphere.axis(),Qt::ZAxis);
    }
    {
        QSignalSpy spyRadius(&sphere,SIGNAL(radiusChanged()));
        sphere.setRadius(2.0);
        QCOMPARE(spyRadius.size(), 1);
        QCOMPARE(sphere.radius(),2.0);
    }
    {
        QSignalSpy spyLOD(&sphere,SIGNAL(levelOfDetailChanged()));
        sphere.setLevelOfDetail(6);
        QCOMPARE(spyLOD.size(), 1);
        QCOMPARE(sphere.levelOfDetail(),6);

        sphere.setLevelOfDetail(9999);
        QCOMPARE(spyLOD.size(), 2);
        QCOMPARE(sphere.levelOfDetail(),10);

        sphere.setLevelOfDetail(-9999);
        QCOMPARE(spyLOD.size(), 3);
        QCOMPARE(sphere.levelOfDetail(),1);
    }
    {
        QSignalSpy spyAxis(&sphere,SIGNAL(axisChanged()));
        sphere.setAxis(Qt::XAxis);
        QCOMPARE(spyAxis.size(), 1);
        QCOMPARE(sphere.axis(),Qt::XAxis);
    }
}
void tst_QSphereMesh::testGeometry()
{
    SphereMesh sphere;
    sphere.setLevelOfDetail(1);

    QGLSceneNode* pRootNode = sphere.getSceneObject();
    QVERIFY(pRootNode!=0);
    {
        QCOMPARE(pRootNode->objectName(),QString("SphereMesh"));
        QGeometryData rootGeom = pRootNode->geometry();
        QVector3DArray rootVerts = rootGeom.vertices();
        QCOMPARE(rootVerts.size(), 0);
        QVector3DArray rootNorms = rootGeom.normals();
        QCOMPARE(rootNorms.size(), 0);
    }

    const QObjectList& rChildren = sphere.children();
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
    QCOMPARE(sceneNodeChildren.size(),1);
    QGLSceneNode* pSceneNodeLevel2 = sceneNodeChildren.at(0);
    QVERIFY(pSceneNodeLevel2!=0);
    {
        QGeometryData geom = pSceneNodeLevel2->geometry();
        QVector3DArray verts = geom.vertices();
        QVector3DArray norms = geom.normals();
        QVERIFY(verts.size()==norms.size());
        QCOMPARE(verts.size(),45);
        QGL::IndexArray ib = geom.indices();
        for (int i=pSceneNodeLevel2->start(); i<pSceneNodeLevel2->start()+pSceneNodeLevel2->count(); i+=3) {
            unsigned int ii0 = ib.at(i);
            QVERIFY(ii0<(unsigned int)verts.size());
            QVector3D vv0 = verts.at(ii0);
            float vv0sq = vv0.lengthSquared();
            QVERIFY(fabs(vv0sq-1.0)<0.0001); // vertex is on sphere
            QVector3D nn0 = norms.at(ii0);
            QVERIFY((fabs(QVector3D::dotProduct(vv0,nn0))-1.0) < 0.0001); // vertex normal == vertex itself
            unsigned int ii1 = ib.at(i+1);
            QVERIFY(ii1<(unsigned int)verts.size());
            QVector3D vv1 = verts.at(ii1);
            float vv1sq = vv1.lengthSquared();
            QVERIFY(fabs(vv1sq-1.0)<0.0001); // vertex is on sphere
            QVector3D nn1 = norms.at(ii1);
            QVERIFY((fabs(QVector3D::dotProduct(vv1,nn1))-1.0) < 0.0001); // vertex normal == vertex itself
            unsigned int ii2 = ib.at(i+2);
            QVERIFY(ii2<(unsigned int)verts.size());
            QVector3D vv2 = verts.at(ii2);
            float vv2sq = vv2.lengthSquared();
            QVERIFY(fabs(vv2sq-1.0)<0.0001); // vertex is on sphere
            QVector3D nn2 = norms.at(ii2);
            QVERIFY((fabs(QVector3D::dotProduct(vv2,nn2))-1.0) < 0.0001); // vertex normal == vertex itself

            QVector3D vvv0 = vv1-vv0;
            if (vvv0.lengthSquared()>0.001) {
                QVector3D vvv1 = vv2-vv0;
                if (vvv1.lengthSquared()>0.001) {
                    vvv0.normalize();
                    vvv1.normalize();
                    QVector3D triangleNormal = QVector3D::crossProduct(vvv0,vvv1);
                    if (triangleNormal.lengthSquared()>0.001) {
                        triangleNormal.normalize();
                        float dp = QVector3D::dotProduct(triangleNormal,nn0);
                        QVERIFY(dp>0.8628);
                    }
                }
            }
        }
    }
}

QTEST_APPLESS_MAIN(tst_QSphereMesh)

#include "tst_qspheremesh.moc"
