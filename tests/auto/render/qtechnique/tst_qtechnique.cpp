// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/private/qgraphicsapifilter_p.h>
#include <Qt3DRender/private/qtechnique_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QTechnique : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QTechnique technique;

        // THEN
        QCOMPARE(technique.graphicsApiFilter()->profile(), Qt3DRender::QGraphicsApiFilter::NoProfile);
        QCOMPARE(technique.graphicsApiFilter()->majorVersion(), 0);
        QCOMPARE(technique.graphicsApiFilter()->minorVersion(), 0);
        QVERIFY(technique.renderPasses().empty());
        QVERIFY(technique.parameters().empty());
        QVERIFY(technique.filterKeys().empty());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QTechnique technique;

        {
            // WHEN
            Qt3DRender::QRenderPass pass;

            technique.addRenderPass(&pass);

            // THEN
            QCOMPARE(technique.renderPasses().size(), 1);

            // WHEN
            technique.addRenderPass(&pass);

            // THEN
            QCOMPARE(technique.renderPasses().size(), 1);

            // WHEN
            technique.removeRenderPass(&pass);

            // THEN
            QCOMPARE(technique.renderPasses().size(), 0);
        }
        {
            // WHEN
            Qt3DRender::QParameter parameter;

            technique.addParameter(&parameter);

            // THEN
            QCOMPARE(technique.parameters().size(), 1);

            // WHEN
            technique.addParameter(&parameter);

            // THEN
            QCOMPARE(technique.parameters().size(), 1);

            // WHEN
            technique.removeParameter(&parameter);

            // THEN
            QCOMPARE(technique.parameters().size(), 0);
        }
        {
            // WHEN
            Qt3DRender::QFilterKey filterKey;

            technique.addFilterKey(&filterKey);

            // THEN
            QCOMPARE(technique.filterKeys().size(), 1);

            // WHEN
            technique.addFilterKey(&filterKey);

            // THEN
            QCOMPARE(technique.filterKeys().size(), 1);

            // WHEN
            technique.removeFilterKey(&filterKey);

            // THEN
            QCOMPARE(technique.filterKeys().size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(technique.graphicsApiFilter(), SIGNAL(graphicsApiFilterChanged()));
            technique.graphicsApiFilter()->setMajorVersion(3);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(technique.graphicsApiFilter()->majorVersion(), 3);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            technique.graphicsApiFilter()->setMajorVersion(3);

            // THEN
            QCOMPARE(technique.graphicsApiFilter()->majorVersion(), 3);
            QCOMPARE(spy.size(), 0);

            // WHEN
            technique.graphicsApiFilter()->setMinorVersion(2);

            // THEN
            QCOMPARE(technique.graphicsApiFilter()->minorVersion(), 2);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            technique.graphicsApiFilter()->setMinorVersion(2);

            // THEN
            QCOMPARE(technique.graphicsApiFilter()->minorVersion(), 2);
            QCOMPARE(spy.size(), 0);

            // WHEN
            technique.graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

            // THEN
            QCOMPARE(technique.graphicsApiFilter()->profile(), Qt3DRender::QGraphicsApiFilter::CoreProfile);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            technique.graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

            // THEN
            QCOMPARE(technique.graphicsApiFilter()->profile(), Qt3DRender::QGraphicsApiFilter::CoreProfile);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkRenderPassBookkeeping()
    {
        // GIVEN
        Qt3DRender::QTechnique technique;

        {
            // WHEN
            Qt3DRender::QRenderPass pass;
            technique.addRenderPass(&pass);

            // THEN
            QCOMPARE(technique.renderPasses().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(technique.renderPasses().size(), 0);
    }

    void checkParameterBookkeeping()
    {
        // GIVEN
        Qt3DRender::QTechnique technique;

        {
            // WHEN
            Qt3DRender::QParameter parameter;
            technique.addParameter(&parameter);

            // THEN
            QCOMPARE(technique.parameters().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(technique.parameters().size(), 0);
    }

    void checkFilterKeyBookkeeping()
    {
        // GIVEN
        Qt3DRender::QTechnique technique;

        {
            // WHEN
            Qt3DRender::QFilterKey filterKey;
            technique.addFilterKey(&filterKey);

            // THEN
            QCOMPARE(technique.filterKeys().size(), 1);
        }

        // THEN -> should not crash
        QCOMPARE(technique.filterKeys().size(), 0);
    }

    void checkRenderPassUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTechnique technique;
        Qt3DRender::QRenderPass pass;
        arbiter.setArbiterOnNode(&technique);

        {
            // WHEN
            technique.addRenderPass(&pass);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }
        {
            // WHEN
            technique.removeRenderPass(&pass);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));
        }
    }

    void checkParameterUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTechnique technique;
        Qt3DRender::QParameter parameter;
        arbiter.setArbiterOnNode(&technique);

        {
            // WHEN
            technique.addParameter(&parameter);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }

        {
            // WHEN
            technique.removeParameter(&parameter);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }
    }

    void checkFilterKeyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTechnique technique;
        Qt3DRender::QFilterKey filterKey;
        arbiter.setArbiterOnNode(&technique);

        {
            // WHEN
            technique.addFilterKey(&filterKey);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }

        {
            // WHEN
            technique.removeFilterKey(&filterKey);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }
    }

    void checkGraphicsAPIFilterUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTechnique technique;
        arbiter.setArbiterOnNode(&technique);

        {
            // WHEN
            technique.graphicsApiFilter()->setMajorVersion(4);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }
        {
            // WHEN
            technique.graphicsApiFilter()->setMinorVersion(5);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }
        {
            // WHEN
            technique.graphicsApiFilter()->setVendor(QStringLiteral("AMD"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }
        {
            // WHEN
            technique.graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CompatibilityProfile);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QVERIFY(arbiter.dirtyNodes().contains(&technique));

            arbiter.clear();
        }
    }
};

QTEST_MAIN(tst_QTechnique)

#include "tst_qtechnique.moc"
