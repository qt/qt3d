// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QDIFFUSESPECULARMATERIAL_P_H
#define QT3DEXTRAS_QDIFFUSESPECULARMATERIAL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qmaterial_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFilterKey;
class QEffect;
class QTechnique;
class QParameter;
class QShaderProgram;
class QShaderProgramBuilder;
class QRenderPass;
class QNoDepthMask;
class QBlendEquationArguments;
class QBlendEquation;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QDiffuseSpecularMaterial;

class QDiffuseSpecularMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QDiffuseSpecularMaterialPrivate();

    void init();

    void handleAmbientChanged(const QVariant &var);
    void handleShininessChanged(const QVariant &var);
    void handleTextureScaleChanged(const QVariant &var);

    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_diffuseTextureParameter;
    Qt3DRender::QParameter *m_specularTextureParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QParameter *m_normalTextureParameter;
    Qt3DRender::QParameter *m_textureScaleParameter;
    Qt3DRender::QTechnique *m_gl3Technique;
    Qt3DRender::QTechnique *m_gl2Technique;
    Qt3DRender::QTechnique *m_es2Technique;
    Qt3DRender::QTechnique *m_rhiTechnique;
    Qt3DRender::QRenderPass *m_gl3RenderPass;
    Qt3DRender::QRenderPass *m_gl2RenderPass;
    Qt3DRender::QRenderPass *m_es2RenderPass;
    Qt3DRender::QRenderPass *m_rhiRenderPass;
    Qt3DRender::QShaderProgram *m_gl3Shader;
    Qt3DRender::QShaderProgramBuilder *m_gl3ShaderBuilder;
    Qt3DRender::QShaderProgram *m_gl2es2Shader;
    Qt3DRender::QShaderProgramBuilder *m_gl2es2ShaderBuilder;
    Qt3DRender::QShaderProgram *m_rhiShader;
    Qt3DRender::QShaderProgramBuilder *m_rhiShaderBuilder;
    Qt3DRender::QNoDepthMask *m_noDepthMask;
    Qt3DRender::QBlendEquationArguments *m_blendState;
    Qt3DRender::QBlendEquation *m_blendEquation;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QDiffuseSpecularMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QDIFFUSESPECULARMATERIAL_P_H

