// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "examplescene.h"
#include "boxentity.h"

#include <QTimer>
#include <qmath.h>

ExampleScene::ExampleScene(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_timer(new QTimer(this))
    , m_even(true)
{
    buildScene();
    QObject::connect(m_timer, SIGNAL(timeout()), SLOT(updateScene()));
    m_timer->setInterval(1200);
    m_timer->start();
}

ExampleScene::~ExampleScene()
{
    qDeleteAll(m_entities);
}

void ExampleScene::updateScene()
{
    for (int i = 0; i < m_entities.size(); ++i) {
        const bool visible = (i % 2) ^ static_cast<int>(m_even);
        m_entities[i]->setParent(visible ? this : nullptr);
    }
    m_even = !m_even;
}

void ExampleScene::buildScene()
{
    int count = 20;
    const float radius = 5.0f;

    for (int i = 0; i < count; ++i) {
        BoxEntity *entity = new BoxEntity;
        const float angle = M_PI * 2.0f * float(i) / count;
        entity->setAngle(angle);
        entity->setRadius(radius);
        entity->setDiffuseColor(QColor(qFabs(qCos(angle)) * 255, 204, 75));
        m_entities.append(entity);
    }
}

