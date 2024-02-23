// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/private/qrenderpassfilter_p.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qfilterkey.h>

#include "testarbiter.h"

class tst_QRenderPassFilter: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QRenderPassFilter> defaultRenderPassFilter(new Qt3DRender::QRenderPassFilter);

        QCOMPARE(defaultRenderPassFilter->matchAny().size(), 0);
        QCOMPARE(defaultRenderPassFilter->parameters().size(), 0);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QRenderPassFilter> renderPassFilter(new Qt3DRender::QRenderPassFilter());
        arbiter.setArbiterOnNode(renderPassFilter.data());

        // WHEN
        Qt3DRender::QParameter *param1 = new Qt3DRender::QParameter();
        renderPassFilter->addParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(renderPassFilter.data()));

        arbiter.clear();

        // WHEN
        renderPassFilter->addParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        renderPassFilter->removeParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(renderPassFilter.data()));

        arbiter.clear();

        // WHEN
        Qt3DRender::QFilterKey *filterKey1 = new Qt3DRender::QFilterKey();
        renderPassFilter->addMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(renderPassFilter.data()));

        arbiter.clear();

        // WHEN
        renderPassFilter->addMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        renderPassFilter->removeMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(renderPassFilter.data()));

        arbiter.clear();
    }

    void checkParameterBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRenderPassFilter> passFilter(new Qt3DRender::QRenderPassFilter);
        {
            // WHEN
            Qt3DRender::QParameter param;
            passFilter->addParameter(&param);

            // THEN
            QCOMPARE(param.parent(), passFilter.data());
            QCOMPARE(passFilter->parameters().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(passFilter->parameters().empty());

        {
            // WHEN
            Qt3DRender::QRenderPassFilter someOtherPassFilter;
            QScopedPointer<Qt3DRender::QParameter> param(new Qt3DRender::QParameter(&someOtherPassFilter));
            passFilter->addParameter(param.data());

            // THEN
            QCOMPARE(param->parent(), &someOtherPassFilter);
            QCOMPARE(passFilter->parameters().size(), 1);

            // WHEN
            passFilter.reset();
            param.reset();

            // THEN Should not crash when the parameter is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkFilterKeyBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRenderPassFilter> passFilter(new Qt3DRender::QRenderPassFilter);
        {
            // WHEN
            Qt3DRender::QFilterKey filterKey;
            passFilter->addMatch(&filterKey);

            // THEN
            QCOMPARE(filterKey.parent(), passFilter.data());
            QCOMPARE(passFilter->matchAny().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(passFilter->matchAny().empty());

        {
            // WHEN
            Qt3DRender::QRenderPassFilter someOtherPassFilter;
            QScopedPointer<Qt3DRender::QFilterKey> filterKey(new Qt3DRender::QFilterKey(&someOtherPassFilter));
            passFilter->addMatch(filterKey.data());

            // THEN
            QCOMPARE(filterKey->parent(), &someOtherPassFilter);
            QCOMPARE(passFilter->matchAny().size(), 1);

            // WHEN
            passFilter.reset();
            filterKey.reset();

            // THEN Should not crash when the parameter is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QRenderPassFilter)

#include "tst_qrenderpassfilter.moc"
