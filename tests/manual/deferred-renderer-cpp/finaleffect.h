// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef FINALEFFECT_H
#define FINALEFFECT_H

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>

QT_BEGIN_NAMESPACE

class FinalEffect : public Qt3DRender::QEffect
{
public:
    explicit FinalEffect(Qt3DCore::QNode *parent = 0);

    QList<Qt3DRender::QFilterKey *> passCriteria() const;
    inline Qt3DRender::QTechnique *gl3Technique() const { return m_gl3Technique; }
    inline Qt3DRender::QTechnique *gl2Technique() const { return m_gl2Technique; }

private :
    Qt3DRender::QTechnique *m_gl3Technique;
    Qt3DRender::QTechnique *m_gl2Technique;
    Qt3DRender::QRenderPass *m_gl2Pass;
    Qt3DRender::QRenderPass *m_gl3Pass;
    Qt3DRender::QFilterKey *m_passCriterion;
};

QT_END_NAMESPACE

#endif // FINALEFFECT_H
