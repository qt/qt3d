// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef POINTLIGHTBLOCK_H
#define POINTLIGHTBLOCK_H

#include <Qt3DRender/QAbstractLight>
#include <Qt3DRender/QShaderData>

class PointLightBlock : public Qt3DRender::QShaderData
{
    Q_OBJECT
    Q_PROPERTY(QList<Qt3DRender::QAbstractLight *> lights READ lights NOTIFY lightsChanged)
public:
    explicit PointLightBlock(Qt3DCore::QNode *parent = 0);
    ~PointLightBlock();

    QList<Qt3DRender::QAbstractLight *> lights() const;
    void addLight(Qt3DRender::QAbstractLight *light);

Q_SIGNALS:
    void lightsChanged();

private:
    QList<Qt3DRender::QAbstractLight *> m_lights;
};

#endif // POINTLIGHTBLOCK_H
