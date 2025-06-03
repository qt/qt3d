// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/private/qrendersurfaceselector_p.h>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QRenderSurfaceSelector: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void shouldFindInstanceInEntityTree_data()
    {
        QTest::addColumn<QSharedPointer<Qt3DCore::QEntity>>("root");
        QTest::addColumn<Qt3DRender::QRenderSurfaceSelector*>("expected");

        auto root = QSharedPointer<Qt3DCore::QEntity>::create();
        auto settings = new Qt3DRender::QRenderSettings;
        root->addComponent(settings);
        auto selector = new Qt3DRender::QRenderSurfaceSelector;
        settings->setActiveFrameGraph(selector);
        QTest::newRow("simplest_tree") << root << selector;

        root = QSharedPointer<Qt3DCore::QEntity>::create();
        settings = new Qt3DRender::QRenderSettings;
        root->addComponent(settings);
        settings->setActiveFrameGraph(new Qt3DRender::QFrameGraphNode);
        selector = nullptr;
        QTest::newRow("no_selector") << root << selector;

        root = QSharedPointer<Qt3DCore::QEntity>::create();
        settings = new Qt3DRender::QRenderSettings;
        root->addComponent(settings);
        selector = nullptr;
        QTest::newRow("no_framegraph") << root << selector;

        root = QSharedPointer<Qt3DCore::QEntity>::create();
        selector = nullptr;
        QTest::newRow("no_rendersettings") << root << selector;

        root = QSharedPointer<Qt3DCore::QEntity>::create();
        auto entity = new Qt3DCore::QEntity(root.data());
        entity = new Qt3DCore::QEntity(entity);
        settings = new Qt3DRender::QRenderSettings;
        entity->addComponent(settings);
        selector = new Qt3DRender::QRenderSurfaceSelector;
        settings->setActiveFrameGraph(selector);
        QTest::newRow("in_subentity") << root << selector;

        root = QSharedPointer<Qt3DCore::QEntity>::create();
        entity = new Qt3DCore::QEntity(root.data());
        entity = new Qt3DCore::QEntity(entity);
        settings = new Qt3DRender::QRenderSettings;
        entity->addComponent(settings);
        auto node = new Qt3DRender::QFrameGraphNode;
        settings->setActiveFrameGraph(node);
        node = new Qt3DRender::QFrameGraphNode(node);
        selector = new Qt3DRender::QRenderSurfaceSelector(node);
        QTest::newRow("in_deeper_framegraph") << root << selector;
    }

    void shouldFindInstanceInEntityTree()
    {
        // GIVEN
        QFETCH(QSharedPointer<Qt3DCore::QEntity>, root);

        // WHEN
        auto selector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(root.data());

        // THEN
        QFETCH(Qt3DRender::QRenderSurfaceSelector*, expected);
        QCOMPARE(selector, expected);
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QRenderSurfaceSelector renderSurfaceSelector;

        // THEN
        QVERIFY(renderSurfaceSelector.surface() == nullptr);
        QCOMPARE(renderSurfaceSelector.externalRenderTargetSize(), QSize());
        QCOMPARE(renderSurfaceSelector.surfacePixelRatio(), 1.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QRenderSurfaceSelector renderSurfaceSelector;

        {
            // WHEN
            QSignalSpy spy(&renderSurfaceSelector, SIGNAL(surfaceChanged(QObject*)));
            QWindow newValue;
            renderSurfaceSelector.setSurface(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderSurfaceSelector.surface(), &newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderSurfaceSelector.setSurface(&newValue);

            // THEN
            QCOMPARE(renderSurfaceSelector.surface(), &newValue);
            QCOMPARE(spy.size(), 0);

            // Prevents crashes with temporary window being destroyed
            renderSurfaceSelector.setSurface(nullptr);
            QCoreApplication::processEvents();
        }
        {
            // WHEN
            QSignalSpy spy(&renderSurfaceSelector, SIGNAL(externalRenderTargetSizeChanged(QSize)));
            const QSize newValue(512, 512);
            renderSurfaceSelector.setExternalRenderTargetSize(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderSurfaceSelector.externalRenderTargetSize(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderSurfaceSelector.setExternalRenderTargetSize(newValue);

            // THEN
            QCOMPARE(renderSurfaceSelector.externalRenderTargetSize(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&renderSurfaceSelector, SIGNAL(surfacePixelRatioChanged(float)));
            const float newValue = 15.0f;
            renderSurfaceSelector.setSurfacePixelRatio(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderSurfaceSelector.surfacePixelRatio(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            renderSurfaceSelector.setSurfacePixelRatio(newValue);

            // THEN
            QCOMPARE(renderSurfaceSelector.surfacePixelRatio(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkSurfaceUpdate()
    {
        // GIVEN
        QWindow newWindow;
        TestArbiter arbiter;
        Qt3DRender::QRenderSurfaceSelector renderSurfaceSelector;
        arbiter.setArbiterOnNode(&renderSurfaceSelector);

        {
            // WHEN
            renderSurfaceSelector.setSurface(&newWindow);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSurfaceSelector);

            arbiter.clear();
        }

        {
            // WHEN
            renderSurfaceSelector.setSurface(&newWindow);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        // Prevents crashes with temporary window being destroyed
        renderSurfaceSelector.setSurface(nullptr);
    }

    void checkExternalRenderTargetSizeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSurfaceSelector renderSurfaceSelector;
        arbiter.setArbiterOnNode(&renderSurfaceSelector);

        {
            // WHEN
            renderSurfaceSelector.setExternalRenderTargetSize(QSize(454, 454));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSurfaceSelector);

            arbiter.clear();
        }

        {
            // WHEN
            renderSurfaceSelector.setExternalRenderTargetSize(QSize(454, 454));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkSurfacePixelRatioUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderSurfaceSelector renderSurfaceSelector;
        arbiter.setArbiterOnNode(&renderSurfaceSelector);

        {
            // WHEN
            renderSurfaceSelector.setSurfacePixelRatio(99.0f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &renderSurfaceSelector);

            arbiter.clear();
        }

        {
            // WHEN
            renderSurfaceSelector.setSurfacePixelRatio(99.0f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QRenderSurfaceSelector)

#include "tst_qrendersurfaceselector.moc"
