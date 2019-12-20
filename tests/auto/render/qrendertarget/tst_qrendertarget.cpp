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
