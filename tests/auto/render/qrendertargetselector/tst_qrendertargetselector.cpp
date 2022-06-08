// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/private/qrendertargetselector_p.h>
#include <Qt3DRender/qrendertarget.h>
#include "testarbiter.h"

class tst_QRenderTargetSelector: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QRenderTargetSelector> defaultTargetSelector(new Qt3DRender::QRenderTargetSelector);

        QVERIFY(defaultTargetSelector->target() == nullptr);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QRenderTargetSelector> renderTargetSelector(new Qt3DRender::QRenderTargetSelector());
        arbiter.setArbiterOnNode(renderTargetSelector.data());

        // WHEN
        Qt3DRender::QRenderTarget *target = new Qt3DRender::QRenderTarget();
        renderTargetSelector->setTarget(target);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), renderTargetSelector.data());

        arbiter.clear();

        // WHEN
        renderTargetSelector->setTarget(target);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        renderTargetSelector->setTarget(nullptr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), renderTargetSelector.data());

        arbiter.clear();
    }

    void checkRenderTargetBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRenderTargetSelector> targetSelector(new Qt3DRender::QRenderTargetSelector);
        {
            // WHEN
            Qt3DRender::QRenderTarget target;
            targetSelector->setTarget(&target);

            // THEN
            QCOMPARE(target.parent(), targetSelector.data());
            QCOMPARE(targetSelector->target(), &target);
        }
        // THEN (Should not crash and effect be unset)
        QVERIFY(targetSelector->target() == nullptr);

        {
            // WHEN
            Qt3DRender::QRenderTargetSelector someOtherTargetSelector;
            QScopedPointer<Qt3DRender::QRenderTarget> target(new Qt3DRender::QRenderTarget(&someOtherTargetSelector));
            targetSelector->setTarget(target.data());

            // THEN
            QCOMPARE(target->parent(), &someOtherTargetSelector);
            QCOMPARE(targetSelector->target(), target.data());

            // WHEN
            targetSelector.reset();
            target.reset();

            // THEN Should not crash when the target is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QRenderTargetSelector)

#include "tst_qrendertargetselector.moc"
