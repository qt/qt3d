// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qsortpolicy.h>
#include <Qt3DRender/private/qsortpolicy_p.h>

#include "testarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QSortPolicy: public Qt3DCore::QNode
{
    Q_OBJECT

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QSortPolicy> defaultsortPolicy(new Qt3DRender::QSortPolicy);

        QVERIFY(defaultsortPolicy->sortTypes().isEmpty());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QSortPolicy> sortPolicy(new Qt3DRender::QSortPolicy());
        arbiter.setArbiterOnNode(sortPolicy.data());

        // WHEN
        const QList<Qt3DRender::QSortPolicy::SortType> sortTypes = { Qt3DRender::QSortPolicy::BackToFront,
                                                                     Qt3DRender::QSortPolicy::Material,
                                                                     Qt3DRender::QSortPolicy::Material,
                                                                     Qt3DRender::QSortPolicy::FrontToBack };
        auto sortTypesInt = QList<int>();
        transformVector(sortTypes, sortTypesInt);
        sortPolicy->setSortTypes(sortTypes);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), sortPolicy.data());

        arbiter.clear();
    }
};

QTEST_MAIN(tst_QSortPolicy)

#include "tst_qsortpolicy.moc"
