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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qcameraselector.h>
#include <Qt3DCore/qentity.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QCameraSelector: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QCameraSelector()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QCameraSelector *>("cameraSelector");
        QTest::addColumn<Qt3DCore::QEntity *>("camera");

        Qt3DRender::QCameraSelector *defaultConstructed = new Qt3DRender::QCameraSelector();
        QTest::newRow("defaultConstructed") << defaultConstructed << static_cast<Qt3DCore::QEntity *>(Q_NULLPTR);

        Qt3DRender::QCameraSelector *selector1 = new Qt3DRender::QCameraSelector();
        Qt3DCore::QEntity *camera1 = new Qt3DCore::QEntity();
        QTest::newRow("valid camera") << selector1 << camera1;
        selector1->setCamera(camera1);
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QCameraSelector *, cameraSelector);
        QFETCH(Qt3DCore::QEntity *, camera);

        // WHEN
        Qt3DRender::QCameraSelector *clone = static_cast<Qt3DRender::QCameraSelector *>(QNode::clone(cameraSelector));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(cameraSelector->id(), clone->id());
        if (cameraSelector->camera()) {
            QVERIFY(clone->camera());
            QCOMPARE(clone->camera()->id(), camera->id());
        } else {
            QVERIFY(clone->camera() == Q_NULLPTR);
        }

        delete cameraSelector;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QCameraSelector> cameraSelector(new Qt3DRender::QCameraSelector());
        TestArbiter arbiter(cameraSelector.data());

        // WHEN
        Qt3DCore::QEntity *camera = new Qt3DCore::QEntity();
        cameraSelector->setCamera(camera);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "camera");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), camera->id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        cameraSelector->setCamera(camera);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        Qt3DCore::QEntity *camera2 = new Qt3DCore::QEntity();
        cameraSelector->setCamera(camera2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "camera");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), camera2->id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        arbiter.events.clear();

        // WHEN
        cameraSelector->setCamera(Q_NULLPTR);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "camera");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), Qt3DCore::QNodeId());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QCameraSelector)

#include "tst_qcameraselector.moc"
