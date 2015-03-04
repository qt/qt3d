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

#include "barrel.h"

const char *diffuseColorsName[] = {
    "red",
    "blue",
    "green",
    "rust",
    "stainless_steel"
};

const char *specularColorsName[] = {
    "_rust",
    "_stainless_steel",
    ""
};

const char *bumpsName[] = {
    "no_bumps",
    "soft_bumps",
    "middle_bumps",
    "hard_bumps"
};

Barrel::Barrel(Qt3D::QNode *parent)
    : RenderableEntity(parent)
    , m_bumps(NoBumps)
    , m_diffuseColor(Red)
    , m_specularColor(None)
    , m_material(new Qt3D::QNormalDiffuseSpecularMapMaterial())
    , m_diffuseTexture(m_material->diffuse())
    , m_normalTexture(m_material->normal())
    , m_specularTexture(m_material->specular())
    , m_diffuseTextureImage(new Qt3D::QTextureImage())
    , m_normalTextureImage(new Qt3D::QTextureImage())
    , m_specularTextureImage(new Qt3D::QTextureImage())
{
    mesh()->setSource(QUrl(QStringLiteral("qrc:/assets/metalbarrel/metal_barrel.obj")));
    scaleTransform()->setScale(0.03f);

    m_diffuseTexture->addTextureImage(m_diffuseTextureImage);
    m_normalTexture->addTextureImage(m_normalTextureImage);
    m_specularTexture->addTextureImage(m_specularTextureImage);

    setNormalTextureSource();
    setDiffuseTextureSource();
    setSpecularTextureSource();
    m_material->setShininess(10.0f);
    addComponent(m_material);
}

Barrel::~Barrel()
{
}

void Barrel::setDiffuse(Barrel::DiffuseColor diffuse)
{
    if (diffuse != m_diffuseColor) {
        m_diffuseColor = diffuse;
        setDiffuseTextureSource();
    }
}

void Barrel::setSpecular(Barrel::SpecularColor specular)
{
    if (specular != m_specularColor) {
        m_specularColor = specular;
        setSpecularTextureSource();
    }
}

void Barrel::setBumps(Barrel::Bumps bumps)
{
    if (bumps != m_bumps) {
        m_bumps = bumps;
        setNormalTextureSource();
    }
}

void Barrel::setShininess(float shininess)
{
    if (shininess != m_material->shininess())
        m_material->setShininess(shininess);
}

Barrel::DiffuseColor Barrel::diffuse() const
{
    return m_diffuseColor;
}

Barrel::SpecularColor Barrel::specular() const
{
    return m_specularColor;
}

Barrel::Bumps Barrel::bumps() const
{
    return m_bumps;
}

float Barrel::shininess() const
{
    return m_material->shininess();
}

void Barrel::setNormalTextureSource()
{
    m_normalTextureImage->setSource(QUrl(QStringLiteral("qrc:/assets/metalbarrel/normal_") + bumpsName[m_bumps] + QStringLiteral(".webp")));
}

void Barrel::setDiffuseTextureSource()
{
    m_diffuseTextureImage->setSource(QUrl(QStringLiteral("qrc:/assets/metalbarrel/diffus_") + diffuseColorsName[m_diffuseColor] + QStringLiteral(".webp")));
}

void Barrel::setSpecularTextureSource()
{
    m_specularTextureImage->setSource(QUrl(QStringLiteral("qrc:/assets/metalbarrel/specular") + specularColorsName[m_specularColor] + QStringLiteral(".webp")));
}
