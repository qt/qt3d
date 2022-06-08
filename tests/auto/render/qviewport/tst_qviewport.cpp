// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/private/qviewport_p.h>

#include "testarbiter.h"

class tst_QViewport: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QViewport> viewport(new Qt3DRender::QViewport());
        arbiter.setArbiterOnNode(viewport.data());

        {
            // WHEN
            viewport->setNormalizedRect(QRectF(0.5, 0.5, 1.0, 1.0));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), viewport.data());

            arbiter.clear();

            // WHEN
            viewport->setNormalizedRect(QRectF(0.5, 0.5, 1.0, 1.0));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);

            // WHEN
            viewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), viewport.data());
        }

        arbiter.clear();

        {
            // WHEN
            viewport->setGamma(1.8f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), viewport.data());

            arbiter.clear();

            // WHEN
            viewport->setGamma(1.8f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);

            // WHEN
            viewport->setGamma(2.0f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), viewport.data());
        }
    }
};

QTEST_MAIN(tst_QViewport)

#include "tst_qviewport.moc"
