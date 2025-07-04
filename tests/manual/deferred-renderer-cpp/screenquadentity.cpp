// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "screenquadentity.h"
#include "finaleffect.h"
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>

#include <QtGui/qquaternion.h>

QT_BEGIN_NAMESPACE

ScreenQuadEntity::ScreenQuadEntity(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_layer(new Qt3DRender::QLayer(this))
    , m_effect(new FinalEffect(this))
{
    Qt3DRender::QMaterial *screenQuadMaterial = new Qt3DRender::QMaterial(this);
    screenQuadMaterial->setEffect(m_effect);

    Qt3DCore::QTransform *screenPlaneTransform = new Qt3DCore::QTransform(this);
    screenPlaneTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 90.0f));

    Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh();
    planeMesh->setMeshResolution(QSize(2, 2));
    planeMesh->setWidth(2.0f);
    planeMesh->setHeight(2.0f);

    addComponent(m_layer);
    addComponent(screenPlaneTransform);
    addComponent(screenQuadMaterial);
    addComponent(planeMesh);
}

Qt3DRender::QLayer *ScreenQuadEntity::layer() const
{
    return m_layer;
}

FinalEffect *ScreenQuadEntity::effect() const
{
    return m_effect;
}

QT_END_NAMESPACE
