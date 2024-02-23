// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef SCENEEFFECT_H
#define SCENEEFFECT_H

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>

QT_BEGIN_NAMESPACE

class SceneEffect : public Qt3DRender::QEffect
{
public:
    explicit SceneEffect(Qt3DCore::QNode *parent = 0);

    QList<Qt3DRender::QFilterKey *> passCriteria() const;

private:
    Qt3DRender::QTechnique *m_gl3Technique;
    Qt3DRender::QTechnique *m_gl2Technique;
    Qt3DRender::QRenderPass *m_gl2Pass;
    Qt3DRender::QRenderPass *m_gl3Pass;
    Qt3DRender::QFilterKey *m_passCriterion;
};

QT_END_NAMESPACE

#endif // SCENEEFFECT_H
