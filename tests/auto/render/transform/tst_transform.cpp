/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qtransform_p.h>
#include <Qt3DRender/private/transform_p.h>
#include <Qt3DRender/private/updateworldtransformjob_p.h>
#include <private/qbackendnode_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"
#include "testpostmanarbiter.h"

class tst_Transform : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::Transform backendTransform;

        // THEN
        QCOMPARE(backendTransform.isEnabled(), false);
        QVERIFY(backendTransform.peerId().isNull());
        QCOMPARE(convertToQMatrix4x4(backendTransform.transformMatrix()), QMatrix4x4());
    }

    void checkCleanupState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Transform backendTransform;
        backendTransform.setRenderer(&renderer);

        // WHEN
        {
            Qt3DCore::QTransform transform;
            transform.setScale3D(QVector3D(1.0f, 2.0f, 3.0f));
            transform.setTranslation(QVector3D(-1.0, 5.0f, -2.0f));
            transform.setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0), 30.0f));
            backendTransform.setRenderer(&renderer);
            simulateInitializationSync(&transform, &backendTransform);
        }
        backendTransform.setEnabled(true);

        backendTransform.cleanup();

        // THEN
        QCOMPARE(backendTransform.isEnabled(), false);
        QCOMPARE(convertToQMatrix4x4(backendTransform.transformMatrix()), QMatrix4x4());
        QCOMPARE(backendTransform.rotation(), QQuaternion());
        QCOMPARE(backendTransform.scale(), QVector3D());
        QCOMPARE(backendTransform.translation(), QVector3D());
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DCore::QTransform transform;
        transform.setScale3D(QVector3D(1.0f, 2.0f, 3.0f));
        transform.setTranslation(QVector3D(-1.0, 5.0f, -2.0f));
        transform.setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0), 30.0f));

        {
            // WHEN
            Qt3DRender::Render::Transform backendTransform;
            backendTransform.setRenderer(&renderer);
            TestRenderer renderer;
            backendTransform.setRenderer(&renderer);
            simulateInitializationSync(&transform, &backendTransform);

            // THEN
            QCOMPARE(backendTransform.isEnabled(), true);
            QCOMPARE(backendTransform.peerId(), transform.id());
            QCOMPARE(convertToQMatrix4x4(backendTransform.transformMatrix()), transform.matrix());
            QCOMPARE(backendTransform.rotation(), transform.rotation());
            QCOMPARE(backendTransform.scale(), transform.scale3D());
            QCOMPARE(backendTransform.translation(), transform.translation());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TransformDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::Transform backendTransform;
            TestRenderer renderer;
            backendTransform.setRenderer(&renderer);
            transform.setEnabled(false);
            simulateInitializationSync(&transform, &backendTransform);

            // THEN
            QCOMPARE(backendTransform.peerId(), transform.id());
            QCOMPARE(backendTransform.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TransformDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DCore::QTransform frontendTranform;
        Qt3DRender::Render::Transform backendTransform;
        TestRenderer renderer;
        backendTransform.setRenderer(&renderer);
        backendTransform.syncFromFrontEnd(&frontendTranform, true);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        {
            // WHEN
            const bool newValue = false;
            frontendTranform.setEnabled(newValue);
            backendTransform.syncFromFrontEnd(&frontendTranform, false);

            // THEN
            QCOMPARE(backendTransform.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TransformDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const QQuaternion newValue = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), 45.0f);
            frontendTranform.setRotation(newValue);
            backendTransform.syncFromFrontEnd(&frontendTranform, false);

            // THEN
            QCOMPARE(backendTransform.rotation(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TransformDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const QVector3D newValue(454.0f, 355.0f, 0.0f);
            frontendTranform.setScale3D(newValue);
            backendTransform.syncFromFrontEnd(&frontendTranform, false);

            // THEN
            QCOMPARE(backendTransform.scale(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TransformDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const QVector3D newValue(383.0f, 0.0f, 427.0f);
            frontendTranform.setTranslation(newValue);
            backendTransform.syncFromFrontEnd(&frontendTranform, false);

            // THEN
            QCOMPARE(backendTransform.translation(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::TransformDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }
};

QTEST_MAIN(tst_Transform)

#include "tst_transform.moc"
