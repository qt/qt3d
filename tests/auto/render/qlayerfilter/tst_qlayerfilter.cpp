/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
