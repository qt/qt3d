// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef SCENE_H
#define SCENE_H

#include <QtCore/QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QTexture>

class Scene : public QObject
{
    Q_OBJECT

public:
    explicit Scene(Qt3DCore::QEntity *rootEntity);
    ~Scene();

    void run();

private Q_SLOTS:
    void updateTimer();

private:
    Qt3DCore::QEntity *m_rootEntity;

    Qt3DCore::QEntity *m_cuboidEntity;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QTexture2D *m_texture;

    float m_angle = 0;
};

#endif // SCENE_H

