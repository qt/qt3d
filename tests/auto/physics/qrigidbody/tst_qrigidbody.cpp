/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>

#include <QSignalSpy>
#include <Qt3DPhysics/qrigidbody.h>
#include <Qt3DPhysics/qphysicsmaterial.h>
#include <Qt3DPhysics/private/qrigidbody_p.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>

#include <Qt3DCore/qpropertyupdatedchange.h>
#include "testpostmanarbiter.h"

class tst_QRigidBody: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DPhysics::QRigidBody rigidBody;

        // THEN
        QCOMPARE(rigidBody.material(), nullptr);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DPhysics::QRigidBody rigidBody;
        Qt3DPhysics::QPhysicsMaterial material1;

        {
            // WHEN
            QSignalSpy spy(&rigidBody, SIGNAL(materialChanged(QPhysicsMaterial*)));
            rigidBody.setMaterial(&material1);

            // THEN
            QVERIFY(spy.isValid());
            QVERIFY(rigidBody.material() == &material1);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            rigidBody.setMaterial(&material1);

            // THEN
            QVERIFY(rigidBody.material() == &material1);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DPhysics::QRigidBody rigidBody;
        Qt3DPhysics::QPhysicsMaterial physicsMaterial;

        rigidBody.setMaterial(&physicsMaterial);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&rigidBody);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData
                = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DPhysics::QRigidBodyData>>(creationChanges.first());
            const Qt3DPhysics::QRigidBodyData cloneData = creationChangeData->data;

            QCOMPARE(qIdForNode(rigidBody.material()), cloneData.materialId);
            QCOMPARE(rigidBody.id(), creationChangeData->subjectId());
            QCOMPARE(rigidBody.isEnabled(), true);
            QCOMPARE(rigidBody.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(rigidBody.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        rigidBody.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&rigidBody);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData
                = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DPhysics::QRigidBodyData>>(creationChanges.first());
            const Qt3DPhysics::QRigidBodyData cloneData = creationChangeData->data;

            QCOMPARE(qIdForNode(rigidBody.material()), cloneData.materialId);
            QCOMPARE(rigidBody.id(), creationChangeData->subjectId());
            QCOMPARE(rigidBody.isEnabled(), false);
            QCOMPARE(rigidBody.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(rigidBody.metaObject(), creationChangeData->metaObject());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DPhysics::QRigidBody rigidBody;
        Qt3DPhysics::QPhysicsMaterial physicsMaterial;
        arbiter.setArbiterOnNode(&rigidBody);

        {
            // WHEN
            rigidBody.setMaterial(&physicsMaterial);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "material");
            QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), qIdForNode(rigidBody.material()));
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            rigidBody.setMaterial(&physicsMaterial);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }
    }

};

QTEST_MAIN(tst_QRigidBody)

#include "tst_qrigidbody.moc"
