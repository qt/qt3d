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
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testpostmanarbiter.h>

#include <QtTest/QTest>
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DAnimation/qlerpclipblend.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/private/qblendedclipanimator_p.h>
#include <Qt3DAnimation/private/blendedclipanimator_p.h>
#include "qbackendnodetester.h"

class tst_BlendedClipAnimator : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DAnimation::Animation::BlendedClipAnimator backendBlendedClipAnimator;

        // THEN
        QCOMPARE(backendBlendedClipAnimator.isEnabled(), false);
        QVERIFY(backendBlendedClipAnimator.peerId().isNull());
        QCOMPARE(backendBlendedClipAnimator.blendTreeRootId(), Qt3DCore::QNodeId());
        QCOMPARE(backendBlendedClipAnimator.mapperId(), Qt3DCore::QNodeId());
        QCOMPARE(backendBlendedClipAnimator.isRunning(), false);
        QCOMPARE(backendBlendedClipAnimator.lastLocalTime(), 0.0);
        QCOMPARE(backendBlendedClipAnimator.nsSincePreviousFrame(0), 0);
        QCOMPARE(backendBlendedClipAnimator.currentLoop(), 0);
        QCOMPARE(backendBlendedClipAnimator.loops(), 1);

    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DAnimation::Animation::BlendedClipAnimator backendBlendedClipAnimator;
        Qt3DAnimation::Animation::Handler handler;
        backendBlendedClipAnimator.setHandler(&handler);

        // WHEN
        backendBlendedClipAnimator.setEnabled(true);
        backendBlendedClipAnimator.setBlendTreeRootId(Qt3DCore::QNodeId::createId());
        backendBlendedClipAnimator.setMapperId(Qt3DCore::QNodeId::createId());
        backendBlendedClipAnimator.setRunning(true);
        backendBlendedClipAnimator.setStartTime(28);
        backendBlendedClipAnimator.setLastLocalTime(0.28);
        backendBlendedClipAnimator.cleanup();

        // THEN
        QCOMPARE(backendBlendedClipAnimator.isEnabled(), false);
        QCOMPARE(backendBlendedClipAnimator.blendTreeRootId(), Qt3DCore::QNodeId());
        QCOMPARE(backendBlendedClipAnimator.mapperId(), Qt3DCore::QNodeId());
        QCOMPARE(backendBlendedClipAnimator.isRunning(), false);
        QCOMPARE(backendBlendedClipAnimator.lastLocalTime(), 0.0);
        QCOMPARE(backendBlendedClipAnimator.nsSincePreviousFrame(0), 0);
        QCOMPARE(backendBlendedClipAnimator.currentLoop(), 0);
        QCOMPARE(backendBlendedClipAnimator.loops(), 1);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;
        Qt3DAnimation::QChannelMapper mapper;
        Qt3DAnimation::QLerpClipBlend blendTree;
        blendedClipAnimator.setRunning(true);
        blendedClipAnimator.setBlendTree(&blendTree);
        blendedClipAnimator.setChannelMapper(&mapper);
        blendedClipAnimator.setLoopCount(10);

        {
            // WHEN
            Qt3DAnimation::Animation::BlendedClipAnimator backendBlendedClipAnimator;
            Qt3DAnimation::Animation::Handler handler;
            backendBlendedClipAnimator.setHandler(&handler);

            simulateInitializationSync(&blendedClipAnimator, &backendBlendedClipAnimator);

            // THEN
            QCOMPARE(backendBlendedClipAnimator.isEnabled(), true);
            QCOMPARE(backendBlendedClipAnimator.peerId(), blendedClipAnimator.id());
            QCOMPARE(backendBlendedClipAnimator.blendTreeRootId(), blendTree.id());
            QCOMPARE(backendBlendedClipAnimator.mapperId(), mapper.id());
            QCOMPARE(backendBlendedClipAnimator.isRunning(), true);
            QCOMPARE(backendBlendedClipAnimator.loops(), 10);
        }
        {
            // WHEN
            Qt3DAnimation::Animation::BlendedClipAnimator backendBlendedClipAnimator;
            Qt3DAnimation::Animation::Handler handler;
            backendBlendedClipAnimator.setHandler(&handler);

            blendedClipAnimator.setEnabled(false);
            simulateInitializationSync(&blendedClipAnimator, &backendBlendedClipAnimator);

            // THEN
            QCOMPARE(backendBlendedClipAnimator.peerId(), blendedClipAnimator.id());
            QCOMPARE(backendBlendedClipAnimator.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;
        Qt3DAnimation::Animation::BlendedClipAnimator backendBlendedClipAnimator;
        Qt3DAnimation::Animation::Handler handler;
        backendBlendedClipAnimator.setHandler(&handler);
        simulateInitializationSync(&blendedClipAnimator, &backendBlendedClipAnimator);

        {
            // WHEN
            const bool newValue = false;
            blendedClipAnimator.setEnabled(newValue);
            backendBlendedClipAnimator.syncFromFrontEnd(&blendedClipAnimator, false);

            // THEN
            QCOMPARE(backendBlendedClipAnimator.isEnabled(), newValue);
        }
        {
            // WHEN
            auto blendTree = new Qt3DAnimation::QAdditiveClipBlend();
            blendedClipAnimator.setBlendTree(blendTree);
            backendBlendedClipAnimator.syncFromFrontEnd(&blendedClipAnimator, false);

            // THEN
            QCOMPARE(backendBlendedClipAnimator.blendTreeRootId(), blendTree->id());
        }
        {
            // WHEN
            auto channelMapper = new Qt3DAnimation::QChannelMapper();
            blendedClipAnimator.setChannelMapper(channelMapper);
            backendBlendedClipAnimator.syncFromFrontEnd(&blendedClipAnimator, false);

            // THEN
            QCOMPARE(backendBlendedClipAnimator.mapperId(), channelMapper->id());
        }
        {
            // WHEN
            const bool newValue = true;
            blendedClipAnimator.setRunning(newValue);
            backendBlendedClipAnimator.syncFromFrontEnd(&blendedClipAnimator, false);

            // THEN
            QCOMPARE(backendBlendedClipAnimator.isRunning(), newValue);
        }
        {
            // WHEN
            const int newValue = 883;
            blendedClipAnimator.setLoopCount(newValue);
            backendBlendedClipAnimator.syncFromFrontEnd(&blendedClipAnimator, false);

            // THEN
            QCOMPARE(backendBlendedClipAnimator.loops(), newValue);
        }
    }

};

QTEST_APPLESS_MAIN(tst_BlendedClipAnimator)

#include "tst_blendedclipanimator.moc"
