// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qskeleton.h>
#include <Qt3DCore/private/qskeleton_p.h>
#include <Qt3DCore/qjoint.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <QSignalSpy>
#include <testarbiter.h>

using namespace Qt3DCore;

class tst_QSkeleton: public QSkeleton
{
    Q_OBJECT
public:
    tst_QSkeleton()
    {
        qRegisterMetaType<Qt3DCore::QJoint*>();
    }

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        QSkeleton skeleton;

        // THEN
        QCOMPARE(skeleton.jointCount(), 0);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<QSkeleton> skeleton(new QSkeleton());
        arbiter.setArbiterOnNode(skeleton.data());

        // WHEN
        QJoint *joint = new QJoint(skeleton.data());
        arbiter.clear();

        skeleton->setRootJoint(joint);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), skeleton.data());

        arbiter.clear();

        // WHEN
        skeleton->setRootJoint(nullptr);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), skeleton.data());

        arbiter.clear();
    }

    void checkRootJointBookkeeping()
    {
        // GIVEN
        QScopedPointer<QSkeleton> skeleton(new QSkeleton);
        {
            // WHEN
            QJoint joint;
            skeleton->setRootJoint(&joint);

            // THEN
            QCOMPARE(joint.parent(), skeleton.data());
            QCOMPARE(skeleton->rootJoint(), &joint);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(skeleton->rootJoint() == nullptr);

        {
            // WHEN
            QSkeleton someOtherSkeleton;
            QScopedPointer<QJoint> joint(new QJoint(&someOtherSkeleton));
            skeleton->setRootJoint(joint.data());

            // THEN
            QCOMPARE(joint->parent(), &someOtherSkeleton);
            QCOMPARE(skeleton->rootJoint(), joint.data());

            // WHEN
            skeleton.reset();
            joint.reset();

            // THEN Should not crash when the joint is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QSkeleton)

#include "tst_qskeleton.moc"
