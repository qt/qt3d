// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/private/qlayerfilter_p.h>

#include "testarbiter.h"

class tst_QLayerFilter: public QObject
{
    Q_OBJECT
public:
    tst_QLayerFilter()
        : QObject()
    {
        qRegisterMetaType<Qt3DCore::QNode*>();
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QLayerFilter> layerFilter(new Qt3DRender::QLayerFilter());
        arbiter.setArbiterOnNode(layerFilter.data());

        // WHEN
        auto layer = new Qt3DRender::QLayer(layerFilter.data());
        QCoreApplication::processEvents();
        layerFilter->addLayer(layer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(layerFilter.data()));

        arbiter.clear();

        // WHEN
        layer = new Qt3DRender::QLayer(layerFilter.data());
        QCoreApplication::processEvents();
        layerFilter->addLayer(layer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(layerFilter.data()));

        arbiter.clear();

        // WHEN
        layer = layerFilter->layers().at(0);
        layerFilter->removeLayer(layer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(layerFilter.data()));

        arbiter.clear();
    }

    void checkLayerBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QLayerFilter> layerFilter(new Qt3DRender::QLayerFilter);
        {
            // WHEN
            Qt3DRender::QLayer layer;
            layerFilter->addLayer(&layer);

            // THEN
            QCOMPARE(layer.parent(), layerFilter.data());
            QCOMPARE(layerFilter->layers().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(layerFilter->layers().empty());

        {
            // WHEN
            Qt3DRender::QLayerFilter someOtherLayerFilter;
            QScopedPointer<Qt3DRender::QLayer> layer(new Qt3DRender::QLayer(&someOtherLayerFilter));
            layerFilter->addLayer(layer.data());

            // THEN
            QCOMPARE(layer->parent(), &someOtherLayerFilter);
            QCOMPARE(layerFilter->layers().size(), 1);

            // WHEN
            layerFilter.reset();
            layer.reset();

            // THEN Should not crash when the layer is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QLayerFilter)

#include "tst_qlayerfilter.moc"
