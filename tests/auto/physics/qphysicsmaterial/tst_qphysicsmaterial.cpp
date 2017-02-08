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
#include <Qt3DPhysics/QPhysicsMaterial>
#include <Qt3DPhysics/private/qphysicsmaterial_p.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>

#include <Qt3DCore/QPropertyUpdatedChange>
#include <Qt3DCore/QPropertyNodeAddedChange>
#include <Qt3DCore/QPropertyNodeRemovedChange>
#include "testpostmanarbiter.h"

class tst_QPhysicsMaterial: public Qt3DPhysics::QPhysicsMaterial
{
    Q_OBJECT
public:
    tst_QPhysicsMaterial()
    {
    }

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DPhysics::QPhysicsMaterial physicsMaterial;

        // THEN
        QCOMPARE(physicsMaterial.mass(), 0.0f);
        QCOMPARE(physicsMaterial.friction(), 0.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DPhysics::QPhysicsMaterial physicsMaterial;

        {
            // WHEN
            QSignalSpy spy(&physicsMaterial, SIGNAL(massChanged(float)));
            const float newValue = 0.5f;
            physicsMaterial.setMass(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(physicsMaterial.mass(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            physicsMaterial.setMass(newValue);

            // THEN
            QCOMPARE(physicsMaterial.mass(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&physicsMaterial, SIGNAL(frictionChanged(float)));
            const float newValue = 1.5f;
            physicsMaterial.setFriction(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(physicsMaterial.friction(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            physicsMaterial.setFriction(newValue);

            // THEN
            QCOMPARE(physicsMaterial.friction(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DPhysics::QPhysicsMaterial physicsMaterial;

        physicsMaterial.setMass(0.8f);
        physicsMaterial.setFriction(1.8f);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&physicsMaterial);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DPhysics::QPhysicsMaterialData>>(creationChanges.first());
            const Qt3DPhysics::QPhysicsMaterialData cloneData = creationChangeData->data;

            QCOMPARE(physicsMaterial.mass(), cloneData.mass);
            QCOMPARE(physicsMaterial.friction(), cloneData.friction);
            QCOMPARE(physicsMaterial.id(), creationChangeData->subjectId());
            QCOMPARE(physicsMaterial.isEnabled(), true);
            QCOMPARE(physicsMaterial.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(physicsMaterial.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        physicsMaterial.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&physicsMaterial);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DPhysics::QPhysicsMaterialData>>(creationChanges.first());
            const Qt3DPhysics::QPhysicsMaterialData cloneData = creationChangeData->data;

            QCOMPARE(physicsMaterial.mass(), cloneData.mass);
            QCOMPARE(physicsMaterial.friction(), cloneData.friction);
            QCOMPARE(physicsMaterial.id(), creationChangeData->subjectId());
            QCOMPARE(physicsMaterial.isEnabled(), false);
            QCOMPARE(physicsMaterial.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(physicsMaterial.metaObject(), creationChangeData->metaObject());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DPhysics::QPhysicsMaterial physicsMaterial;
        arbiter.setArbiterOnNode(&physicsMaterial);

        {
            // WHEN
            physicsMaterial.setMass(0.7f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "mass");
            QCOMPARE(change->value().value<float>(), physicsMaterial.mass());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            physicsMaterial.setMass(0.7f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

        {
            // WHEN
            physicsMaterial.setFriction(1.7f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "friction");
            QCOMPARE(change->value().value<float>(), physicsMaterial.friction());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            physicsMaterial.setFriction(1.7f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }
    }

};

QTEST_MAIN(tst_QPhysicsMaterial)

#include "tst_qphysicsmaterial.moc"
