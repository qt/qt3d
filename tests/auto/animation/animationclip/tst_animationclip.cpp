/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DAnimation/private/animationclip_p.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testpostmanarbiter.h>

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
