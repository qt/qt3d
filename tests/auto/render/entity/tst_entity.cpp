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

#include <QtTest/QtTest>
#include <Qt3DRender/private/entity_p.h>

#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QScenePropertyChange>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QMesh>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QShaderData>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QBoundingVolumeDebug>
#include <Qt3DRender/QComputeCommand>

#include "testrenderer.h"

typedef Qt3DCore::QNodeId (*UuidMethod)(Qt3DRender::Render::Entity *);
typedef QVector<Qt3DCore::QNodeId> (*UuidListMethod)(Qt3DRender::Render::Entity *);

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

void noopDeleter(QNode *) {}


QNodeId transformUuid(Entity *entity) { return entity->componentUuid<Transform>(); }
QNodeId cameraLensUuid(Entity *entity) { return entity->componentUuid<CameraLens>(); }
QNodeId materialUuid(Entity *entity) { return entity->componentUuid<Material>(); }
QNodeId geometryRendererUuid(Entity *entity) { return entity->componentUuid<GeometryRenderer>(); }
QNodeId objectPickerUuid(Entity *entity) { return entity->componentUuid<ObjectPicker>(); }
QNodeId boundingVolumeDebugUuid(Entity *entity) { return entity->componentUuid<BoundingVolumeDebug>(); }
QNodeId computeJobUuid(Entity *entity) { return entity->componentUuid<ComputeCommand>(); }

QVector<QNodeId> layersUuid(Entity *entity) { return entity->componentsUuid<Layer>(); }
QVector<QNodeId> shadersUuid(Entity *entity) { return entity->componentsUuid<ShaderData>(); }

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
                << new QBoundingVolumeDebug
                << new QComputeCommand;

        QTest::newRow("all components") << components;
    }

    void checkInitialAndCleanUpState()
    {
        // GIVEN
        QFETCH(QList<QComponent*>, components);

        TestRenderer renderer;
        Qt3DRender::Render::Entity entity;
        entity.setRenderer(&renderer);

        // THEN
        QVERIFY(entity.componentUuid<Transform>().isNull());
        QVERIFY(entity.componentUuid<CameraLens>().isNull());
        QVERIFY(entity.componentUuid<Material>().isNull());
        QVERIFY(entity.componentUuid<GeometryRenderer>().isNull());
        QVERIFY(entity.componentUuid<ObjectPicker>().isNull());
        QVERIFY(entity.componentUuid<BoundingVolumeDebug>().isNull());
        QVERIFY(entity.componentUuid<ComputeCommand>().isNull());
        QVERIFY(entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(entity.componentsUuid<ShaderData>().isEmpty());
        QVERIFY(!entity.isBoundingVolumeDirty());

        // WHEN
        Q_FOREACH (QComponent *component, components) {
            QScenePropertyChangePtr addChange(new QScenePropertyChange(ComponentAdded, QSceneChange::Node, component->id()));
            addChange->setPropertyName("component");
            addChange->setValue(QVariant::fromValue(QNodePtr(component, noopDeleter)));
            entity.sceneChangeEvent(addChange);
        }

        // THEN
        QVERIFY(!entity.componentUuid<Transform>().isNull());
        QVERIFY(!entity.componentUuid<CameraLens>().isNull());
        QVERIFY(!entity.componentUuid<Material>().isNull());
        QVERIFY(!entity.componentUuid<GeometryRenderer>().isNull());
        QVERIFY(!entity.componentUuid<ObjectPicker>().isNull());
        QVERIFY(!entity.componentUuid<BoundingVolumeDebug>().isNull());
        QVERIFY(!entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(!entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(!entity.componentsUuid<ShaderData>().isEmpty());
        QVERIFY(entity.isBoundingVolumeDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        entity.cleanup();

        // THEN
        QVERIFY(entity.componentUuid<Transform>().isNull());
        QVERIFY(entity.componentUuid<CameraLens>().isNull());
        QVERIFY(entity.componentUuid<Material>().isNull());
        QVERIFY(entity.componentUuid<GeometryRenderer>().isNull());
        QVERIFY(entity.componentUuid<ObjectPicker>().isNull());
        QVERIFY(entity.componentUuid<BoundingVolumeDebug>().isNull());
        QVERIFY(entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(entity.componentsUuid<Layer>().isEmpty());
        QVERIFY(entity.componentsUuid<ShaderData>().isEmpty());
        QVERIFY(!entity.isBoundingVolumeDirty());
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

        component = new QBoundingVolumeDebug;
        QTest::newRow("boundingVolumeDebug") << component << reinterpret_cast<void*>(boundingVolumeDebugUuid);

        component = new QComputeCommand;
        QTest::newRow("computeJob") << component << reinterpret_cast<void*>(computeJobUuid);
    }

    void shouldHandleSingleComponentEvents()
    {
        // GIVEN
        QFETCH(QComponent*, component);
        QFETCH(void*, functionPtr);
        UuidMethod method = reinterpret_cast<UuidMethod>(functionPtr);

        TestRenderer renderer;
        Qt3DRender::Render::Entity entity;
        entity.setRenderer(&renderer);

        // THEN
        QVERIFY(method(&entity).isNull());

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(ComponentAdded, QSceneChange::Node, component->id()));
        addChange->setPropertyName("component");
        addChange->setValue(QVariant::fromValue(QNodePtr(component, noopDeleter)));
        entity.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(method(&entity), component->id());
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        renderer.resetDirty();
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(ComponentRemoved, QSceneChange::Node, component->id()));
        removeChange->setPropertyName("componentId");
        removeChange->setValue(QVariant::fromValue(component->id()));
        entity.sceneChangeEvent(removeChange);

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
    }

    void shouldHandleComponentsEvents()
    {
        // GIVEN
        QFETCH(QList<QComponent*>, components);
        QFETCH(void*, functionPtr);
        UuidListMethod method = reinterpret_cast<UuidListMethod>(functionPtr);

        TestRenderer renderer;
        Qt3DRender::Render::Entity entity;
        entity.setRenderer(&renderer);

        // THEN
        QVERIFY(method(&entity).isEmpty());

        // WHEN
        Q_FOREACH (QComponent *component, components) {
            QScenePropertyChangePtr addChange(new QScenePropertyChange(ComponentAdded, QSceneChange::Node, component->id()));
            addChange->setPropertyName("component");
            addChange->setValue(QVariant::fromValue(QNodePtr(component, noopDeleter)));
            entity.sceneChangeEvent(addChange);
        }

        // THEN
        QCOMPARE(method(&entity).size(), components.size());
        Q_FOREACH (QComponent *component, components) {
            QVERIFY(method(&entity).contains(component->id()));
        }
        QVERIFY(renderer.dirtyBits() != 0);

        // WHEN
        renderer.resetDirty();
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(ComponentRemoved, QSceneChange::Node, components.first()->id()));
        removeChange->setPropertyName("componentId");
        removeChange->setValue(QVariant::fromValue(components.first()->id()));
        entity.sceneChangeEvent(removeChange);

        // THEN
        QCOMPARE(method(&entity).size(), components.size() - 1);
        QVERIFY(!method(&entity).contains(components.first()->id()));
        QVERIFY(renderer.dirtyBits() != 0);

        qDeleteAll(components);
    }
};

QTEST_APPLESS_MAIN(tst_RenderEntity)

#include "tst_entity.moc"
