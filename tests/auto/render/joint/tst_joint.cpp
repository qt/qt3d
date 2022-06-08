// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DRender/private/joint_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qvector3d.h>
#include <qbackendnodetester.h>
#include <testarbiter.h>
#include <testrenderer.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

class tst_Joint : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Joint backendJoint;
        backendJoint.setRenderer(&renderer);
        backendJoint.setJointManager(nodeManagers.jointManager());
        backendJoint.setSkeletonManager(nodeManagers.skeletonManager());
        QJoint joint;

        joint.setTranslation(QVector3D(1.0f, 2.0f, 3.0f));
        joint.setScale(QVector3D(1.5f, 2.5f, 3.5));
        joint.setRotation(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 45.0f));
        QMatrix4x4 inverseBind;
        inverseBind.rotate(-45.0f, 1.0f, 0.0, 0.0f);
        joint.setInverseBindMatrix(inverseBind);

        QList<QJoint *> childJoints;
        for (int i = 0; i < 10; ++i) {
            const auto childJoint = new QJoint();
            joint.addChildJoint(childJoint);
            childJoints.push_back(childJoint);
        }

        // WHEN
        simulateInitializationSync(&joint, &backendJoint);

        // THEN
        QCOMPARE(backendJoint.peerId(), joint.id());
        QCOMPARE(backendJoint.isEnabled(), joint.isEnabled());
        QCOMPARE(backendJoint.translation(), joint.translation());
        QCOMPARE(backendJoint.rotation(), joint.rotation());
        QCOMPARE(backendJoint.scale(), joint.scale());
        QCOMPARE(backendJoint.inverseBindMatrix(), joint.inverseBindMatrix());
        for (int i = 0; i < childJoints.size(); ++i) {
            QCOMPARE(backendJoint.childJointIds()[i], childJoints[i]->id());
        }
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Joint backendJoint;
        backendJoint.setRenderer(&renderer);
        backendJoint.setJointManager(nodeManagers.jointManager());
        backendJoint.setSkeletonManager(nodeManagers.skeletonManager());

        // THEN
        QVERIFY(backendJoint.peerId().isNull());
        QCOMPARE(backendJoint.isEnabled(), false);
        QCOMPARE(backendJoint.translation(), QVector3D());
        QCOMPARE(backendJoint.rotation(), QQuaternion());
        QCOMPARE(backendJoint.scale(), QVector3D(1.0f, 1.0f, 1.0f));
        QCOMPARE(backendJoint.inverseBindMatrix(), QMatrix4x4());
        QCOMPARE(backendJoint.childJointIds(), QNodeIdVector());
        QCOMPARE(backendJoint.owningSkeleton(), HSkeleton());

        // GIVEN
        QJoint joint;
        joint.setTranslation(QVector3D(1.0f, 2.0f, 3.0f));
        joint.setScale(QVector3D(1.5f, 2.5f, 3.5));
        joint.setRotation(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 45.0f));
        QMatrix4x4 inverseBind;
        inverseBind.rotate(-45.0f, 1.0f, 0.0, 0.0f);
        joint.setInverseBindMatrix(inverseBind);

        QList<QJoint *> childJoints;
        for (int i = 0; i < 10; ++i) {
            const auto childJoint = new QJoint();
            joint.addChildJoint(childJoint);
            childJoints.push_back(childJoint);
        }

        // WHEN
        simulateInitializationSync(&joint, &backendJoint);
        backendJoint.cleanup();

        // THEN
        QCOMPARE(backendJoint.isEnabled(), false);
        QCOMPARE(backendJoint.translation(), QVector3D());
        QCOMPARE(backendJoint.rotation(), QQuaternion());
        QCOMPARE(backendJoint.scale(), QVector3D(1.0f, 1.0f, 1.0f));
        QCOMPARE(backendJoint.inverseBindMatrix(), QMatrix4x4());
        QCOMPARE(backendJoint.childJointIds(), QNodeIdVector());
        QCOMPARE(backendJoint.owningSkeleton(), HSkeleton());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Joint backendJoint;
        backendJoint.setRenderer(&renderer);
        backendJoint.setJointManager(nodeManagers.jointManager());
        backendJoint.setSkeletonManager(nodeManagers.skeletonManager());
        QJoint joint;
        simulateInitializationSync(&joint, &backendJoint);

        // WHEN
        joint.setEnabled(false);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.isEnabled(), false);

        // WHEN
        const QVector3D newTranslation = QVector3D(1.0f, 2.0f, 3.0f);
        joint.setTranslation(newTranslation);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.translation(), newTranslation);

        // WHEN
        const QQuaternion newRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 45.0f);
        joint.setRotation(newRotation);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.rotation(), newRotation);

        // WHEN
        const QVector3D newScale = QVector3D(1.5f, 2.5f, 3.5f);
        joint.setScale(newScale);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.scale(), newScale);

        // WHEN
        QMatrix4x4 newInverseBind;
        newInverseBind.scale(5.4f);
        joint.setInverseBindMatrix(newInverseBind);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.inverseBindMatrix(), newInverseBind);

        // WHEN
        QList<QJoint *> childJoints;
        for (int i = 0; i < 10; ++i) {
            const auto childJoint = new QJoint();
            childJoints.push_back(childJoint);
            joint.addChildJoint(childJoint);
        }
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        for (int i = 0; i < childJoints.size(); ++i)
            QCOMPARE(backendJoint.childJointIds()[i], childJoints[i]->id());

        for (int i = 0; i < 10; ++i) {
            // WHEN
            const auto childJoint = childJoints.takeLast();
            joint.removeChildJoint(childJoint);
            backendJoint.syncFromFrontEnd(&joint, false);

            // THEN
            for (int i = 0; i < childJoints.size(); ++i)
                QCOMPARE(backendJoint.childJointIds()[i], childJoints[i]->id());
        }
    }

    void checkDirectPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Joint backendJoint;
        backendJoint.setRenderer(&renderer);
        backendJoint.setJointManager(nodeManagers.jointManager());
        backendJoint.setSkeletonManager(nodeManagers.skeletonManager());

        QJoint joint;
        simulateInitializationSync(&joint, &backendJoint);

        // WHEN
        joint.setEnabled(false);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.isEnabled(), false);

        // WHEN
        const QVector3D newTranslation = QVector3D(1.0f, 2.0f, 3.0f);
        joint.setTranslation(newTranslation);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.translation(), newTranslation);

        // WHEN
        const QQuaternion newRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 45.0f);
        joint.setRotation(newRotation);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.rotation(), newRotation);

        // WHEN
        const QVector3D newScale = QVector3D(1.5f, 2.5f, 3.5f);
        joint.setScale(newScale);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.scale(), newScale);

        // WHEN
        QMatrix4x4 newInverseBind;
        newInverseBind.scale(5.4f);
        joint.setInverseBindMatrix(newInverseBind);
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        QCOMPARE(backendJoint.inverseBindMatrix(), newInverseBind);

        // WHEN
        QList<QJoint *> childJoints;
        for (int i = 0; i < 10; ++i) {
            const auto childJoint = new QJoint();
            joint.addChildJoint(childJoint);
            childJoints.push_back(childJoint);
        }
        backendJoint.syncFromFrontEnd(&joint, false);

        // THEN
        for (int i = 0; i < childJoints.size(); ++i) {
            QCOMPARE(backendJoint.childJointIds()[i], childJoints[i]->id());
        }

        for (int i = 0; i < 10; ++i) {
            // WHEN
            const auto childJoint = childJoints.takeLast();

            joint.removeChildJoint(childJoint);
            backendJoint.syncFromFrontEnd(&joint, false);

            // THEN
            for (int j = 0; j < childJoints.size(); ++j)
                QCOMPARE(backendJoint.childJointIds()[j], childJoints[j]->id());
        }
    }
};

QTEST_APPLESS_MAIN(tst_Joint)

#include "tst_joint.moc"
