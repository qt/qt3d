// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/private/qrendertarget_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QRenderTarget : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QRenderTarget renderTarget;

        // THEN
        QVERIFY(renderTarget.outputs().empty());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QRenderTarget renderTarget;

        // WHEN
        Qt3DRender::QRenderTargetOutput output;

        renderTarget.addOutput(&output);

        // THEN
        QCOMPARE(renderTarget.outputs().size(), 1);

        // WHEN
        renderTarget.addOutput(&output);

        // THEN
        QCOMPARE(renderTarget.outputs().size(), 1);

        // WHEN
        renderTarget.removeOutput(&output);

        // THEN
        QCOMPARE(renderTarget.outputs().size(), 0);
    }

    void checkRenderTargetOutputBookkeeping()
    {
        // GIVEN
        Qt3DRender::QRenderTarget renderTarget;

        {
            // WHEN
            Qt3DRender::QRenderTargetOutput output;
            renderTarget.addOutput(&output);

            // THEN
            QCOMPARE(renderTarget.outputs().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(renderTarget.outputs().size(), 0);
    }

    void checkRenderTargetOutputUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderTarget renderTarget;
        Qt3DRender::QRenderTargetOutput renderTargetOutput;
        arbiter.setArbiterOnNode(&renderTarget);

        {
            // WHEN
            renderTarget.addOutput(&renderTargetOutput);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderTarget);

            arbiter.clear();
        }

        {
            // WHEN
            renderTarget.removeOutput(&renderTargetOutput);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderTarget);

            arbiter.clear();
        }
    }
};

QTEST_MAIN(tst_QRenderTarget)

#include "tst_qrendertarget.moc"
