// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QNORMALDIFFUSESPECULARMAPMATERIAL_P_H
#define QT3DEXTRAS_QNORMALDIFFUSESPECULARMAPMATERIAL_P_H

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
class QAbstractTexture;
class QTechnique;
class QParameter;
class QShaderProgram;
class QShaderProgramBuilder;
class QRenderPass;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QNormalDiffuseSpecularMapMaterial;

class QNormalDiffuseSpecularMapMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QNormalDiffuseSpecularMapMaterialPrivate();

    void init();

    void handleAmbientChanged(const QVariant &var);
    void handleDiffuseChanged(const QVariant &var);
    void handleNormalChanged(const QVariant &var);
    void handleSpecularChanged(const QVariant &var);
    void handleShininessChanged(const QVariant &var);
    void handleTextureScaleChanged(const QVariant &var);

    Qt3DRender::QEffect *m_normalDiffuseSpecularEffect;
    Qt3DRender::QAbstractTexture *m_diffuseTexture;
    Qt3DRender::QAbstractTexture *m_normalTexture;
    Qt3DRender::QAbstractTexture *m_specularTexture;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_normalParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QParameter *m_textureScaleParameter;
    Qt3DRender::QTechnique *m_normalDiffuseSpecularGL3Technique;
    Qt3DRender::QTechnique *m_normalDiffuseSpecularGL2Technique;
    Qt3DRender::QTechnique *m_normalDiffuseSpecularES2Technique;
    Qt3DRender::QTechnique *m_normalDiffuseSpecularRHITechnique;
    Qt3DRender::QRenderPass *m_normalDiffuseSpecularGL3RenderPass;
    Qt3DRender::QRenderPass *m_normalDiffuseSpecularGL2RenderPass;
    Qt3DRender::QRenderPass *m_normalDiffuseSpecularES2RenderPass;
    Qt3DRender::QRenderPass *m_normalDiffuseSpecularRHIRenderPass;
    Qt3DRender::QShaderProgram *m_normalDiffuseSpecularGL3Shader;
    Qt3DRender::QShaderProgramBuilder *m_normalDiffuseSpecularGL3ShaderBuilder;
    Qt3DRender::QShaderProgram *m_normalDiffuseSpecularGL2ES2Shader;
    Qt3DRender::QShaderProgramBuilder *m_normalDiffuseSpecularGL2ES2ShaderBuilder;
    Qt3DRender::QShaderProgram *m_normalDiffuseSpecularRHIShader;
    Qt3DRender::QShaderProgramBuilder *m_normalDiffuseSpecularRHIShaderBuilder;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QNormalDiffuseSpecularMapMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QNORMALDIFFUSESPECULARMAPMATERIAL_P_H

