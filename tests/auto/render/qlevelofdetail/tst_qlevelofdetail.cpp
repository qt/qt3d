// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qlevelofdetail.h>
#include <Qt3DRender/private/qlevelofdetail_p.h>

#include "testarbiter.h"

class tst_QLevelOfDetail: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QLevelOfDetail> lod(new Qt3DRender::QLevelOfDetail());
        arbiter.setArbiterOnNode(lod.data());

        {
            // WHEN
            lod->setThresholdType(Qt3DRender::QLevelOfDetail::ProjectedScreenPixelSizeThreshold);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), lod.data());

            arbiter.clear();
        }

        {
            // WHEN
            const QList<qreal> thresholds = { 10., 20., 30. };
            lod->setThresholds(thresholds);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), lod.data());

            arbiter.clear();
        }
    }
};

QTEST_MAIN(tst_QLevelOfDetail)

#include "tst_qlevelofdetail.moc"
