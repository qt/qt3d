// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef SCENE_H
#define SCENE_H

#include <QtCore/QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPaintedTextureImage>

class Scene : public QObject
{
    Q_OBJECT

public:
    explicit Scene(Qt3DCore::QEntity *rootEntity);
    ~Scene();

public Q_SLOTS:
    void redrawTexture();
    void setSize(QSize size);

private Q_SLOTS:
    void updateTimer();

private:
    Qt3DCore::QEntity *m_rootEntity;

    Qt3DCore::QEntity *m_cuboidEntity;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QPaintedTextureImage *m_paintedTextureImage;

    float m_angle = 0;
};

#endif // SCENE_H

