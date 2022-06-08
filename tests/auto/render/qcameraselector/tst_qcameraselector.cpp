// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/private/qcameraselector_p.h>
#include <Qt3DCore/qentity.h>
#include <testarbiter.h>

class tst_QCameraSelector: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QCameraSelector> cameraSelector(new Qt3DRender::QCameraSelector());
        arbiter.setArbiterOnNode(cameraSelector.data());

        // WHEN
        Qt3DCore::QEntity *camera = new Qt3DCore::QEntity();
        cameraSelector->setCamera(camera);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), cameraSelector.data());

        arbiter.clear();

        // WHEN
        cameraSelector->setCamera(camera);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        Qt3DCore::QEntity *camera2 = new Qt3DCore::QEntity();
        cameraSelector->setCamera(camera2);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), cameraSelector.data());

        arbiter.clear();

        // WHEN
        cameraSelector->setCamera(nullptr);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), cameraSelector.data());

        arbiter.clear();
    }

    void checkCameraBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QCameraSelector> cameraSelector(new Qt3DRender::QCameraSelector);
        {
            // WHEN
            Qt3DCore::QEntity camera;
            cameraSelector->setCamera(&camera);

            // THEN
            QCOMPARE(camera.parent(), cameraSelector.data());
            QCOMPARE(cameraSelector->camera(), &camera);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(cameraSelector->camera() == nullptr);

        {
            // WHEN
            Qt3DRender::QCameraSelector someOtherCameraSelector;
            QScopedPointer<Qt3DCore::QEntity> camera(new Qt3DCore::QEntity(&someOtherCameraSelector));
            cameraSelector->setCamera(camera.data());

            // THEN
            QCOMPARE(camera->parent(), &someOtherCameraSelector);
            QCOMPARE(cameraSelector->camera(), camera.data());

            // WHEN
            cameraSelector.reset();
            camera.reset();

            // THEN Should not crash when the camera is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QCameraSelector)

#include "tst_qcameraselector.moc"
