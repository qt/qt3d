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

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QtTest>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/entity_p_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entityvisitor_p.h>
#include <Qt3DRender/private/entityaccumulator_p.h>

#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QEnvironmentLight>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QShaderData>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QComputeCommand>
#include <Qt3DCore/QArmature>

#include "testrenderer.h"

typedef Qt3DCore::QNodeId (*UuidMethod)(Qt3DRender::Render::Entity *);
typedef QVector<Qt3DCore::QNodeId> (*UuidListMethod)(Qt3DRender::Render::Entity *);

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

QNodeId transformUuid(Entity *entity) { return entity->componentUuid<Transform>(); }
QNodeId cameraLensUuid(Entity *entity) { return entity->componentUuid<CameraLens>(); }
QNodeId materialUuid(Entity *entity) { return entity->componentUuid<Material>(); }
QNodeId geometryRendererUuid(Entity *entity) { return entity->componentUuid<GeometryRenderer>(); }
QNodeId objectPickerUuid(Entity *entity) { return entity->componentUuid<ObjectPicker>(); }
QNodeId computeJobUuid(Entity *entity) { return entity->componentUuid<ComputeCommand>(); }
QNodeId armatureUuid(Entity *entity) { return entity->componentUuid<Armature>(); }

QVector<QNodeId> layersUuid(Entity *entity) { return entity->componentsUuid<Layer>(); }
QVector<QNodeId> shadersUuid(Entity *entity) { return entity->componentsUuid<ShaderData>(); }
QVector<QNodeId> environmentLightsUuid(Entity *entity) { return entity->componentsUuid<EnvironmentLight>(); }

class CompleteVisitor : public EntityVisitor
{
public:
    CompleteVisitor(NodeManagers *manager) : EntityVisitor(manager) { }

    int count = 0;
    Operation visit(Entity *) { count++; return Continue; }
};

class EnabledVisitor : public EntityVisitor
{
public:
    EnabledVisitor(NodeManagers *manager) : EntityVisitor(manager) { }

    int count = 0;
    Operation visit(Entity *e) { count++; return e->isEnabled() ? Continue : Prune; }
};

class tst_RenderEntity : public QObject
{
    Q_OBJECT
public:
    tst_RenderEntity() {}
    ~tst_RenderEntity() {}

private slots:

    void checkInitialAndCleanUpState_data()
    {
        QTest::addColumn< QList<QComponent*> >("components");

        QList<QComponent *> components = QList<QComponent *>()
                << new Qt3DCore::QTransform
                << new QGeometryRenderer
                << new QCameraLens
                << new QMaterial
                << new QObjectPicker
                << new QLayer
                << new QShaderData
                << new QComputeCommand
                << new QEnvironmentLight
                << new QArmature;

        QTest::newRow("all components") << components;
    }

