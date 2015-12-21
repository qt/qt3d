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

#include "entity.h"

#include <Qt3DRender/QCylinderMesh>
#include <Qt3DRender/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <QMatrix4x4>

Entity::Entity(Qt3DCore::QNode *parent)
    : QEntity(parent)
    , m_transform(new Qt3DCore::QTransform())
    , m_mesh(new Qt3DRender::QCylinderMesh())
    , m_material(new Qt3DRender::QPhongMaterial())
{
    m_mesh->setRings(50.0f);
    m_mesh->setSlices(30.0f);
    m_mesh->setRadius(2.5f);
    m_mesh->setLength(5.0f);

    addComponent(m_mesh);
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
    return m_material->diffuse();
}

void Entity::setTheta(float theta)
{
    if (m_theta == theta)
        return;

    m_theta = theta;
    emit thetaChanged(theta);
    updateTransform();
}

void Entity::setPhi(float phi)
{
    if (m_phi == phi)
        return;

    m_phi = phi;
    emit phiChanged(phi);
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
    if (m_material->diffuse() == diffuseColor)
        return;

    m_material->setDiffuse(diffuseColor);
    emit diffuseColorChanged(diffuseColor);
}
