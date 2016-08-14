/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/private/qrendersurfaceselector_p.h>

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
};

QTEST_MAIN(tst_QRenderSurfaceSelector)

#include "tst_qrendersurfaceselector.moc"