    void checkInitialAndCleanUpState()
    {
        // GIVEN
        QFETCH(const QList<QComponent*>, components);

        TestRenderer renderer;
        NodeManagers nodeManagers;
        Qt3DRender::Render::Entity entity;
        entity.setRenderer(&renderer);
        entity.setNodeManagers(&nodeManagers);

        // THEN
        QCOMPARE(renderer.dirtyBits(), 0);

        // THEN
        QVERIFY(entity.componentUuid<Transform>().isNull());
        QVERIFY(entity.componentUuid<CameraLens>().isNull());
        QVERIFY(entity.componentUuid<Material>().isNull());
        QVERIFY(entity.componentUuid<GeometryRenderer>().isNull());
        QVERIFY(entity.componentUuid<ObjectPicker>().isNull());
        QVERIFY(entity.componentUuid<ComputeCommand>().isNull());
        QVERIFY(entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(entity.componentsUuid<ShaderData>().isEmpty());
        QVERIFY(entity.componentsUuid<EnvironmentLight>().isEmpty());
        QVERIFY(entity.componentUuid<Armature>().isNull());
        QVERIFY(!entity.isBoundingVolumeDirty());
        QVERIFY(entity.childrenHandles().isEmpty());
        QVERIFY(entity.layerIds().isEmpty());
        QCOMPARE(entity.renderer(), &renderer);

        QCOMPARE(renderer.dirtyBits(), 0);

        // WHEN
        for (QComponent *component : components)
            EntityPrivate::get(&entity)->componentAdded(component);

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();

        // THEN
        QVERIFY(!entity.componentUuid<Transform>().isNull());
        QVERIFY(!entity.componentUuid<CameraLens>().isNull());
        QVERIFY(!entity.componentUuid<Material>().isNull());
        QVERIFY(!entity.componentUuid<GeometryRenderer>().isNull());
        QVERIFY(!entity.componentUuid<ObjectPicker>().isNull());
        QVERIFY(!entity.componentUuid<ComputeCommand>().isNull());
        QVERIFY(!entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(!entity.componentsUuid<ShaderData>().isEmpty());
        QVERIFY(!entity.componentsUuid<EnvironmentLight>().isEmpty());
        QVERIFY(!entity.componentUuid<Armature>().isNull());
        QVERIFY(entity.isBoundingVolumeDirty());
        QVERIFY(entity.childrenHandles().isEmpty());
        QVERIFY(!entity.layerIds().isEmpty());
        QCOMPARE(renderer.dirtyBits(), 0);
        bool containsAll = entity.containsComponentsOfType<Transform,
                CameraLens, Material, GeometryRenderer, ObjectPicker, ComputeCommand, Armature>();
        QVERIFY(containsAll);

        // WHEN
        entity.cleanup();

        // THEN
        QVERIFY(entity.componentUuid<Transform>().isNull());
        QVERIFY(entity.componentUuid<CameraLens>().isNull());
        QVERIFY(entity.componentUuid<Material>().isNull());
        QVERIFY(entity.componentUuid<GeometryRenderer>().isNull());
        QVERIFY(entity.componentUuid<ObjectPicker>().isNull());
        QVERIFY(entity.componentUuid<QComputeCommand>().isNull());
        QVERIFY(entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(entity.componentsUuid<ShaderData>().isEmpty());
        QVERIFY(entity.componentsUuid<EnvironmentLight>().isEmpty());
        QVERIFY(entity.componentUuid<Armature>().isNull());
        QVERIFY(!entity.isBoundingVolumeDirty());
        QVERIFY(entity.childrenHandles().isEmpty());
        QVERIFY(entity.layerIds().isEmpty());
        containsAll = entity.containsComponentsOfType<Transform,
                CameraLens, Material, GeometryRenderer, ObjectPicker, ComputeCommand, Armature>();
        QVERIFY(!containsAll);

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();
    }

    void checkEntityReparenting()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        Qt3DCore::QEntity frontendEntityA, frontendEntityB, frontendEntityC;

        auto backendA = createEntity(renderer, nodeManagers, frontendEntityA);
        auto backendB = createEntity(renderer, nodeManagers, frontendEntityB);
        auto backendC = createEntity(renderer, nodeManagers, frontendEntityC);

        // THEN
        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == nullptr);
        QVERIFY(backendC->parent() == nullptr);

        QVERIFY(backendA->childrenHandles().isEmpty());
        QVERIFY(backendB->childrenHandles().isEmpty());
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();

        // WHEN
        auto sendParentChange = [&nodeManagers](const Qt3DCore::QEntity &entity) {
            Entity *backendEntity = nodeManagers.renderNodesManager()->getOrCreateResource(entity.id());
            backendEntity->syncFromFrontEnd(&entity, false);
        };

        // reparent B to A and C to B.
        frontendEntityB.setParent(&frontendEntityA);
        sendParentChange(frontendEntityB);
        frontendEntityC.setParent(&frontendEntityB);
        sendParentChange(frontendEntityC);

        // THEN
        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == backendA);
        QVERIFY(backendC->parent() == backendB);

        QCOMPARE(backendA->childrenHandles().count(), 1);
        QCOMPARE(backendB->childrenHandles().count(), 1);
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();

          // WHEN - reparent C to A
        frontendEntityC.setParent(&frontendEntityA);
        sendParentChange(frontendEntityC);

        // THEN
        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == backendA);
        QVERIFY(backendC->parent() == backendA);

