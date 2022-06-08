// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QtTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qnode_p.h>

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/filterlayerentityjob_p.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class TestAspect : public Qt3DRender::QRenderAspect
{
public:
    TestAspect(Qt3DCore::QNode *root)
        : Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Manual)
        , m_jobManager(new Qt3DCore::QAspectJobManager())
    {
        Qt3DCore::QAbstractAspectPrivate::get(this)->m_jobManager = m_jobManager.data();
        QRenderAspect::onRegistered();

        QList<Qt3DCore::NodeTreeChange> nodes;
        Qt3DCore::QNodeVisitor v;
        v.traverse(root, [&nodes](Qt3DCore::QNode *node) {
            Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(node);
            d->m_typeInfo = const_cast<QMetaObject*>(Qt3DCore::QNodePrivate::findStaticMetaObject(node->metaObject()));
            d->m_hasBackendNode = true;
            nodes.push_back({
                node->id(),
                Qt3DCore::QNodePrivate::get(node)->m_typeInfo,
                Qt3DCore::NodeTreeChange::Added,
                node
            });
        });

        for (const auto &node: nodes)
            d_func()->createBackendNode(node);
    }

    ~TestAspect()
    {
        QRenderAspect::onUnregistered();
    }

    Qt3DRender::Render::NodeManagers *nodeManagers() const
    {
        return d_func()->m_renderer->nodeManagers();
    }

    void onRegistered() { QRenderAspect::onRegistered(); }
    void onUnregistered() { QRenderAspect::onUnregistered(); }

private:
    QScopedPointer<Qt3DCore::QAspectJobManager> m_jobManager;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

namespace {

Qt3DCore::QEntity *buildTestScene(int layersCount,
                                  int entityCount,
                                  QList<Qt3DCore::QNodeId> &layerFilterIds,
                                  bool alwaysEnabled = true)
{
    Qt3DCore::QEntity *root = new Qt3DCore::QEntity();
    Qt3DRender::QLayerFilter *layerFilter = new Qt3DRender::QLayerFilter(root);
    layerFilterIds.push_back(layerFilter->id());

    QList<Qt3DRender::QLayer *> layers;
    layers.reserve(layersCount);

    for (int i = 0; i < layersCount; ++i) {
        Qt3DRender::QLayer *layer = new Qt3DRender::QLayer(root);
        layers.push_back(layer);
        layerFilter->addLayer(layer);
    }

    for (int i = 0; i < entityCount; ++i) {
        Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(root);

        if (layersCount > 0)
            entity->addComponent(layers.at(QRandomGenerator::global()->bounded(layersCount)));

        if (!alwaysEnabled && i % 128 == 0)
            entity->setEnabled(false);
    }

    return root;
}

} // anonymous

class tst_BenchLayerFiltering : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void filterEntities_data()
    {
        QTest::addColumn<Qt3DCore::QEntity *>("entitySubtree");
        QTest::addColumn<Qt3DCore::QNodeIdVector>("layerFilterIds");


        {
            Qt3DCore::QNodeIdVector layerFilterIds;
            Qt3DCore::QEntity *rootEntity = buildTestScene(0, 5000, layerFilterIds);

            QTest::newRow("Filter-NoLayerFilterAllEnabled") << rootEntity
                                                            << layerFilterIds;
        }

        {
            Qt3DCore::QNodeIdVector layerFilterIds;
            Qt3DCore::QEntity *rootEntity = buildTestScene(0, 5000, layerFilterIds, false);
            QTest::newRow("Filter-NoLayerFilterSomeDisabled") << rootEntity
                                                              << layerFilterIds;
        }

        {
            Qt3DCore::QNodeIdVector layerFilterIds;
            Qt3DCore::QEntity *rootEntity = buildTestScene(10, 5000, layerFilterIds);

            QTest::newRow("FilterLayerFilterAllEnabled") << rootEntity
                                                         << layerFilterIds;
        }

        {
            Qt3DCore::QNodeIdVector layerFilterIds;
            Qt3DCore::QEntity *rootEntity = buildTestScene(10, 5000, layerFilterIds, false);

            QTest::newRow("FilterLayerFilterSomeDisabled") << rootEntity
                                                         << layerFilterIds;
        }

    }

    void filterEntities()
    {
        QFETCH(Qt3DCore::QEntity *, entitySubtree);
        QFETCH(Qt3DCore::QNodeIdVector, layerFilterIds);

        // GIVEN
        QScopedPointer<Qt3DRender::TestAspect> aspect(new Qt3DRender::TestAspect(entitySubtree));

        // WHEN
        Qt3DRender::Render::FilterLayerEntityJob filterJob;
        filterJob.setLayerFilters(layerFilterIds);
        filterJob.setManager(aspect->nodeManagers());

        QBENCHMARK {
            filterJob.run();
        }
    }
};

QTEST_MAIN(tst_BenchLayerFiltering)

#include "tst_bench_layerfiltering.moc"
