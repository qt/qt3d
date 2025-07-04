// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qskeleton.h>
#include <Qt3DAnimation/private/skeleton_p.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testarbiter.h>

#include <QtGui/qquaternion.h>

using namespace Qt3DCore;
using namespace Qt3DAnimation;
using namespace Qt3DAnimation::Animation;

Q_DECLARE_METATYPE(Skeleton*)

class tst_Skeleton : public QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Handler handler;
        Skeleton backendSkeleton;
        backendSkeleton.setHandler(&handler);
        QSkeleton skeleton;
        auto rootJoint = new QJoint();
        rootJoint->setName(QLatin1String("rootJoint"));
        auto childJoint = new QJoint();
        childJoint->setName(QLatin1String("childJoint"));
        rootJoint->addChildJoint(childJoint);
        skeleton.setRootJoint(rootJoint);

        // WHEN
        simulateInitializationSync(&skeleton, &backendSkeleton);

        // THEN - nothing mirrored from frontend
        QCOMPARE(backendSkeleton.peerId(), skeleton.id());
        QCOMPARE(backendSkeleton.isEnabled(), skeleton.isEnabled());
        QCOMPARE(backendSkeleton.jointNames().size(), 0);
        QCOMPARE(backendSkeleton.jointLocalPoses().size(), 0);
    }

    void checkJointName()
    {
        // GIVEN
        Skeleton backendSkeleton;
        const QVector<QString> jointNames = { QLatin1String("rootJoint"),
                                              QLatin1String("child1Joint"),
                                              QLatin1String("child2Joint") };

        // WHEN
        backendSkeleton.setJointNames(jointNames);

        // THEN
        const int jointNameCount = jointNames.size();
        for (int i = 0; i < jointNameCount; ++i) {
            QCOMPARE(jointNames[i], backendSkeleton.jointName(i));
        }
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Handler handler;
        Skeleton backendSkeleton;
        backendSkeleton.setHandler(&handler);

        // THEN
        QVERIFY(backendSkeleton.peerId().isNull());
        QCOMPARE(backendSkeleton.isEnabled(), false);
        QCOMPARE(backendSkeleton.jointNames().size(), 0);
        QCOMPARE(backendSkeleton.jointLocalPoses().size(), 0);

        // GIVEN
        const QVector<QString> names = { QLatin1String("root"),
                                         QLatin1String("child1"),
                                         QLatin1String("child2") };
        const QVector<Sqt> localPoses = { Sqt(), Sqt(), Sqt() };

        // WHEN
        backendSkeleton.setJointNames(names);
        backendSkeleton.setJointLocalPoses(localPoses);
        backendSkeleton.cleanup();

        // THEN
        QCOMPARE(backendSkeleton.isEnabled(), false);
        QCOMPARE(backendSkeleton.jointNames().size(), 0);
        QCOMPARE(backendSkeleton.jointLocalPoses().size(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Handler handler;
        Skeleton backendSkeleton;
        backendSkeleton.setHandler(&handler);
        QSkeleton skeleton;
        simulateInitializationSync(&skeleton, &backendSkeleton);

        // WHEN
        skeleton.setEnabled(false);
        backendSkeleton.syncFromFrontEnd(&skeleton, false);

        // THEN
        QCOMPARE(backendSkeleton.isEnabled(), false);
    }

    void checkJointTransforms_data()
    {
        QTest::addColumn<Skeleton*>("skeleton");
        QTest::addColumn<QList<Sqt>>("jointTransforms");

        const int count = 5;
        auto skeleton = new Skeleton;
        skeleton->setJointCount(count);
        QList<Sqt> jointTransforms;
        jointTransforms.reserve(count);
        for (int i = 0; i < count; ++i) {
            const float f = float(i);
            Sqt transform;
            transform.scale = QVector3D(f, f, f);
            transform.rotation = QQuaternion(f, 1.0f, 0.0f, 0.0f).normalized();
            transform.translation = QVector3D(1.0 * f, 2.0 * f, 3.0 * f);
            skeleton->setJointScale(i, transform.scale);
            skeleton->setJointRotation(i, transform.rotation);
            skeleton->setJointTranslation(i, transform.translation);
            jointTransforms.push_back(transform);
        }

        QTest::newRow("5 joints") << skeleton << jointTransforms;
    }

    void checkJointTransforms()
    {
        // GIVEN
        QFETCH(Skeleton*, skeleton);
        QFETCH(QList<Sqt>, jointTransforms);

        const int count = skeleton->jointCount();
        for (int i = 0; i < count; ++i) {
            // WHEN
            const QVector3D s = skeleton->jointScale(i);

            // THEN
            QCOMPARE(s, jointTransforms[i].scale);

            // WHEN
            const QQuaternion q = skeleton->jointRotation(i);

            // THEN
            QCOMPARE(q, jointTransforms[i].rotation);

            // WHEN
            const QVector3D t = skeleton->jointTranslation(i);

            // THEN
            QCOMPARE(t, jointTransforms[i].translation);
        }

        // Cleanup
        delete skeleton;
    }
};

QTEST_APPLESS_MAIN(tst_Skeleton)

#include "tst_skeleton.moc"
