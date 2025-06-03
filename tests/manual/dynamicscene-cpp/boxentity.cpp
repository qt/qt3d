// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "boxentity.h"
#include <Qt3DRender/QGeometryRenderer>

#include <qmath.h>

BoxEntity::BoxEntity(QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_transform(new Qt3DCore::QTransform())
    , m_mesh(new Qt3DExtras::QCuboidMesh())
    , m_material(new Qt3DExtras::QPhongMaterial())
    , m_angle(0.0f)
    , m_radius(1.0f)
{
    connect(m_material, SIGNAL(diffuseChanged(QColor)),
            this, SIGNAL(diffuseColorChanged(QColor)));
    m_material->setAmbient(Qt::gray);
    m_material->setSpecular(Qt::white);
    m_material->setShininess(150.0f);

    addComponent(m_transform);
    addComponent(m_mesh);
    addComponent(m_material);
}

void BoxEntity::setDiffuseColor(const QColor &diffuseColor)
{
    m_material->setDiffuse(diffuseColor);
}

void BoxEntity::setAngle(float arg)
{
    if (m_angle == arg)
        return;

    m_angle = arg;
    emit angleChanged();
    updateTransformation();
}

void BoxEntity::setRadius(float arg)
{
    if (m_radius == arg)
        return;

    m_radius = arg;
    emit radiusChanged();
    updateTransformation();
}

QColor BoxEntity::diffuseColor()
{
    return m_material->diffuse();
}

float BoxEntity::angle() const
{
    return m_angle;
}

float BoxEntity::radius() const
{
    return m_radius;
}

void BoxEntity::updateTransformation()
{
    m_transform->setTranslation(QVector3D(qCos(m_angle) * m_radius,
                                          1.0f,
                                          qSin(m_angle) * m_radius));
}

