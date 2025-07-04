// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/private/qjoint_p.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

#include <QtGui/qquaternion.h>

using namespace Qt3DCore;

class tst_QJoint : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        QJoint joint;

        // THEN
        QCOMPARE(joint.scale(), QVector3D(1.0f, 1.0f, 1.0f));
        QCOMPARE(joint.rotation(), QQuaternion());
        QCOMPARE(joint.translation(), QVector3D(0.0f, 0.0f, 0.0f));
        QCOMPARE(joint.inverseBindMatrix(), QMatrix4x4());
        QCOMPARE(joint.rotationX(), 0.0f);
        QCOMPARE(joint.rotationY(), 0.0f);
        QCOMPARE(joint.rotationZ(), 0.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        QJoint joint;

        {
            // WHEN
            joint.setToIdentity();
            QSignalSpy spy(&joint, SIGNAL(scaleChanged(QVector3D)));
            const QVector3D newValue(2.5f, 2.0f, 1.3f);
            joint.setScale(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(joint.scale(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            joint.setScale(newValue);

            // THEN
            QCOMPARE(joint.scale(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            joint.setToIdentity();
            QSignalSpy spy(&joint, SIGNAL(rotationChanged(QQuaternion)));
            const auto newValue = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 45.0f);
            joint.setRotation(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(joint.rotation(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            joint.setRotation(newValue);

            // THEN
            QCOMPARE(joint.rotation(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            joint.setToIdentity();
            QSignalSpy spy(&joint, SIGNAL(translationChanged(QVector3D)));
            const QVector3D newValue(1.0f, 2.0f, 3.0f);
            joint.setTranslation(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(joint.translation(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            joint.setTranslation(newValue);

            // THEN
            QCOMPARE(joint.translation(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            joint.setToIdentity();
            QSignalSpy spy(&joint, SIGNAL(inverseBindMatrixChanged(QMatrix4x4)));
            QMatrix4x4 newValue;
            newValue.scale(3.5f);
            joint.setInverseBindMatrix(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(joint.inverseBindMatrix(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            joint.setInverseBindMatrix(newValue);

            // THEN
            QCOMPARE(joint.inverseBindMatrix(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            joint.setToIdentity();
            QSignalSpy spy(&joint, SIGNAL(rotationChanged(QQuaternion)));
            QSignalSpy spyEuler(&joint, SIGNAL(rotationXChanged(float)));
            const auto newValue = 45.0f;
            const auto newValueAsQuaternion = QQuaternion::fromEulerAngles(newValue, 0.0f, 0.0f);
            joint.setRotationX(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QVERIFY(spyEuler.isValid());
            QCOMPARE(joint.rotationX(), newValue);
            QCOMPARE(joint.rotation(), newValueAsQuaternion);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spyEuler.size(), 1);

            // WHEN
            spy.clear();
            spyEuler.clear();
            joint.setRotationX(newValue);

            // THEN
            QCOMPARE(joint.rotationX(), newValue);
            QCOMPARE(joint.rotation(), newValueAsQuaternion);
            QCOMPARE(spy.size(), 0);
            QCOMPARE(spyEuler.size(), 0);

            joint.setRotationX(0.0f);
        }

        {
            // WHEN
            joint.setToIdentity();
            QSignalSpy spy(&joint, SIGNAL(rotationChanged(QQuaternion)));
            QSignalSpy spyEuler(&joint, SIGNAL(rotationYChanged(float)));
            const auto newValue = 45.0f;
            const auto newValueAsQuaternion = QQuaternion::fromEulerAngles(0.0f, newValue, 0.0f);
            joint.setRotationY(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QVERIFY(spyEuler.isValid());
            QCOMPARE(joint.rotationY(), newValue);
            QCOMPARE(joint.rotation(), newValueAsQuaternion);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spyEuler.size(), 1);

            // WHEN
            spy.clear();
            spyEuler.clear();
            joint.setRotationY(newValue);

            // THEN
            QCOMPARE(joint.rotationY(), newValue);
            QCOMPARE(joint.rotation(), newValueAsQuaternion);
            QCOMPARE(spy.size(), 0);
            QCOMPARE(spyEuler.size(), 0);

            joint.setRotationY(0.0f);
        }

        {
            // WHEN
            joint.setToIdentity();
            QSignalSpy spy(&joint, SIGNAL(rotationChanged(QQuaternion)));
            QSignalSpy spyEuler(&joint, SIGNAL(rotationZChanged(float)));
            const auto newValue = 45.0f;
            const auto newValueAsQuaternion = QQuaternion::fromEulerAngles(0.0f, 0.0f, newValue);
            joint.setRotationZ(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QVERIFY(spyEuler.isValid());
            QCOMPARE(joint.rotationZ(), newValue);
            QCOMPARE(joint.rotation(), newValueAsQuaternion);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spyEuler.size(), 1);

            // WHEN
            spy.clear();
            spyEuler.clear();
            joint.setRotationZ(newValue);

            // THEN
            QCOMPARE(joint.rotationZ(), newValue);
            QCOMPARE(joint.rotation(), newValueAsQuaternion);
            QCOMPARE(spy.size(), 0);
            QCOMPARE(spyEuler.size(), 0);

            joint.setRotationZ(0.0f);
        }
    }

    void checkPropertyUpdateChanges()
    {
        // GIVEN
        TestArbiter arbiter;
        QJoint joint;
        arbiter.setArbiterOnNode(&joint);

        {
            // WHEN
            joint.setScale(QVector3D(2.0f, 1.0f, 3.0f));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &joint);

            arbiter.clear();

            // WHEN
            joint.setScale(QVector3D(2.0f, 1.0f, 3.0f));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            const auto newValue = QQuaternion::fromAxisAndAngle(1.0f, 1.0f, 1.0f, 45.0f);
            joint.setRotation(newValue);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &joint);

            arbiter.clear();

            // WHEN
            joint.setRotation(newValue);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            const QVector3D newValue(1.0f, 2.0f, 3.0f);
            joint.setTranslation(newValue);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &joint);

            arbiter.clear();

            // WHEN
            joint.setTranslation(newValue);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            QMatrix4x4 newValue;
            newValue.rotate(90.0f, 1.0f, 0.0f, 0.0f);
            joint.setInverseBindMatrix(newValue);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &joint);

            arbiter.clear();

            // WHEN
            joint.setInverseBindMatrix(newValue);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }
};

QTEST_MAIN(tst_QJoint)

#include "tst_qjoint.moc"
