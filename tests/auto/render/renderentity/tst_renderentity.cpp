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

#include <QtTest/QtTest>
#include <Qt3DRenderer/private/renderentity_p.h>

#include <Qt3DCore/QCameraLens>
#include <Qt3DCore/QScenePropertyChange>
#include <Qt3DCore/QTransform>

#include <Qt3DRenderer/QMesh>
#include <Qt3DRenderer/QMaterial>
#include <Qt3DRenderer/QLayer>
#include <Qt3DRenderer/QShaderData>
#include <Qt3DRenderer/QGeometryRenderer>

typedef Qt3D::QNodeId (*UuidMethod)(Qt3D::Render::RenderEntity *);
typedef QList<Qt3D::QNodeId> (*UuidListMethod)(Qt3D::Render::RenderEntity *);

using namespace Qt3D;
using namespace Qt3D::Render;

void noopDeleter(QNode *) {}


QNodeId transformUuid(RenderEntity *entity) { return entity->componentUuid<RenderTransform>(); }
QNodeId cameraLensUuid(RenderEntity *entity) { return entity->componentUuid<RenderCameraLens>(); }
QNodeId materialUuid(RenderEntity *entity) { return entity->componentUuid<RenderMaterial>(); }
QNodeId geometryRendererUuid(RenderEntity *entity) { return entity->componentUuid<RenderGeometryRenderer>(); }

QList<QNodeId> layersUuid(RenderEntity *entity) { return entity->componentsUuid<RenderLayer>(); }
QList<QNodeId> shadersUuid(RenderEntity *entity) { return entity->componentsUuid<RenderShaderData>(); }

class tst_RenderEntity : public QObject
{
    Q_OBJECT
public:
    tst_RenderEntity() {}
    ~tst_RenderEntity() {}

private slots:
    void shouldHandleSingleComponentEvents_data()
    {
        QTest::addColumn<QComponent*>("component");
        QTest::addColumn<void*>("functionPtr");

        QComponent *component = new Qt3D::QTransform;
        QTest::newRow("transform") << component << reinterpret_cast<void*>(transformUuid);

        component = new QGeometryRenderer;
        QTest::newRow("mesh") << component << reinterpret_cast<void*>(geometryRendererUuid);

        component = new QCameraLens;
        QTest::newRow("camera lens") << component << reinterpret_cast<void*>(cameraLensUuid);

        component = new QMaterial;
        QTest::newRow("material") << component << reinterpret_cast<void*>(materialUuid);
    }

    void shouldHandleSingleComponentEvents()
    {
        // GIVEN
        QFETCH(QComponent*, component);
        QFETCH(void*, functionPtr);
        UuidMethod method = reinterpret_cast<UuidMethod>(functionPtr);

        RenderEntity entity;

        // THEN
        QVERIFY(method(&entity).isNull());

        // WHEN
        QScenePropertyChangePtr addChange(new QScenePropertyChange(ComponentAdded, QSceneChange::Node, component->id()));
        addChange->setPropertyName("component");
        addChange->setValue(QVariant::fromValue(QNodePtr(component, noopDeleter)));
        entity.sceneChangeEvent(addChange);

        // THEN
        QCOMPARE(method(&entity), component->id());

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(ComponentRemoved, QSceneChange::Node, component->id()));
        removeChange->setPropertyName("componentId");
        removeChange->setValue(QVariant::fromValue(component->id()));
        entity.sceneChangeEvent(removeChange);

        // THEN
        QVERIFY(method(&entity).isNull());

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

        RenderEntity entity;

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

        // WHEN
        QScenePropertyChangePtr removeChange(new QScenePropertyChange(ComponentRemoved, QSceneChange::Node, components.first()->id()));
        removeChange->setPropertyName("componentId");
        removeChange->setValue(QVariant::fromValue(components.first()->id()));
        entity.sceneChangeEvent(removeChange);

        // THEN
        QCOMPARE(method(&entity).size(), components.size() - 1);
        QVERIFY(!method(&entity).contains(components.first()->id()));

        qDeleteAll(components);
    }
};

QTEST_APPLESS_MAIN(tst_RenderEntity)

#include "tst_renderentity.moc"
