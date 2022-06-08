// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef SCENEENTITY_H
#define SCENEENTITY_H

#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
class QCamera;
}
class SceneEffect;

class SceneEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit SceneEntity(Qt3DCore::QNode *parent = nullptr);

    Qt3DRender::QLayer *layer() const;
    SceneEffect *effect() const;
private:
    Qt3DRender::QLayer *m_layer;
    SceneEffect *m_effect;
};

QT_END_NAMESPACE

#endif // SCENEENTITY_H
