// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/sortpolicy_p.h>
#include "testrenderer.h"

class tst_SortPolicy : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:
    void checkInitialState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::SortPolicy backendNode;
        backendNode.setRenderer(&renderer);

        // THEN
        QVERIFY(backendNode.peerId().isNull());
        QVERIFY(backendNode.parentId().isNull());
        QVERIFY(backendNode.sortTypes().isEmpty());
    }

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QFrameGraphNode parent;
        auto parentBackend = new Qt3DRender::Render::FrameGraphNode;
        parentBackend->setRenderer(&renderer);
        simulateInitializationSync(&parent, parentBackend);

        Qt3DRender::Render::FrameGraphManager manager;
        manager.appendNode(parent.id(), parentBackend);

        const QList<Qt3DRender::QSortPolicy::SortType> sortTypes
                = { Qt3DRender::QSortPolicy::BackToFront,
                    Qt3DRender::QSortPolicy::Material };
        Qt3DRender::Render::SortPolicy backendNode;
        backendNode.setFrameGraphManager(&manager);
        Qt3DRender::QSortPolicy sortPolicy(&parent);
        sortPolicy.setSortTypes(sortTypes);

        // WHEN
        backendNode.setRenderer(&renderer);
        simulateInitializationSync(&sortPolicy, &backendNode);

        // THEN
        QCOMPARE(backendNode.peerId(), sortPolicy.id());
        QCOMPARE(backendNode.parentId(), parent.id());
        QCOMPARE(backendNode.sortTypes(), sortTypes);
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        const QList<Qt3DRender::QSortPolicy::SortType> sortTypes
                = { Qt3DRender::QSortPolicy::BackToFront,
                    Qt3DRender::QSortPolicy::Material };
        TestRenderer renderer;
        Qt3DRender::Render::SortPolicy backendNode;
        backendNode.setRenderer(&renderer);

        Qt3DRender::QSortPolicy sortPolicy;
        sortPolicy.setSortTypes(sortTypes);

        simulateInitializationSync(&sortPolicy, &backendNode);

        // WHEN
        sortPolicy.setSortTypes(sortTypes);
        backendNode.syncFromFrontEnd(&sortPolicy, false);

        // THEN
        QCOMPARE(backendNode.sortTypes(), sortTypes);
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
    }
};


QTEST_APPLESS_MAIN(tst_SortPolicy)

#include "tst_sortpolicy.moc"
