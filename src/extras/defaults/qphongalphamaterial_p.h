// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QPHONGALPHAMATERIAL_P_H
#define QT3DEXTRAS_QPHONGALPHAMATERIAL_P_H

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

class QPhongAlphaMaterial;

class QPhongAlphaMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QPhongAlphaMaterialPrivate();

    void init();

    void handleAmbientChanged(const QVariant &var);
    void handleDiffuseChanged(const QVariant &var);
    void handleSpecularChanged(const QVariant &var);
    void handleShininessChanged(const QVariant &var);

    Qt3DRender::QEffect *m_phongEffect;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QTechnique *m_phongAlphaGL3Technique;
    Qt3DRender::QTechnique *m_phongAlphaGL2Technique;
    Qt3DRender::QTechnique *m_phongAlphaES2Technique;
    Qt3DRender::QTechnique *m_phongAlphaRHITechnique;
    Qt3DRender::QRenderPass *m_phongAlphaGL3RenderPass;
    Qt3DRender::QRenderPass *m_phongAlphaGL2RenderPass;
    Qt3DRender::QRenderPass *m_phongAlphaES2RenderPass;
    Qt3DRender::QRenderPass *m_phongAlphaRHIRenderPass;
    Qt3DRender::QShaderProgram *m_phongAlphaGL3Shader;
    Qt3DRender::QShaderProgramBuilder *m_phongAlphaGL3ShaderBuilder;
    Qt3DRender::QShaderProgram *m_phongAlphaGL2ES2Shader;
    Qt3DRender::QShaderProgramBuilder *m_phongAlphaGL2ES2ShaderBuilder;
    Qt3DRender::QShaderProgram *m_phongAlphaRHIShader;
    Qt3DRender::QShaderProgramBuilder *m_phongAlphaRHIShaderBuilder;
    Qt3DRender::QNoDepthMask *m_noDepthMask;
    Qt3DRender::QBlendEquationArguments *m_blendState;
    Qt3DRender::QBlendEquation *m_blendEquation;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QPhongAlphaMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPHONGALPHAMATERIAL_P_H

