// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/private/qbuffer_p.h>

#include "testarbiter.h"

class tst_QBuffer: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DCore::QBuffer> buffer(new Qt3DCore::QBuffer);
        arbiter.setArbiterOnNode(buffer.data());

        // WHEN
        buffer->setUsage(Qt3DCore::QBuffer::DynamicCopy);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), buffer.data());

        arbiter.clear();

        // WHEN
        buffer->setData(QByteArrayLiteral("Z28"));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), buffer.data());

        arbiter.clear();

        // WHEN
        buffer->updateData(1, QByteArrayLiteral("L1"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), buffer.data());
    }

    void checkAccumulatesBufferUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DCore::QBuffer> buffer(new Qt3DCore::QBuffer);
        arbiter.setArbiterOnNode(buffer.data());

        QByteArray initData(6, '\0');
        buffer->setData(initData);

        // THEN
        QCOMPARE(buffer->data(), initData);
        QCOMPARE(buffer->property(Qt3DCore::QBufferPrivate::UpdateDataPropertyName), QVariant());

        // WHEN
        const QByteArray uData("012");
        buffer->updateData(0, uData);

        // THEN
        {
            const QVariant v = buffer->property(Qt3DCore::QBufferPrivate::UpdateDataPropertyName);
            QVERIFY(v.isValid());
            const QVariantList l = v.toList();
            QCOMPARE(l.size(), 1);
            const Qt3DCore::QBufferUpdate update = l.first().value<Qt3DCore::QBufferUpdate>();
            QCOMPARE(update.offset, 0);
            QCOMPARE(update.data, uData);
            QCOMPARE(buffer->data().mid(0, 3), uData);
        }

        // WHEN
        const QByteArray uData2("345");
        buffer->updateData(3, uData2);

        // THEN
        {
            const QVariant v = buffer->property(Qt3DCore::QBufferPrivate::UpdateDataPropertyName);
            QVERIFY(v.isValid());
            const QVariantList l = v.toList();
            QCOMPARE(l.size(), 2);
            const Qt3DCore::QBufferUpdate update = l.last().value<Qt3DCore::QBufferUpdate>();
            QCOMPARE(update.offset, 3);
            QCOMPARE(update.data, uData2);
            QCOMPARE(buffer->data(), QByteArray("012345"));
        }
    }
};

QTEST_MAIN(tst_QBuffer)

#include "tst_qbuffer.moc"
