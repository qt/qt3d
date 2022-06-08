// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DAnimation/private/animationclip_p.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>

using namespace Qt3DAnimation::Animation;

class tst_AnimationClip : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        AnimationClip backendClip;
        Handler handler;
        backendClip.setHandler(&handler);
        Qt3DAnimation::QAnimationClipLoader clip;

        clip.setSource(QUrl::fromLocalFile("walk.qlip"));

        // WHEN
        simulateInitializationSync(&clip, &backendClip);

        // THEN
        QCOMPARE(backendClip.peerId(), clip.id());
        QCOMPARE(backendClip.isEnabled(), clip.isEnabled());
        QCOMPARE(backendClip.source(), clip.source());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        AnimationClip backendClip;
        Handler handler;
        backendClip.setHandler(&handler);

        // THEN
        QVERIFY(backendClip.peerId().isNull());
        QCOMPARE(backendClip.isEnabled(), false);
        QCOMPARE(backendClip.source(), QUrl());
        QCOMPARE(backendClip.duration(), 0.0f);
        QCOMPARE(backendClip.status(), Qt3DAnimation::QAnimationClipLoader::NotReady);

        // GIVEN
        Qt3DAnimation::QAnimationClipLoader clip;
        clip.setSource(QUrl::fromLocalFile("walk.qlip"));

        // WHEN
        simulateInitializationSync(&clip, &backendClip);
        backendClip.setSource(QUrl::fromLocalFile("run.qlip"));
        backendClip.cleanup();

        // THEN
        QCOMPARE(backendClip.source(), QUrl());
        QCOMPARE(backendClip.isEnabled(), false);
        QCOMPARE(backendClip.duration(), 0.0f);
        QCOMPARE(backendClip.status(), Qt3DAnimation::QAnimationClipLoader::NotReady);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QAnimationClipLoader clip;
        AnimationClip backendClip;
        Handler handler;
        backendClip.setHandler(&handler);
        simulateInitializationSync(&clip, &backendClip);

        // WHEN
        clip.setEnabled(false);
        backendClip.syncFromFrontEnd(&clip, false);

        // THEN
        QCOMPARE(backendClip.isEnabled(), false);

        // WHEN
        const QUrl newSource = QUrl::fromLocalFile("fallover.qlip");
        clip.setSource(newSource);
        backendClip.syncFromFrontEnd(&clip, false);

        // THEN
        QCOMPARE(backendClip.source(), newSource);
    }
};

QTEST_APPLESS_MAIN(tst_AnimationClip)

#include "tst_animationclip.moc"
