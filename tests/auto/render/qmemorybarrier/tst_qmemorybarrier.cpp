// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qmemorybarrier.h>
#include <Qt3DRender/private/qmemorybarrier_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QMemoryBarrier : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QMemoryBarrier::Operations>("QMemoryBarrier::Operations");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QMemoryBarrier memoryBarrier;

        // THEN
        QCOMPARE(memoryBarrier.waitOperations(), Qt3DRender::QMemoryBarrier::None);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QMemoryBarrier memoryBarrier;

        {
            // WHEN
            QSignalSpy spy(&memoryBarrier, SIGNAL(waitOperationsChanged(QMemoryBarrier::Operations)));
            const Qt3DRender::QMemoryBarrier::Operations newValue(Qt3DRender::QMemoryBarrier::ShaderStorage|Qt3DRender::QMemoryBarrier::VertexAttributeArray);
            memoryBarrier.setWaitOperations(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(memoryBarrier.waitOperations(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            memoryBarrier.setWaitOperations(newValue);

            // THEN
            QCOMPARE(memoryBarrier.waitOperations(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkTypesUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QMemoryBarrier memoryBarrier;
        arbiter.setArbiterOnNode(&memoryBarrier);

        {
            // WHEN
            memoryBarrier.setWaitOperations(Qt3DRender::QMemoryBarrier::ShaderStorage);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &memoryBarrier);

            arbiter.clear();
        }

        {
            // WHEN
            memoryBarrier.setWaitOperations(Qt3DRender::QMemoryBarrier::ShaderStorage);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QMemoryBarrier)

#include "tst_qmemorybarrier.moc"
