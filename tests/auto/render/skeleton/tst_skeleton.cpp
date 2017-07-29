/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRender/private/skeleton_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DCore/qskeletonloader.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DCore/private/qpropertyupdatedchangebase_p.h>
#include <qbackendnodetester.h>
#include <testpostmanarbiter.h>
#include <testrenderer.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

class tst_Skeleton : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Skeleton backendSkeleton;
        backendSkeleton.setRenderer(&renderer);
        QSkeletonLoader skeleton;

        skeleton.setSource(QUrl::fromLocalFile("funnybones.json"));

        // WHEN
        simulateInitialization(&skeleton, &backendSkeleton);

        // THEN
        QCOMPARE(backendSkeleton.peerId(), skeleton.id());
        QCOMPARE(backendSkeleton.isEnabled(), skeleton.isEnabled());
        QCOMPARE(backendSkeleton.source(), skeleton.source());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Skeleton backendSkeleton;
        backendSkeleton.setRenderer(&renderer);

        // THEN
        QVERIFY(backendSkeleton.peerId().isNull());
        QCOMPARE(backendSkeleton.isEnabled(), false);
        QCOMPARE(backendSkeleton.source(), QUrl());
        QCOMPARE(backendSkeleton.status(), QSkeletonLoader::NotReady);

        // GIVEN
        QSkeletonLoader skeleton;
        skeleton.setSource(QUrl::fromLocalFile("skeleton1.json"));

        // WHEN
        simulateInitialization(&skeleton, &backendSkeleton);
        backendSkeleton.cleanup();

        // THEN
        QCOMPARE(backendSkeleton.source(), QUrl());
        QCOMPARE(backendSkeleton.isEnabled(), false);
        QCOMPARE(backendSkeleton.status(), QSkeletonLoader::NotReady);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Skeleton backendSkeleton;
        backendSkeleton.setRenderer(&renderer);
        backendSkeleton.setDataType(Skeleton::File);
        Qt3DCore::QPropertyUpdatedChangePtr updateChange;

        // Initialize to ensure skeleton manager is set
        QSkeletonLoader skeleton;
        skeleton.setSource(QUrl::fromLocalFile("skeleton1.json"));
        simulateInitialization(&skeleton, &backendSkeleton);

        // WHEN
        updateChange.reset(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        updateChange->setPropertyName("enabled");
        updateChange->setValue(true);
        backendSkeleton.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendSkeleton.isEnabled(), true);

        // WHEN
        const QUrl newSource = QUrl::fromLocalFile("terminator.json");
        updateChange.reset(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        updateChange->setPropertyName("source");
        updateChange->setValue(newSource);
        backendSkeleton.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendSkeleton.source(), newSource);
    }

    void checkStatusPropertyBackendNotification()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        TestArbiter arbiter;
        Skeleton backendSkeleton;
        backendSkeleton.setRenderer(&renderer);
        backendSkeleton.setEnabled(true);
        Qt3DCore::QBackendNodePrivate::get(&backendSkeleton)->setArbiter(&arbiter);

        // WHEN
        backendSkeleton.setStatus(QSkeletonLoader::Error);

        // THEN
        QCOMPARE(backendSkeleton.status(), QSkeletonLoader::Error);
        QCOMPARE(arbiter.events.count(), 1);
        Qt3DCore::QPropertyUpdatedChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "status");
        QCOMPARE(change->value().value<QSkeletonLoader::Status>(), backendSkeleton.status());
        QCOMPARE(Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(change.data())->m_isIntermediate,
                 false);

        arbiter.events.clear();

        // WHEN
        backendSkeleton.setStatus(QSkeletonLoader::Error);

        // THEN
        QCOMPARE(backendSkeleton.status(), QSkeletonLoader::Error);
        QCOMPARE(arbiter.events.count(), 0);

        arbiter.events.clear();
    }
};

QTEST_APPLESS_MAIN(tst_Skeleton)

#include "tst_skeleton.moc"
