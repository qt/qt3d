// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef PLANEMATERIAL_H
#define PLANEMATERIAL_H

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QMaterial>

class PlaneMaterial : public Qt3DRender::QMaterial
{
public:
    explicit PlaneMaterial(Qt3DRender::QAbstractTexture *texture, Qt3DCore::QNode *parent = nullptr);
private:
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QAbstractTexture *m_texture;
    Qt3DRender::QParameter *m_textureParam, *m_texCoordScaleParam, *m_texCoordBiasParam;
    Qt3DRender::QFilterKey *m_filter;
    Qt3DRender::QTechnique *m_techniqueGLES, *m_techniqueGL3, *m_techniqueGL2;
    Qt3DRender::QShaderProgram *m_programGLES, *m_programGL3;
    Qt3DRender::QRenderPass *m_renderPassGLES, *m_renderPassGL3, *m_renderPassGL2;
};

#endif // PLANEMATERIAL_H
