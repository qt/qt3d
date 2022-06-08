// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/private/qclearbuffers_p.h>
#include <testarbiter.h>

class tst_QClearBuffers: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QClearBuffers> clearBuffer(new Qt3DRender::QClearBuffers());
        arbiter.setArbiterOnNode(clearBuffer.data());

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), clearBuffer.data());

        arbiter.clear();

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        clearBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), clearBuffer.data());

        arbiter.clear();
    }
};

QTEST_MAIN(tst_QClearBuffers)

#include "tst_qclearbuffers.moc"
