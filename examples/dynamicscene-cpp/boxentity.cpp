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

#include "boxentity.h"

#include <qmath.h>

BoxEntity::BoxEntity(QNode *parent)
    : Qt3D::QEntity(parent)
    , m_transform(new Qt3D::QTransform())
    , m_translate(new Qt3D::QTranslateTransform())
    , m_mesh(new Qt3D::QCuboidMesh())
    , m_material(new Qt3D::QPhongMaterial())
    , m_angle(0.0f)
    , m_radius(1.0f)
{
    m_transform->addTransform(m_translate);

    connect(m_material, SIGNAL(diffuseChanged()), this, SIGNAL(diffuseColorChanged()));
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
    m_translate->setTranslation(QVector3D(qCos(m_angle) * m_radius,
                                          1.0f,
                                          qSin(m_angle) * m_radius));
}

