// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "pointlightblock.h"

PointLightBlock::PointLightBlock(Qt3DCore::QNode *parent)
    : Qt3DRender::QShaderData(parent)
{

}

PointLightBlock::~PointLightBlock()
{
}

QList<Qt3DRender::QAbstractLight *> PointLightBlock::lights() const
{
    return m_lights;
}

void PointLightBlock::addLight(Qt3DRender::QAbstractLight *light)
{
    m_lights.append(light);
    emit lightsChanged();
}

