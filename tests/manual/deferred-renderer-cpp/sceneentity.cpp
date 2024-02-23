// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "sceneentity.h"
#include "sceneeffect.h"
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DCore/QTransform>

QT_BEGIN_NAMESPACE

SceneEntity::SceneEntity(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_layer(new Qt3DRender::QLayer(this))
    , m_effect(new SceneEffect(this))
{
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh(this);
    sphereMesh->setRings(50);
    sphereMesh->setSlices(100);

    {
        Qt3DRender::QPointLight *light1 = new Qt3DRender::QPointLight(this);
        light1->setColor(Qt::white);
        light1->setIntensity(1.5f);

        addComponent(light1);
    }

    {
        Qt3DCore::QEntity *sphereOne = new Qt3DCore::QEntity(this);
        Qt3DRender::QMaterial *sphereOneMaterial = new Qt3DRender::QMaterial(sphereOne);

        sphereOneMaterial->setEffect(m_effect);
        sphereOneMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("meshColor"), QColor(Qt::blue)));

        Qt3DCore::QTransform *sphereOneTransform = new Qt3DCore::QTransform(sphereOne);
        sphereOneTransform->setTranslation(QVector3D(-10.0f, 0.0f, 25.0f));

        Qt3DRender::QPointLight *light2 = new Qt3DRender::QPointLight(sphereOne);
        light2->setColor(Qt::white);
        light2->setIntensity(1.5f);

        sphereOne->addComponent(sphereOneTransform);
        sphereOne->addComponent(sphereOneMaterial);
        sphereOne->addComponent(sphereMesh);
        sphereOne->addComponent(m_layer);
        sphereOne->addComponent(light2);
    }

    {
        Qt3DCore::QEntity *sphereTwo = new Qt3DCore::QEntity(this);
        Qt3DRender::QMaterial *sphereTwoMaterial = new Qt3DRender::QMaterial(sphereTwo);
        sphereTwoMaterial->setEffect(m_effect);
        sphereTwoMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("meshColor"), QColor(Qt::yellow)));

        Qt3DRender::QPointLight *light3 = new Qt3DRender::QPointLight(sphereTwo);
        light3->setColor(Qt::blue);
        light3->setIntensity(0.5f);

        sphereTwo->addComponent(sphereMesh);
        sphereTwo->addComponent(m_layer);
        sphereTwo->addComponent(sphereTwoMaterial);
        sphereTwo->addComponent(light3);
    }
}

Qt3DRender::QLayer *SceneEntity::layer() const
{
    return m_layer;
}

SceneEffect *SceneEntity::effect() const
{
    return m_effect;
}

QT_END_NAMESPACE
