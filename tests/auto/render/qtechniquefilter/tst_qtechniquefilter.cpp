// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/private/qtechniquefilter_p.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qfilterkey.h>

#include "testarbiter.h"

class tst_QTechniqueFilter: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QTechniqueFilter> defaulttechniqueFilter(new Qt3DRender::QTechniqueFilter);

        QCOMPARE(defaulttechniqueFilter->matchAll().size(), 0);
        QCOMPARE(defaulttechniqueFilter->parameters().size(), 0);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QTechniqueFilter> techniqueFilter(new Qt3DRender::QTechniqueFilter());
        arbiter.setArbiterOnNode(techniqueFilter.data());

        // WHEN
        Qt3DRender::QParameter *param1 = new Qt3DRender::QParameter();
        techniqueFilter->addParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), techniqueFilter.data());
        arbiter.clear();


        // WHEN
        techniqueFilter->addParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        techniqueFilter->removeParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), techniqueFilter.data());
        arbiter.clear();

        // WHEN
        Qt3DRender::QFilterKey *filterKey1 = new Qt3DRender::QFilterKey();
        techniqueFilter->addMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), techniqueFilter.data());
        arbiter.clear();

        // WHEN
        techniqueFilter->addMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        techniqueFilter->removeMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), techniqueFilter.data());
        arbiter.clear();
    }

    void checkParameterBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QTechniqueFilter> techniqueFilter(new Qt3DRender::QTechniqueFilter);
        {
            // WHEN
            Qt3DRender::QParameter param;
            techniqueFilter->addParameter(&param);

            // THEN
            QCOMPARE(param.parent(), techniqueFilter.data());
            QCOMPARE(techniqueFilter->parameters().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(techniqueFilter->parameters().empty());

        {
            // WHEN
            Qt3DRender::QTechniqueFilter someOtherTechniqueFilter;
            QScopedPointer<Qt3DRender::QParameter> param(new Qt3DRender::QParameter(&someOtherTechniqueFilter));
            techniqueFilter->addParameter(param.data());

            // THEN
            QCOMPARE(param->parent(), &someOtherTechniqueFilter);
            QCOMPARE(techniqueFilter->parameters().size(), 1);

            // WHEN
            techniqueFilter.reset();
            param.reset();

            // THEN Should not crash when the parameter is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkFilterKeyBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QTechniqueFilter> techniqueFilter(new Qt3DRender::QTechniqueFilter);
        {
            // WHEN
            Qt3DRender::QFilterKey filterKey;
            techniqueFilter->addMatch(&filterKey);

            // THEN
            QCOMPARE(filterKey.parent(), techniqueFilter.data());
            QCOMPARE(techniqueFilter->matchAll().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(techniqueFilter->matchAll().empty());

        {
            // WHEN
            Qt3DRender::QTechniqueFilter someOtherTechniqueFilter;
            QScopedPointer<Qt3DRender::QFilterKey> filterKey(new Qt3DRender::QFilterKey(&someOtherTechniqueFilter));
            techniqueFilter->addMatch(filterKey.data());

            // THEN
            QCOMPARE(filterKey->parent(), &someOtherTechniqueFilter);
            QCOMPARE(techniqueFilter->matchAll().size(), 1);

            // WHEN
            techniqueFilter.reset();
            filterKey.reset();

            // THEN Should not crash when the filterKey is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QTechniqueFilter)

#include "tst_qtechniquefilter.moc"
