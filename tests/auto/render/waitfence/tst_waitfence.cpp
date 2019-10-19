/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/



#include <QtTest/QTest>
#include <Qt3DRender/qwaitfence.h>
#include <Qt3DRender/private/qwaitfence_p.h>
#include <Qt3DRender/private/waitfence_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_WaitFence : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::WaitFence backendWaitFence;

        // THEN
        QCOMPARE(backendWaitFence.isEnabled(), false);
        QVERIFY(backendWaitFence.peerId().isNull());
        QCOMPARE(backendWaitFence.nodeType(), Qt3DRender::Render::FrameGraphNode::WaitFence);
        QCOMPARE(backendWaitFence.data().handleType, Qt3DRender::QWaitFence::NoHandle);
        QCOMPARE(backendWaitFence.data().handle, QVariant());
        QCOMPARE(backendWaitFence.data().waitOnCPU, false);
        QCOMPARE(backendWaitFence.data().timeout, quint64(-1));
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QWaitFence waitFence;
        waitFence.setHandle(QVariant(883));
        waitFence.setWaitOnCPU(true);
        waitFence.setTimeout(8);
        waitFence.setHandleType(Qt3DRender::QWaitFence::OpenGLFenceId);

        {
            // WHEN
            Qt3DRender::Render::WaitFence backendWaitFence;
            backendWaitFence.setRenderer(&renderer);
            simulateInitializationSync(&waitFence, &backendWaitFence);

            // THEN
            QCOMPARE(backendWaitFence.isEnabled(), true);
            QCOMPARE(backendWaitFence.peerId(), waitFence.id());
            QCOMPARE(backendWaitFence.data().handleType, Qt3DRender::QWaitFence::OpenGLFenceId);
            QCOMPARE(backendWaitFence.data().handle, QVariant(883));
            QCOMPARE(backendWaitFence.data().waitOnCPU, true);
            QCOMPARE(backendWaitFence.data().timeout, quint64(8));
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::WaitFence backendWaitFence;
            waitFence.setEnabled(false);
            backendWaitFence.setRenderer(&renderer);
            simulateInitializationSync(&waitFence, &backendWaitFence);

            // THEN
            QCOMPARE(backendWaitFence.peerId(), waitFence.id());
            QCOMPARE(backendWaitFence.isEnabled(), false);
            QCOMPARE(backendWaitFence.data().handleType, Qt3DRender::QWaitFence::OpenGLFenceId);
            QCOMPARE(backendWaitFence.data().handle, QVariant(883));
            QCOMPARE(backendWaitFence.data().waitOnCPU, true);
            QCOMPARE(backendWaitFence.data().timeout, quint64(8));
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QWaitFence waitFence;
        Qt3DRender::Render::WaitFence backendWaitFence;
        TestRenderer renderer;
        backendWaitFence.setRenderer(&renderer);
        simulateInitializationSync(&waitFence, &backendWaitFence);

        {
            // WHEN
            const bool newValue = false;
            waitFence.setEnabled(newValue);
            backendWaitFence.syncFromFrontEnd(&waitFence, false);

            // THEN
            QCOMPARE(backendWaitFence.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const QVariant newValue(984);
            waitFence.setHandle(newValue);
            backendWaitFence.syncFromFrontEnd(&waitFence, false);

            // THEN
            QCOMPARE(backendWaitFence.data().handle, QVariant(984));
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const Qt3DRender::QWaitFence::HandleType newValue = Qt3DRender::QWaitFence::OpenGLFenceId;
            waitFence.setHandleType(newValue);
            backendWaitFence.syncFromFrontEnd(&waitFence, false);

            // THEN
            QCOMPARE(backendWaitFence.data().handleType, Qt3DRender::QWaitFence::OpenGLFenceId);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const bool newValue = true;
            waitFence.setWaitOnCPU(newValue);
            backendWaitFence.syncFromFrontEnd(&waitFence, false);

            // THEN
            QCOMPARE(backendWaitFence.data().waitOnCPU, true);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const quint64 newValue = 984;
            waitFence.setTimeout(newValue);
            backendWaitFence.syncFromFrontEnd(&waitFence, false);

            // THEN
            QCOMPARE(backendWaitFence.data().timeout, quint64(984));
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }
};

QTEST_MAIN(tst_WaitFence)

#include "tst_waitfence.moc"
