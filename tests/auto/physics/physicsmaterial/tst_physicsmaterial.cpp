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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DPhysics/private/physicsmaterial_p.h>
#include <Qt3DPhysics/QPhysicsMaterial>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testpostmanarbiter.h"

class tst_PhysicsMaterial: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkInitialState()
    {
        // GIVEN
        Qt3DPhysics::Physics::PhysicsMaterial backendPhysicsMaterial;

        // THEN
        QCOMPARE(backendPhysicsMaterial.isEnabled(), false);
        QVERIFY(backendPhysicsMaterial.peerId().isNull());
        QCOMPARE(backendPhysicsMaterial.mass(), 0.0f);
        QCOMPARE(backendPhysicsMaterial.friction(), 0.0f);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        const float initMass = 0.8f;
        const float initFriction = 1.8f;
        const QVector3D initInertia(1.f, 1.f, 1.f);

        Qt3DPhysics::QPhysicsMaterial physicsMaterial;

        physicsMaterial.setMass(initMass);
        physicsMaterial.setFriction(initFriction);

        {
            // WHEN
            Qt3DPhysics::Physics::PhysicsMaterial backendPhysicsMaterial;
            simulateInitialization(&physicsMaterial, &backendPhysicsMaterial);

            // THEN
            QCOMPARE(backendPhysicsMaterial.isEnabled(), true);
            QCOMPARE(backendPhysicsMaterial.peerId(), physicsMaterial.id());
            QCOMPARE(backendPhysicsMaterial.mass(), initMass);
            QCOMPARE(backendPhysicsMaterial.friction(), initFriction);
        }
        {
            // WHEN
            Qt3DPhysics::Physics::PhysicsMaterial backendPhysicsMaterial;
            physicsMaterial.setEnabled(false);
            simulateInitialization(&physicsMaterial, &backendPhysicsMaterial);

            // THEN
            QCOMPARE(backendPhysicsMaterial.peerId(), physicsMaterial.id());
            QCOMPARE(backendPhysicsMaterial.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DPhysics::Physics::PhysicsMaterial backendPhysicsMaterial;

        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendPhysicsMaterial.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendPhysicsMaterial.isEnabled(), newValue);
        }
        {
            // WHEN
            const float newValue = 99.0f;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("mass");
            change->setValue(QVariant::fromValue(newValue));
            backendPhysicsMaterial.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendPhysicsMaterial.mass(), newValue);
        }
        {
            // WHEN
            const float newValue = 99.0f;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("friction");
            change->setValue(QVariant::fromValue(newValue));
            backendPhysicsMaterial.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendPhysicsMaterial.friction(), newValue);
        }
    }
};

QTEST_APPLESS_MAIN(tst_PhysicsMaterial)

#include "tst_physicsmaterial.moc"
