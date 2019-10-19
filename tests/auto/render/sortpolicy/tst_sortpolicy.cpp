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
        simulateInitialization(&parent, parentBackend);

        Qt3DRender::Render::FrameGraphManager manager;
        manager.appendNode(parent.id(), parentBackend);

        const auto sortTypes = QVector<Qt3DRender::QSortPolicy::SortType>() << Qt3DRender::QSortPolicy::BackToFront
                                                                            << Qt3DRender::QSortPolicy::Material;
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
        const auto sortTypes = QVector<Qt3DRender::QSortPolicy::SortType>() << Qt3DRender::QSortPolicy::BackToFront
                                                                            << Qt3DRender::QSortPolicy::Material;
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
