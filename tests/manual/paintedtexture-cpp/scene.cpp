// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "scene.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>

#include <QtGui/QPainter>
#include <QtGui/qquaternion.h>

#include <Qt3DRender/QPaintedTextureImage>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QDiffuseMapMaterial>

class MyTextureImage : public Qt3DRender::QPaintedTextureImage
{
public:
    MyTextureImage(int w, int h)
    {
        setSize(QSize(w, h));
    }

protected:
    void paint(QPainter *painter) override
    {
        int w = painter->device()->width();
        int h = painter->device()->height();

        painter->setViewport(0, h, w, -h);

        // clear to white
        painter->fillRect(0, 0, w, h, QColor(255, 255, 255));

        // draw some text
        painter->setPen(QColor(255, 0, 0));
        painter->setFont(QFont("Times", 30, QFont::Normal, true));
        painter->drawText(w / 8, h / 6, QDateTime::currentDateTime().toString());

        painter->setPen(QPen(QBrush(QColor(0, 0, 0)) ,10));
        painter->setBrush(QColor(0, 0, 255));

        // draw some circles
        for (int i = 0; i < m_circleCount; i++) {
            float r = w / 8.f;
            float cw =  0.25 * w +   0.5 * h * (i % 2);
            float ch = 0.375 * w + 0.375 * h * (i / 2);
            painter->drawEllipse(QPointF(cw, ch), r, r);
        }

        m_circleCount = (m_circleCount % 4) + 1;
    }

    int m_circleCount = 1;
};

Scene::Scene(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity)
{
    // Cuboid shape data
    Qt3DExtras::QCuboidMesh *cuboid = new Qt3DExtras::QCuboidMesh();

    // CuboidMesh Transform
    m_transform = new Qt3DCore::QTransform();
    m_transform->setScale(4.0f);;

    // create texture
    m_paintedTextureImage = new MyTextureImage(128, 128);
    m_paintedTextureImage->update();

    Qt3DRender::QTexture2D *tex = new Qt3DRender::QTexture2D();
    tex->addTextureImage(m_paintedTextureImage);

    Qt3DExtras::QDiffuseMapMaterial *mat = new Qt3DExtras::QDiffuseMapMaterial();
    mat->setAmbient(QColor(64, 64, 64));
    mat->setSpecular(QColor(255, 255, 255));
    mat->setDiffuse(tex);

    //Cuboid
    m_cuboidEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_cuboidEntity->addComponent(cuboid);
    m_cuboidEntity->addComponent(mat);
    m_cuboidEntity->addComponent(m_transform);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Scene::updateTimer);
    timer->start(50);
}

Scene::~Scene()
{
}

void Scene::redrawTexture()
{
    m_paintedTextureImage->update();
}

void Scene::setSize(QSize size)
{
    m_paintedTextureImage->setSize(size);
}

void Scene::updateTimer()
{
    m_transform->setRotation(QQuaternion::fromAxisAndAngle(0, 1, 0, m_angle));
    m_angle += 0.4f;
}
