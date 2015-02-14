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

#include "examplescene.h"
#include "boxentity.h"

#include <QTimer>
#include <qmath.h>

ExampleScene::ExampleScene(Qt3D::QNode *parent)
    : Qt3D::QEntity(parent)
    , m_timer(new QTimer(this))
    , m_even(true)
{
    buildScene();

    QObject::connect(m_timer, SIGNAL(timeout()), SLOT(updateScene()));
    m_timer->setInterval(500);
    m_timer->start();
}

ExampleScene::~ExampleScene()
{
    qDeleteAll(m_entities);
}

void ExampleScene::updateScene()
{
    int i = 0;
    Q_FOREACH (BoxEntity *entity, m_entities) {
        if (i % 2 == 0)
            entity->setParent(m_even ? Q_NULLPTR : this);
        else
            entity->setParent(m_even ? this : Q_NULLPTR);
        ++i;
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