        QCOMPARE(backendA->childrenHandles().size(), 2);
        QVERIFY(backendB->childrenHandles().isEmpty());
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();

        // WHEN - reparent B to null.
        frontendEntityB.setParent(static_cast<Qt3DCore::QNode *>(nullptr));
        sendParentChange(frontendEntityB);

        // THEN
        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == nullptr);
        QVERIFY(backendC->parent() == backendA);

        QCOMPARE(backendA->childrenHandles().count(), 1);
        QVERIFY(!backendA->childrenHandles().contains(backendB->handle()));
        QVERIFY(backendB->childrenHandles().isEmpty());
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();
      }

    void checkEntityCleanup()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        Qt3DCore::QEntity frontendEntityA, frontendEntityB, frontendEntityC;

        auto backendA = createEntity(renderer, nodeManagers, frontendEntityA);
        auto backendB = createEntity(renderer, nodeManagers, frontendEntityB);
        auto backendC = createEntity(renderer, nodeManagers, frontendEntityC);

        // WHEN
        auto sendParentChange = [&nodeManagers](const Qt3DCore::QEntity &entity) {
            Entity *backendEntity = nodeManagers.renderNodesManager()->getOrCreateResource(entity.id());
            backendEntity->syncFromFrontEnd(&entity, false);
        };

        // reparent B and C to A.
        frontendEntityB.setParent(&frontendEntityA);
        sendParentChange(frontendEntityB);
        frontendEntityC.setParent(&frontendEntityA);
        sendParentChange(frontendEntityC);

        // THEN
        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == backendA);
        QVERIFY(backendC->parent() == backendA);

        QCOMPARE(backendA->childrenHandles().count(), 2);
        QVERIFY(backendB->childrenHandles().isEmpty());
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();

        // WHEN - cleaning up a child
        backendC->cleanup();

        // THEN - the child's parent should be null and it
        // should be removed from its parent's list of children
        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == backendA);
        QVERIFY(backendC->parent() == nullptr);

        QCOMPARE(backendA->childrenHandles().count(), 1);
        QVERIFY(!backendA->childrenHandles().contains(backendC->handle()));
        QVERIFY(backendB->childrenHandles().isEmpty());
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();

        // WHEN (cleaning up parent)
        backendA->cleanup();

        // THEN  (it's children's parent should be set to null)
        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == nullptr);
        QVERIFY(backendC->parent() == nullptr);

        QVERIFY(backendA->childrenHandles().isEmpty());
        QVERIFY(backendB->childrenHandles().isEmpty());
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();

        // WHEN
        backendB->cleanup();

        // THEN nothing should change
        QVERIFY(backendA->childrenHandles().isEmpty());
        QVERIFY(backendB->childrenHandles().isEmpty());
        QVERIFY(backendC->childrenHandles().isEmpty());

        QVERIFY(backendA->parent() == nullptr);
        QVERIFY(backendB->parent() == nullptr);
        QVERIFY(backendC->parent() == nullptr);

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderer.resetDirty();
    }

    void shouldHandleSingleComponentEvents_data()
    {
        QTest::addColumn<QComponent*>("component");
        QTest::addColumn<void*>("functionPtr");

        QComponent *component = new Qt3DCore::QTransform;
        QTest::newRow("transform") << component << reinterpret_cast<void*>(transformUuid);

        component = new QGeometryRenderer;
        QTest::newRow("mesh") << component << reinterpret_cast<void*>(geometryRendererUuid);

        component = new QCameraLens;
        QTest::newRow("camera lens") << component << reinterpret_cast<void*>(cameraLensUuid);

        component = new QMaterial;
        QTest::newRow("material") << component << reinterpret_cast<void*>(materialUuid);

        component = new QObjectPicker;
        QTest::newRow("objectPicker") << component << reinterpret_cast<void*>(objectPickerUuid);

        component = new QComputeCommand;
        QTest::newRow("computeJob") << component << reinterpret_cast<void*>(computeJobUuid);

        component = new QArmature;
        QTest::newRow("armature") << component << reinterpret_cast<void*>(armatureUuid);
    }

    void shouldHandleSingleComponentEvents()
    {
        // GIVEN
        QFETCH(QComponent*, component);
        QFETCH(void*, functionPtr);
        UuidMethod method = reinterpret_cast<UuidMethod>(functionPtr);

        TestRenderer renderer;
        Qt3DRender::Render::Entity entity;
        Qt3DCore::QEntity dummyFrontendEntity;
        entity.setRenderer(&renderer);

        // THEN
        QVERIFY(method(&entity).isNull());

        // WHEN
        EntityPrivate::get(&entity)->componentAdded(component);

        // THEN
        QCOMPARE(method(&entity), component->id());
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        renderer.resetDirty();
        EntityPrivate::get(&entity)->componentRemoved(component);

        // THEN
        QVERIFY(method(&entity).isNull());
        QVERIFY(renderer.dirtyBits() != 0);

        delete component;
    }

    void shouldHandleComponentsEvents_data()
    {
        QTest::addColumn< QList<QComponent*> >("components");
        QTest::addColumn<void*>("functionPtr");

        QList<QComponent*> components;

        components.clear();
        components << new QLayer << new QLayer << new QLayer;
        QTest::newRow("layers") << components << reinterpret_cast<void*>(layersUuid);

        components.clear();
        components << new QShaderData << new QShaderData << new QShaderData;
        QTest::newRow("shader data") << components << reinterpret_cast<void*>(shadersUuid);

        components.clear();
        components << new QEnvironmentLight << new QEnvironmentLight << new QEnvironmentLight;
        QTest::newRow("environmentLights") << components << reinterpret_cast<void*>(environmentLightsUuid);
    }

    void shouldHandleComponentsEvents()
    {
        // GIVEN
        QFETCH(const QList<QComponent*>, components);
        QFETCH(void*, functionPtr);
        UuidListMethod method = reinterpret_cast<UuidListMethod>(functionPtr);

        TestRenderer renderer;
        Qt3DRender::Render::Entity entity;
        Qt3DCore::QEntity dummyFrontendEntity;
        entity.setRenderer(&renderer);

        // THEN
        QVERIFY(method(&entity).isEmpty());

        // WHEN
        for (QComponent *component : components)
            EntityPrivate::get(&entity)->componentAdded(component);

        // THEN
        QCOMPARE(method(&entity).size(), components.size());
        for (QComponent *component : components) {
            QVERIFY(method(&entity).contains(component->id()));
        }
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        renderer.resetDirty();
        EntityPrivate::get(&entity)->componentRemoved(components.first());

        // THEN
        QCOMPARE(method(&entity).size(), components.size() - 1);
        QVERIFY(!method(&entity).contains(components.first()->id()));
        QVERIFY(renderer.dirtyBits() != 0);

        qDeleteAll(components);
    }

    void traversal() {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        Qt3DCore::QEntity frontendEntityA, frontendEntityB, frontendEntityC;

        auto backendA = createEntity(renderer, nodeManagers, frontendEntityA);
        createEntity(renderer, nodeManagers, frontendEntityB);
        createEntity(renderer, nodeManagers, frontendEntityC);

        auto sendParentChange = [&nodeManagers](const Qt3DCore::QEntity &entity) {
            Entity *backendEntity = nodeManagers.renderNodesManager()->getOrCreateResource(entity.id());
            backendEntity->syncFromFrontEnd(&entity, false);
        };

        // reparent B to A and C to B.
        frontendEntityB.setParent(&frontendEntityA);
        sendParentChange(frontendEntityB);
        frontendEntityC.setParent(&frontendEntityB);
        sendParentChange(frontendEntityC);

        // WHEN
        int visitCount = 0;
        auto counter = [&visitCount](const Entity *) { ++visitCount; };
        backendA->traverse(counter);

        // THEN
        QCOMPARE(visitCount, 3);
    }

    void visitor()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        Qt3DCore::QEntity frontendEntityA, frontendEntityB, frontendEntityC;

        frontendEntityA.setEnabled(false);
        frontendEntityB.setEnabled(false);
        frontendEntityC.setEnabled(false);

        auto backendA = createEntity(renderer, nodeManagers, frontendEntityA);
        createEntity(renderer, nodeManagers, frontendEntityB);
        createEntity(renderer, nodeManagers, frontendEntityC);

        auto sendParentChange = [&nodeManagers](const Qt3DCore::QEntity &entity) {
            Entity *backendEntity = nodeManagers.renderNodesManager()->getOrCreateResource(entity.id());
            backendEntity->syncFromFrontEnd(&entity, false);
        };

        // reparent B to A and C to B.
        frontendEntityB.setParent(&frontendEntityA);
        sendParentChange(frontendEntityB);
        frontendEntityC.setParent(&frontendEntityB);
        sendParentChange(frontendEntityC);

       // WHEN
       CompleteVisitor v1(&nodeManagers);
       EnabledVisitor v2(&nodeManagers);
       CompleteVisitor v3(&nodeManagers);
       v3.setPruneDisabled(true);
       v1.apply(backendA);
       v2.apply(backendA);
       v3.apply(backendA);

       // THEN
       QCOMPARE(v1.count, 3);
       QCOMPARE(v2.count, 1); // nodes disabled but the first one is still visited before visitation finds out it's disabled
       QCOMPARE(v3.count, 0); // nodes disabled
    }

    void accumulator()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        Qt3DCore::QEntity frontendEntityA, frontendEntityB, frontendEntityC;

        frontendEntityA.setEnabled(false);
        frontendEntityB.setEnabled(false);
        frontendEntityC.setEnabled(false);

        auto backendA = createEntity(renderer, nodeManagers, frontendEntityA);
        createEntity(renderer, nodeManagers, frontendEntityB);
        createEntity(renderer, nodeManagers, frontendEntityC);

        auto sendParentChange = [&nodeManagers](const Qt3DCore::QEntity &entity) {
            Entity *backendEntity = nodeManagers.renderNodesManager()->getOrCreateResource(entity.id());
            backendEntity->syncFromFrontEnd(&entity, false);
        };

        // reparent B to A and C to B.
        frontendEntityB.setParent(&frontendEntityA);
        sendParentChange(frontendEntityB);
        frontendEntityC.setParent(&frontendEntityB);
        sendParentChange(frontendEntityC);

        // WHEN
        EntityAccumulator v1(&nodeManagers);
        EntityAccumulator v2([](Entity *e) { return e->isEnabled(); }, &nodeManagers);
        const auto r1 = v1.apply(backendA);
        const auto r2 = v2.apply(backendA);

        // THEN
        QCOMPARE(r1.count(), 3);
        QCOMPARE(r2.count(), 0);
    }

private:
    Entity *createEntity(TestRenderer &renderer, NodeManagers &nodeManagers, const Qt3DCore::QEntity &frontEndEntity) {
        HEntity renderNodeHandle = nodeManagers.renderNodesManager()->getOrAcquireHandle(frontEndEntity.id());
        Entity *entity = nodeManagers.renderNodesManager()->data(renderNodeHandle);
        entity->setNodeManagers(&nodeManagers);
        entity->setHandle(renderNodeHandle);
        entity->setRenderer(&renderer);
        entity->syncFromFrontEnd(&frontEndEntity, true);
        return entity;
    }

};

QTEST_APPLESS_MAIN(tst_RenderEntity)

#include "tst_entity.moc"
