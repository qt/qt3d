// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/filterentitybycomponentjob_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qcomputecommand.h>

#include <Qt3DRender/private/transform_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/computecommand_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include "testaspect.h"

class tst_FilterEntityByComponent : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void filterEntities()
    {
        // GIVEN
        Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
        Qt3DCore::QEntity *childEntity1 = new Qt3DCore::QEntity(rootEntity);
        Qt3DCore::QEntity *childEntity2 = new Qt3DCore::QEntity(rootEntity);
        Qt3DCore::QEntity *childEntity3 = new Qt3DCore::QEntity(rootEntity);

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform(rootEntity);
        Qt3DRender::QGeometryRenderer *geomRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
        Qt3DRender::QCameraLens *cameraLens = new Qt3DRender::QCameraLens(rootEntity);
        Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial(rootEntity);
        Qt3DRender::QObjectPicker *objectPicker = new Qt3DRender::QObjectPicker(rootEntity);
        Qt3DRender::QComputeCommand *computeCommand = new Qt3DRender::QComputeCommand(rootEntity);

        childEntity1->addComponent(transform);
        childEntity1->addComponent(geomRenderer);
        childEntity1->addComponent(material);

        childEntity2->addComponent(cameraLens);
        childEntity2->addComponent(transform);

        childEntity3->addComponent(objectPicker);
        childEntity3->addComponent(computeCommand);

        QScopedPointer<Qt3DRender::TestAspect> aspect(new Qt3DRender::TestAspect(rootEntity));

        {
            // WHEN
            Qt3DRender::Render::FilterEntityByComponentJob<Qt3DRender::Render::Transform> filterJob;
            filterJob.setManager(aspect->nodeManagers()->renderNodesManager());
            filterJob.run();

            // THEN
            QCOMPARE(filterJob.filteredEntities().size(), 2U);
            QCOMPARE(filterJob.filteredEntities().front()->peerId(), childEntity1->id());
            QCOMPARE(filterJob.filteredEntities().back()->peerId(), childEntity2->id());
        }

        {
            // WHEN
            Qt3DRender::Render::FilterEntityByComponentJob<Qt3DRender::Render::Transform, Qt3DRender::Render::Material> filterJob;
            filterJob.setManager(aspect->nodeManagers()->renderNodesManager());
            filterJob.run();

            // THEN
            QCOMPARE(filterJob.filteredEntities().size(), 1U);
            QCOMPARE(filterJob.filteredEntities().front()->peerId(), childEntity1->id());
        }

        {
            // WHEN
            Qt3DRender::Render::FilterEntityByComponentJob<Qt3DRender::Render::Transform, Qt3DRender::Render::Material, Qt3DRender::Render::ComputeCommand> filterJob;
            filterJob.setManager(aspect->nodeManagers()->renderNodesManager());
            filterJob.run();

            // THEN
            QCOMPARE(filterJob.filteredEntities().size(), 0U);
        }

        {
            // WHEN
            Qt3DRender::Render::FilterEntityByComponentJob<Qt3DRender::Render::ComputeCommand> filterJob;
            filterJob.setManager(aspect->nodeManagers()->renderNodesManager());
            filterJob.run();

            // THEN
            QCOMPARE(filterJob.filteredEntities().size(), 1U);
            QCOMPARE(filterJob.filteredEntities().front()->peerId(), childEntity3->id());
        }
    }
};

QTEST_MAIN(tst_FilterEntityByComponent)

#include "tst_filterentitybycomponent.moc"
