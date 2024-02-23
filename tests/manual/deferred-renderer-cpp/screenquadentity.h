// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef SCREENQUADENTITY_H
#define SCREENQUADENTITY_H

#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QLayer;
}

class FinalEffect;

class ScreenQuadEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit ScreenQuadEntity(Qt3DCore::QNode *parent = nullptr);

    Qt3DRender::QLayer *layer() const;
    FinalEffect *effect() const;

private:
    Qt3DRender::QLayer *m_layer;
    FinalEffect *m_effect;
};

QT_END_NAMESPACE

#endif // SCREENQUADENTITY_H
