// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "entity.h"

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DCore/QTransform>
#include <QMatrix4x4>

Entity::Entity(Qt3DRender::QEffect *effect, Qt3DCore::QNode *parent)
    : QEntity(parent)
    , m_transform(new Qt3DCore::QTransform())
    , m_material(new Qt3DRender::QMaterial())
    , m_diffuseColorParam(new Qt3DRender::QParameter())
{
    m_diffuseColorParam->setName(QLatin1String("kd"));
    m_material->addParameter(m_diffuseColorParam);
    m_material->setEffect(effect);

    addComponent(m_transform);
    addComponent(m_material);
}

void Entity::updateTransform()
{
    QMatrix4x4 m;
    m.translate(m_position);
    m.rotate(m_phi, QVector3D(1.0f, 0.0f, 0.0f));
    m.rotate(m_theta, QVector3D(0.0f, 0.0f, 1.0f));
    m_transform->setMatrix(m);
}

float Entity::theta() const
{
    return m_theta;
}

float Entity::phi() const
{
    return m_phi;
}

QVector3D Entity::position() const
{
    return m_position;
}

QColor Entity::diffuseColor() const
{
    return m_diffuseColorParam->value().value<QColor>();
}

void Entity::setTheta(float theta)
{
    if (qFuzzyCompare(m_theta, theta))
        return;

    m_theta = theta;
    const bool wasBlocked = blockNotifications(true);
    emit thetaChanged(theta);
    blockNotifications(wasBlocked);
    updateTransform();
}

void Entity::setPhi(float phi)
{
    if (qFuzzyCompare(m_phi, phi))
        return;

    m_phi = phi;
    const bool wasBlocked = blockNotifications(true);
    emit phiChanged(phi);
    blockNotifications(wasBlocked);
    updateTransform();
}

void Entity::setPosition(QVector3D position)
{
    if (m_position == position)
        return;

    m_position = position;
    emit positionChanged(position);
    updateTransform();
}

void Entity::setDiffuseColor(QColor diffuseColor)
{
    if (m_diffuseColorParam->value().value<QColor>() == diffuseColor)
        return;

    m_diffuseColorParam->setValue(QVariant::fromValue(diffuseColor));
    const bool wasBlocked = blockNotifications(true);
    emit diffuseColorChanged(diffuseColor);
    blockNotifications(wasBlocked);
}
