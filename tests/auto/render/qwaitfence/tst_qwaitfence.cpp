// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qwaitfence.h>
#include <Qt3DRender/private/qwaitfence_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QWaitFence : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QWaitFence::HandleType>("HandleType");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QWaitFence waitFence;

        // THEN
        QCOMPARE(waitFence.handleType(), Qt3DRender::QWaitFence::NoHandle);
        QCOMPARE(waitFence.handle(), QVariant());
        QCOMPARE(waitFence.waitOnCPU(), false);
        QCOMPARE(waitFence.timeout(), quint64(-1));
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QWaitFence waitFence;

        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(handleTypeChanged(HandleType)));
            const Qt3DRender::QWaitFence::HandleType newValue = Qt3DRender::QWaitFence::OpenGLFenceId;
            waitFence.setHandleType(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.handleType(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            waitFence.setHandleType(newValue);

            // THEN
            QCOMPARE(waitFence.handleType(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(handleChanged(QVariant)));
            const QVariant newValue(883);
            waitFence.setHandle(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.handle(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            waitFence.setHandle(newValue);

            // THEN
            QCOMPARE(waitFence.handle(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(waitOnCPUChanged(bool)));
            const bool newValue = true;
            waitFence.setWaitOnCPU(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.waitOnCPU(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            waitFence.setWaitOnCPU(newValue);

            // THEN
            QCOMPARE(waitFence.waitOnCPU(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&waitFence, SIGNAL(timeoutChanged(quint64)));
            const quint64 newValue = 984;
            waitFence.setTimeout(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(waitFence.timeout(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            waitFence.setTimeout(newValue);

            // THEN
            QCOMPARE(waitFence.timeout(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkHandleTypeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setHandleType(Qt3DRender::QWaitFence::OpenGLFenceId);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &waitFence);

            arbiter.clear();
        }

        {
            // WHEN
            waitFence.setHandleType(Qt3DRender::QWaitFence::OpenGLFenceId);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkHandleUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setHandle(QVariant(883));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &waitFence);

            arbiter.clear();
        }

        {
            // WHEN
            waitFence.setHandle(QVariant(883));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkWaitOnCPUUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setWaitOnCPU(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &waitFence);

            arbiter.clear();
        }

        {
            // WHEN
            waitFence.setWaitOnCPU(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkTimeoutUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QWaitFence waitFence;
        arbiter.setArbiterOnNode(&waitFence);

        {
            // WHEN
            waitFence.setTimeout(quint64(600));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &waitFence);

            arbiter.clear();
        }

        {
            // WHEN
            waitFence.setTimeout(quint64(600));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }
};

QTEST_MAIN(tst_QWaitFence)

#include "tst_qwaitfence.moc"
