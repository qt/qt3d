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
#include <Qt3DRender/qdispatchcompute.h>
#include <Qt3DRender/private/qdispatchcompute_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QDispatchCompute : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QDispatchCompute dispatchCompute;

        // THEN
        QCOMPARE(dispatchCompute.workGroupX(), 1);
        QCOMPARE(dispatchCompute.workGroupY(), 1);
        QCOMPARE(dispatchCompute.workGroupZ(), 1);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QDispatchCompute dispatchCompute;

        {
            // WHEN
            QSignalSpy spy(&dispatchCompute, SIGNAL(workGroupXChanged()));
            const int newValue = 128;
            dispatchCompute.setWorkGroupX(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(dispatchCompute.workGroupX(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            dispatchCompute.setWorkGroupX(newValue);

            // THEN
            QCOMPARE(dispatchCompute.workGroupX(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&dispatchCompute, SIGNAL(workGroupYChanged()));
            const int newValue = 1024;
            dispatchCompute.setWorkGroupY(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(dispatchCompute.workGroupY(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            dispatchCompute.setWorkGroupY(newValue);

            // THEN
            QCOMPARE(dispatchCompute.workGroupY(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&dispatchCompute, SIGNAL(workGroupZChanged()));
            const int newValue = 32;
            dispatchCompute.setWorkGroupZ(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(dispatchCompute.workGroupZ(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            dispatchCompute.setWorkGroupZ(newValue);

            // THEN
            QCOMPARE(dispatchCompute.workGroupZ(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkWorkGroupXUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QDispatchCompute dispatchCompute;
        arbiter.setArbiterOnNode(&dispatchCompute);

        {
            // WHEN
            dispatchCompute.setWorkGroupX(883);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &dispatchCompute);

            arbiter.clear();
        }

        {
            // WHEN
            dispatchCompute.setWorkGroupX(883);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkWorkGroupYUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QDispatchCompute dispatchCompute;
        arbiter.setArbiterOnNode(&dispatchCompute);

        {
            // WHEN
            dispatchCompute.setWorkGroupY(1340);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &dispatchCompute);

            arbiter.clear();
        }

        {
            // WHEN
            dispatchCompute.setWorkGroupY(1340);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkWorkGroupZUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QDispatchCompute dispatchCompute;
        arbiter.setArbiterOnNode(&dispatchCompute);

        {
            // WHEN
            dispatchCompute.setWorkGroupZ(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &dispatchCompute);

            arbiter.clear();
        }

        {
            // WHEN
            dispatchCompute.setWorkGroupZ(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QDispatchCompute)

#include "tst_qdispatchcompute.moc"
