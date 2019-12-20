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
#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/private/qarmature_p.h>
#include <Qt3DCore/qskeleton.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <testarbiter.h>

using namespace Qt3DCore;

class tst_QArmature: public QObject
{
    Q_OBJECT
public:
    tst_QArmature()
    {
        qRegisterMetaType<Qt3DCore::QAbstractSkeleton*>();
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<QArmature> armature(new QArmature());
        arbiter.setArbiterOnNode(armature.data());

        // WHEN
        QSkeleton *skeleton = new QSkeleton(armature.data());
        armature->setSkeleton(skeleton);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), armature.data());

        arbiter.clear();

        // WHEN
        armature->setSkeleton(nullptr);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), armature.data());

        arbiter.clear();
    }

    void checkSkeletonBookkeeping()
    {
        // GIVEN
        QScopedPointer<QArmature> armature(new QArmature);
        {
            // WHEN
            QSkeleton skeleton;
            armature->setSkeleton(&skeleton);

            // THEN
            QCOMPARE(skeleton.parent(), armature.data());
            QCOMPARE(armature->skeleton(), &skeleton);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(armature->skeleton() == nullptr);

        {
            // WHEN
            QArmature someOtherArmature;
            QScopedPointer<QSkeleton> skeleton(new QSkeleton(&someOtherArmature));
            armature->setSkeleton(skeleton.data());

            // THEN
            QCOMPARE(skeleton->parent(), &someOtherArmature);
            QCOMPARE(armature->skeleton(), skeleton.data());

            // WHEN
            armature.reset();
            skeleton.reset();

            // THEN Should not crash when the joint is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QArmature)

#include "tst_qarmature.moc"
