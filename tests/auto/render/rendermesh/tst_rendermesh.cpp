/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DRenderer/private/meshdatamanager_p.h>
#include <Qt3DRenderer/private/rendermesh_p.h>
#include <Qt3DRenderer/QAbstractMesh>
#include <Qt3DRenderer/QAbstractMeshFunctor>
#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QPlaneMesh>
#include <Qt3DRenderer/QCuboidMesh>
#include <Qt3DRenderer/QSphereMesh>
#include <Qt3DRenderer/QTorusMesh>
#include <Qt3DRenderer/QMesh>
#include <Qt3DCore/QScenePropertyChange>


using namespace Qt3D;
using namespace Qt3D::Render;

class tst_RenderMesh : public QObject
{
    Q_OBJECT
public:
    tst_RenderMesh() {}

private slots:
    void shouldHaveInitialState();
    void shouldHavePropertiesMirroringFromItsPeer_data();
    void shouldHavePropertiesMirroringFromItsPeer();
    void shouldHandleMeshFunctorPropertyChange();
    void shouldHandleEnablePropertyChange();
};


void tst_RenderMesh::shouldHaveInitialState()
{
    // GIVEN
    RenderMesh backend;

    // THEN
    QVERIFY(backend.meshFunctor().isNull());
    QVERIFY(backend.isEnabled());
}

void tst_RenderMesh::shouldHavePropertiesMirroringFromItsPeer_data()
{
    QTest::addColumn<QAbstractMesh *>("frontendMesh");

    QTest::newRow("planeMesh") << (QAbstractMesh *)new QPlaneMesh();
    QTest::newRow("cuboidMesh") << (QAbstractMesh *)new QCuboidMesh();
    QTest::newRow("sphereMesh") << (QAbstractMesh *)new QSphereMesh();
    QTest::newRow("torusMesh") << (QAbstractMesh *)new QTorusMesh();
    QTest::newRow("cylinderMesh") << (QAbstractMesh *)new QCylinderMesh();
    QTest::newRow("mesh") << (QAbstractMesh *)new QMesh();
}

void tst_RenderMesh::shouldHavePropertiesMirroringFromItsPeer()
{
    // GIVEN
    QFETCH(QAbstractMesh *, frontendMesh);
    RenderMesh backend;
    QScopedPointer<MeshDataManager> manager(new MeshDataManager());
    backend.setMeshDataManager(manager.data());

    // WHEN
    backend.setPeer(frontendMesh);

    // THEN
    QVERIFY(backend.isEnabled() == frontendMesh->isEnabled());
    QVERIFY(backend.meshFunctor());
    QVERIFY(*backend.meshFunctor() == *frontendMesh->meshFunctor());

    delete frontendMesh;
}

void tst_RenderMesh::shouldHandleMeshFunctorPropertyChange()
{
    // GIVEN
    RenderMesh backend;
    QScopedPointer<MeshDataManager> manager(new MeshDataManager());
    backend.setMeshDataManager(manager.data());

    QScopedPointer<QPlaneMesh> planeMesh(new QPlaneMesh());
    QScopedPointer<QSphereMesh> sphereMesh(new QSphereMesh());
    QAbstractMeshFunctorPtr meshFunctor(planeMesh->meshFunctor());

    // THEN
    QVERIFY(backend.meshFunctor() != meshFunctor);

    // WHEN
    QScenePropertyChangePtr updateChange(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, QNodeId()));

    updateChange->setValue(QVariant::fromValue(meshFunctor));
    updateChange->setPropertyName("meshFunctor");
    backend.sceneChangeEvent(updateChange);

    // THEN
    QVERIFY(backend.meshFunctor() == meshFunctor);

    // GIVEN
    meshFunctor = sphereMesh->meshFunctor();

    // THEN
    QVERIFY(backend.meshFunctor() != meshFunctor);

    // WHEN
    QScenePropertyChangePtr secondUpdateChange(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, QNodeId()));
    secondUpdateChange->setValue(QVariant::fromValue(meshFunctor));
    secondUpdateChange->setPropertyName("meshFunctor");
    backend.sceneChangeEvent(secondUpdateChange);

    // THEN
    QVERIFY(backend.meshFunctor() == meshFunctor);
}

void tst_RenderMesh::shouldHandleEnablePropertyChange()
{
    // GIVEN
    RenderMesh backend;
    QScopedPointer<MeshDataManager> manager(new MeshDataManager());
    backend.setMeshDataManager(manager.data());

    // WHEN
    QScenePropertyChangePtr updateChange(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, QNodeId()));
    updateChange->setValue(false);
    updateChange->setPropertyName("enabled");
    backend.sceneChangeEvent(updateChange);

    // THEN
    QVERIFY(!backend.isEnabled());

    // WHEN
    QScenePropertyChangePtr secondUpdateChange(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, QNodeId()));
    secondUpdateChange->setValue(true);
    secondUpdateChange->setPropertyName("enabled");
    backend.sceneChangeEvent(secondUpdateChange);

    // THEN
    QVERIFY(backend.isEnabled());
}

QTEST_MAIN(tst_RenderMesh)

#include "tst_rendermesh.moc"
