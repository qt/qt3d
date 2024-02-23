// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/private/armature_p.h>
#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qskeleton.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testarbiter.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

class tst_Armature: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Armature backendArmature;
        QArmature armature;
        auto skeleton = new QSkeleton;

        armature.setSkeleton(skeleton);

        // WHEN
        simulateInitializationSync(&armature, &backendArmature);

        // THEN
        QCOMPARE(backendArmature.peerId(), armature.id());
        QCOMPARE(backendArmature.isEnabled(), armature.isEnabled());
        QCOMPARE(backendArmature.skeletonId(), skeleton->id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Armature backendArmature;

        // THEN
        QVERIFY(backendArmature.peerId().isNull());
        QCOMPARE(backendArmature.isEnabled(), false);
        QCOMPARE(backendArmature.skeletonId(), Qt3DCore::QNodeId());

        // GIVEN
        QArmature armature;
        auto skeleton = new QSkeleton();
        armature.setSkeleton(skeleton);

        // WHEN
        simulateInitializationSync(&armature, &backendArmature);
        backendArmature.cleanup();

        // THEN
        QCOMPARE(backendArmature.skeletonId(), Qt3DCore::QNodeId());
        QCOMPARE(backendArmature.isEnabled(), false);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        QArmature armature;
        Armature backendArmature;
        simulateInitializationSync(&armature, &backendArmature);

        // WHEN
        armature.setEnabled(false);
        backendArmature.syncFromFrontEnd(&armature, false);

        // THEN
        QCOMPARE(backendArmature.isEnabled(), false);

        // WHEN
        auto newSkeleton = new QSkeleton();
        armature.setSkeleton(newSkeleton);
        backendArmature.syncFromFrontEnd(&armature, false);

        // THEN
        QCOMPARE(backendArmature.skeletonId(), newSkeleton->id());
    }
};

QTEST_APPLESS_MAIN(tst_Armature)

#include "tst_armature.moc"
