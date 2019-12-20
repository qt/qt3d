/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
