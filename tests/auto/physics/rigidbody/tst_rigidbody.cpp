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
#include <qbackendnodetester.h>

#include <Qt3DPhysics/qrigidbody.h>
#include <Qt3DPhysics/qphysicsmaterial.h>
#include <Qt3DPhysics/private/rigidbody_p.h>
#include <Qt3DPhysics/private/physicsmaterial_p.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qbackendnode_p.h>

#include "testpostmanarbiter.h"

class tst_RigidBody: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkInitialState()
    {
        // GIVEN
        Qt3DPhysics::Physics::RigidBody rigidBody;

        // THEN
        QCOMPARE(rigidBody.isEnabled(), false);
        QVERIFY(rigidBody.peerId().isNull());
        QVERIFY(rigidBody.materialId().isNull());
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DPhysics::QRigidBody rigidBody;
        Qt3DPhysics::QPhysicsMaterial physicsMaterial;

        rigidBody.setMaterial(&physicsMaterial);

        {
            // WHEN
            Qt3DPhysics::Physics::RigidBody backendRigidBody;
            simulateInitialization(&rigidBody, &backendRigidBody);

            // THEN
            QCOMPARE(backendRigidBody.isEnabled(), true);
            QCOMPARE(backendRigidBody.peerId(), rigidBody.id());
            QVERIFY(!backendRigidBody.materialId().isNull());
            QCOMPARE(backendRigidBody.materialId(), qIdForNode(rigidBody.material()));
        }
        {
            // WHEN
            Qt3DPhysics::Physics::RigidBody backendRigidBody;
            rigidBody.setEnabled(false);
            simulateInitialization(&rigidBody, &backendRigidBody);

            // THEN
            QCOMPARE(backendRigidBody.peerId(), rigidBody.id());
            QCOMPARE(backendRigidBody.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DPhysics::Physics::RigidBody backendRigidBody;

        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendRigidBody.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendRigidBody.isEnabled(), newValue);
        }
        {
            // WHEN
            Qt3DPhysics::QPhysicsMaterial physicsMaterial;
            auto newValue = qIdForNode(&physicsMaterial);

            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("materialId");
            change->setValue(QVariant::fromValue(newValue));
            backendRigidBody.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendRigidBody.materialId(), newValue);
            QVERIFY(!newValue.isNull());
        }
    }
};

QTEST_APPLESS_MAIN(tst_RigidBody)

#include "tst_rigidbody.moc"
