// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QMETALROUGHMATERIAL_P_H
#define QT3DEXTRAS_QMETALROUGHMATERIAL_P_H

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

class QMetalRoughMaterial;

class QMetalRoughMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QMetalRoughMaterialPrivate();

    void init();

    void handleTextureScaleChanged(const QVariant &var);
    void updateLayersOnTechnique(const QStringList &layers);
    void updateVertexShaderBasedOnLayers(const QStringList &layers);

    Qt3DRender::QParameter *m_baseColorParameter;
    Qt3DRender::QParameter *m_metalnessParameter;
    Qt3DRender::QParameter *m_roughnessParameter;
    Qt3DRender::QParameter *m_baseColorMapParameter;
    Qt3DRender::QParameter *m_metalnessMapParameter;
    Qt3DRender::QParameter *m_roughnessMapParameter;
    Qt3DRender::QParameter *m_ambientOcclusionMapParameter;
    Qt3DRender::QParameter *m_normalMapParameter;
    Qt3DRender::QParameter *m_textureScaleParameter;
    Qt3DRender::QEffect *m_metalRoughEffect;
    Qt3DRender::QTechnique *m_metalRoughGL3Technique;
    Qt3DRender::QRenderPass *m_metalRoughGL3RenderPass;
    Qt3DRender::QShaderProgram *m_metalRoughGL3Shader;
    Qt3DRender::QShaderProgramBuilder *m_metalRoughGL3ShaderBuilder;
    Qt3DRender::QTechnique *m_metalRoughES3Technique;
    Qt3DRender::QRenderPass *m_metalRoughES3RenderPass;
    Qt3DRender::QShaderProgram *m_metalRoughES3Shader;
    Qt3DRender::QShaderProgramBuilder *m_metalRoughES3ShaderBuilder;
    Qt3DRender::QTechnique *m_metalRoughRHITechnique;
    Qt3DRender::QRenderPass *m_metalRoughRHIRenderPass;
    Qt3DRender::QShaderProgram *m_metalRoughRHIShader;
    Qt3DRender::QShaderProgramBuilder *m_metalRoughRHIShaderBuilder;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QMetalRoughMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QMETALROUGHMATERIAL_P_H

