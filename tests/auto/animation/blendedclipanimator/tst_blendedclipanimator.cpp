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
#include <Qt3DAnimation/private/blendedclipanimator_p.h>
#include <Qt3DAnimation/qanimationclip.h>
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testpostmanarbiter.h>

class tst_BlendedClipAnimator: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DAnimation::Animation::BlendedClipAnimator backendAnimator;
        Qt3DAnimation::QBlendedClipAnimator animator;

        // WHEN
        simulateInitialization(&animator, &backendAnimator);

        // THEN
        QCOMPARE(backendAnimator.peerId(), animator.id());
        QCOMPARE(backendAnimator.isEnabled(), animator.isEnabled());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DAnimation::Animation::BlendedClipAnimator backendAnimator;

        // THEN
        QVERIFY(backendAnimator.peerId().isNull());
        QCOMPARE(backendAnimator.isEnabled(), false);

        // GIVEN
        Qt3DAnimation::QBlendedClipAnimator animator;

        // WHEN
        simulateInitialization(&animator, &backendAnimator);
        backendAnimator.cleanup();

        // THEN
        QCOMPARE(backendAnimator.isEnabled(), false);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::Animation::BlendedClipAnimator backendAnimator;
        Qt3DCore::QPropertyUpdatedChangePtr updateChange;

        // WHEN
        updateChange.reset(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        updateChange->setPropertyName("enabled");
        updateChange->setValue(true);
        backendAnimator.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAnimator.isEnabled(), true);
    }
};

QTEST_APPLESS_MAIN(tst_BlendedClipAnimator)

#include "tst_blendedclipanimator.moc"
