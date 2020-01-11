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

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qsetfence.h>
#include <Qt3DRender/private/qsetfence_p.h>
#include <Qt3DRender/private/setfence_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"
#include "testpostmanarbiter.h"

class tst_SetFence : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::SetFence backendSetFence;

        // THEN
        QCOMPARE(backendSetFence.isEnabled(), false);
        QVERIFY(backendSetFence.peerId().isNull());
        QCOMPARE(backendSetFence.nodeType(), Qt3DRender::Render::FrameGraphNode::SetFence);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QSetFence setFence;

        {
            // WHEN
            Qt3DRender::Render::SetFence backendSetFence;
            backendSetFence.setRenderer(&renderer);
            simulateInitializationSync(&setFence, &backendSetFence);

            // THEN
            QCOMPARE(backendSetFence.isEnabled(), true);
            QCOMPARE(backendSetFence.peerId(), setFence.id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::SetFence backendSetFence;
            setFence.setEnabled(false);
            backendSetFence.setRenderer(&renderer);
            simulateInitializationSync(&setFence, &backendSetFence);

            // THEN
            QCOMPARE(backendSetFence.peerId(), setFence.id());
            QCOMPARE(backendSetFence.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::SetFence backendSetFence;
        Qt3DRender::QSetFence setFence;
        TestRenderer renderer;
        backendSetFence.setRenderer(&renderer);
        simulateInitializationSync(&setFence, &backendSetFence);

        {
             // WHEN
             const bool newValue = false;
             setFence.setEnabled(newValue);
             backendSetFence.syncFromFrontEnd(&setFence, false);

             // THEN
            QCOMPARE(backendSetFence.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }

    void checkSetHandleType()
    {
        // GIVEN
        Qt3DRender::Render::SetFence backendSetFence;
        TestRenderer renderer;
        TestArbiter arbiter;

        Qt3DCore::QBackendNodePrivate::get(&backendSetFence)->setArbiter(&arbiter);
        backendSetFence.setRenderer(&renderer);

        // WHEN
        backendSetFence.setHandleType(Qt3DRender::QSetFence::OpenGLFenceId);

        // THEN
        Qt3DCore::QPropertyUpdatedChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(arbiter.events.count(), 1);
        QCOMPARE(change->propertyName(), "handleType");
        QCOMPARE(change->value().value<Qt3DRender::QSetFence::HandleType>(), Qt3DRender::QSetFence::OpenGLFenceId);

        arbiter.events.clear();
    }

    void checkSetHandle()
    {
        // GIVEN
        Qt3DRender::Render::SetFence backendSetFence;
        TestRenderer renderer;
        TestArbiter arbiter;

        Qt3DCore::QBackendNodePrivate::get(&backendSetFence)->setArbiter(&arbiter);
        backendSetFence.setRenderer(&renderer);

        // WHEN
        backendSetFence.setHandle(QVariant(984));

        // THEN
        Qt3DCore::QPropertyUpdatedChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(arbiter.events.count(), 1);
        QCOMPARE(change->propertyName(), "handle");
        QCOMPARE(change->value(), QVariant(984));

        arbiter.events.clear();
    }
};

QTEST_MAIN(tst_SetFence)

#include "tst_setfence.moc"
