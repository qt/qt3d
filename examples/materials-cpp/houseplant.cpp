/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "houseplant.h"
#include <Qt3DRenderer/qtexture.h>

const char *potNames[] = {
    "cross",
    "square",
    "triangle",
    "sphere"
};

const char *plantNames[] = {
    "bamboo",
    "palm",
    "pine",
    "spikes",
    "shrub"
};


HousePlant::HousePlant(Qt3D::QNode *parent)
    : Qt3D::QEntity(parent)
    , m_pot(new RenderableEntity(this))
    , m_plant(new RenderableEntity(m_pot))
    , m_cover(new RenderableEntity(m_pot))
    , m_potMaterial(new Qt3D::QNormalDiffuseMapMaterial())
    , m_plantMaterial(new Qt3D::QNormalDiffuseMapAlphaMaterial())
    , m_coverMaterial(new Qt3D::QNormalDiffuseMapMaterial())
    , m_potImage(new Qt3D::QTextureImage())

    , m_potNormalImage(new Qt3D::QTextureImage())
    , m_plantImage(new Qt3D::QTextureImage())
    , m_plantNormalImage(new Qt3D::QTextureImage())
    , m_coverImage(new Qt3D::QTextureImage())
    , m_coverNormalImage(new Qt3D::QTextureImage())
    , m_plantType(Bamboo)
    , m_potShape(Cross)
{
    m_pot->scaleTransform()->setScale(0.03f);
    m_pot->addComponent(m_potMaterial);
    m_plant->addComponent(m_plantMaterial);
    m_cover->addComponent(m_coverMaterial);

    m_potMaterial->diffuse()->addTextureImage(m_potImage);
    m_potMaterial->normal()->addTextureImage(m_potNormalImage);
    m_plantMaterial->diffuse()->addTextureImage(m_plantImage);
    m_plantMaterial->normal()->addTextureImage(m_plantNormalImage);
    m_coverMaterial->diffuse()->addTextureImage(m_coverImage);
    m_coverMaterial->normal()->addTextureImage(m_coverNormalImage);

    updatePlantType();
    updatePotShape();

    m_coverImage->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/cover.webp")));
    m_coverNormalImage->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/cover_normal.webp")));
    m_potImage->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/pot.webp")));
    m_potNormalImage->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/pot_normal.webp")));

    m_potMaterial->setShininess(10.0f);
    m_potMaterial->setSpecular(QColor::fromRgbF(0.75f, 0.75f, 0.75f, 1.0f));

    m_plantMaterial->setShininess(10.0f);

    m_coverMaterial->setSpecular(QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f));
    m_coverMaterial->setShininess(5.0f);
}

HousePlant::~HousePlant()
{
}

void HousePlant::setPotShape(HousePlant::PotShape shape)
{
    if (shape != m_potShape) {
        m_potShape = shape;
        updatePotShape();
    }
}

void HousePlant::setPlantType(HousePlant::Plant plant)
{
    if (plant != m_plantType) {
        m_plantType = plant;
        updatePlantType();
    }
}

HousePlant::PotShape HousePlant::potShape() const
{
    return m_potShape;
}

HousePlant::Plant HousePlant::plantType() const
{
    return m_plantType;
}

void HousePlant::setX(float x)
{
    m_pot->translateTransform()->setDx(x);
}

void HousePlant::setY(float y)
{
    m_pot->translateTransform()->setDy(y);
}

void HousePlant::setZ(float z)
{
    m_pot->translateTransform()->setDz(z);
}

void HousePlant::setScale(float scale)
{
    m_pot->scaleTransform()->setScale(scale);
}

float HousePlant::x() const
{
    return m_pot->translateTransform()->dx();
}

float HousePlant::y() const
{
    return m_pot->translateTransform()->dy();
}

float HousePlant::z() const
{
    return m_pot->translateTransform()->dz();
}

float HousePlant::scale() const
{
    return m_pot->scaleTransform()->scale();
}

void HousePlant::updatePotShape()
{
    m_pot->mesh()->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/") + potNames[m_potShape] + QStringLiteral("-pot.obj")));
    m_plant->mesh()->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/") + potNames[m_potShape] + QStringLiteral("-") + plantNames[m_plantType] + QStringLiteral(".obj")));
}

void HousePlant::updatePlantType()
{
    m_plant->mesh()->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/") + potNames[m_potShape] + QStringLiteral("-") + plantNames[m_plantType] + QStringLiteral(".obj")));

    m_plantImage->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/") + plantNames[m_plantType] + QStringLiteral(".webp")));
    m_plantNormalImage->setSource(QUrl(QStringLiteral("qrc:/assets/houseplants/") + plantNames[m_plantType] + QStringLiteral("_normal.webp")));
}

