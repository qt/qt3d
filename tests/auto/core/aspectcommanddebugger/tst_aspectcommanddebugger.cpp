// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/aspectcommanddebugger_p.h>

using namespace Qt3DCore::Debug;

class tst_AspectCommandDebugger : public QObject
{
    Q_OBJECT

private slots:
    void checkReadBufferInitialState()
    {
        // GIVEN
        AspectCommandDebugger::ReadBuffer buffer;

        // THEN
        QCOMPARE(buffer.startIdx, 0);
        QCOMPARE(buffer.endIdx, 0);
    }

    void checkReadBufferInsert()
    {
        // GIVEN
        AspectCommandDebugger::ReadBuffer buffer;

        // WHEN
        QByteArray fakeData(1024, '8');
        buffer.insert(fakeData);

        // THEN
        QCOMPARE(buffer.startIdx, 0);
        QCOMPARE(buffer.endIdx, 1024);
        QCOMPARE(buffer.buffer.size(), 1024);
        QCOMPARE(fakeData, QByteArray(buffer.buffer.constData(), 1024));

        // WHEN
        QByteArray hugeFakeData(1024 * 16, '.');
        buffer.insert(hugeFakeData);

        // THEN
        QCOMPARE(buffer.startIdx, 0);
        QCOMPARE(buffer.endIdx, 1024 + 16 * 1024);
        QCOMPARE(fakeData, QByteArray(buffer.buffer.constData(), 1024));
        QCOMPARE(hugeFakeData, QByteArray(buffer.buffer.constData() + 1024, 1024 * 16));
    }

    void checkBufferTrim()
    {
        // GIVEN
        AspectCommandDebugger::ReadBuffer buffer;
        QByteArray fakeData(1024, '9');
        QByteArray hugeFakeData(1024 * 16, '.');
        buffer.insert(fakeData);
        buffer.insert(hugeFakeData);

        // WHEN
        buffer.startIdx += 1024;
        buffer.trim();

        // THEN
        QCOMPARE(buffer.size(), 16 * 1024);
        QCOMPARE(hugeFakeData, QByteArray(buffer.buffer.constData(), 1024 * 16));
    }
};

QTEST_APPLESS_MAIN(tst_AspectCommandDebugger)

#include "tst_aspectcommanddebugger.moc"
