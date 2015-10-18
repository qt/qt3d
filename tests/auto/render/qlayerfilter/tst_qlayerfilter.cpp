/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/qlayerfilter.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QLayerFilter: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QLayerFilter()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QLayerFilter *>("layerFilter");
        QTest::addColumn<QStringList>("layerNames");

        Qt3DRender::QLayerFilter *defaultConstructed = new Qt3DRender::QLayerFilter();
        QTest::newRow("defaultConstructed") << defaultConstructed << QStringList();

        Qt3DRender::QLayerFilter *singleLayer = new Qt3DRender::QLayerFilter();
        QStringList layer = QStringList() << QStringLiteral("myLayer");
        singleLayer->setLayers(layer);
        QTest::newRow("single layer") << singleLayer << layer;

        Qt3DRender::QLayerFilter *multiLayers = new Qt3DRender::QLayerFilter();
        QStringList layers = QStringList() << QStringLiteral("883") << QStringLiteral("350") << QStringLiteral("454");
        multiLayers->setLayers(layers);
        QTest::newRow("multi layers") << multiLayers << layers;
     }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QLayerFilter *, layerFilter);
        QFETCH(QStringList, layerNames);

        // THEN
        QCOMPARE(layerFilter->layers(), layerNames);

        // WHEN
        Qt3DRender::QLayerFilter *clone = static_cast<Qt3DRender::QLayerFilter *>(QNode::clone(layerFilter));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(layerFilter->id(), clone->id());
        QCOMPARE(layerFilter->layers(), clone->layers());

        delete layerFilter;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QLayerFilter> layerFilter(new Qt3DRender::QLayerFilter());
        TestArbiter arbiter(layerFilter.data());

        // WHEN
        QStringList layerNames1 = QStringList() << QStringLiteral("883") << QStringLiteral("350");
        layerFilter->setLayers(layerNames1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "layers");
        QCOMPARE(change->subjectId(), layerFilter->id());
        QCOMPARE(change->value().value<QStringList>(), layerNames1);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        layerFilter->setLayers(layerNames1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        QStringList layerNames2 = QStringList() << QStringLiteral("454");
        layerFilter->setLayers(layerNames2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "layers");
        QCOMPARE(change->subjectId(), layerFilter->id());
        QCOMPARE(change->value().value<QStringList>(), layerNames2);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        layerFilter->setLayers(QStringList());
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "layers");
        QCOMPARE(change->subjectId(), layerFilter->id());
        QCOMPARE(change->value().value<QStringList>(), QStringList());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);


        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QLayerFilter)

#include "tst_qlayerfilter.moc"
