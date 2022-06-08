// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QDIFFUSEMAPMATERIAL_P_H
#define QT3DEXTRAS_QDIFFUSEMAPMATERIAL_P_H

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

class QDiffuseMapMaterial;

class QDiffuseMapMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
    QDiffuseMapMaterialPrivate();

    void init();

    void handleAmbientChanged(const QVariant &var);
    void handleDiffuseChanged(const QVariant &var);
    void handleSpecularChanged(const QVariant &var);
    void handleShininessChanged(const QVariant &var);
    void handleTextureScaleChanged(const QVariant &var);

    Qt3DRender::QEffect *m_diffuseMapEffect;
    Qt3DRender::QAbstractTexture *m_diffuseTexture;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QParameter *m_textureScaleParameter;
    Qt3DRender::QTechnique *m_diffuseMapGL3Technique;
    Qt3DRender::QTechnique *m_diffuseMapGL2Technique;
    Qt3DRender::QTechnique *m_diffuseMapES2Technique;
    Qt3DRender::QTechnique *m_diffuseMapRHITechnique;
    Qt3DRender::QRenderPass *m_diffuseMapGL3RenderPass;
    Qt3DRender::QRenderPass *m_diffuseMapGL2RenderPass;
    Qt3DRender::QRenderPass *m_diffuseMapES2RenderPass;
    Qt3DRender::QRenderPass *m_diffuseMapRHIRenderPass;
    Qt3DRender::QShaderProgram *m_diffuseMapGL3Shader;
    Qt3DRender::QShaderProgramBuilder *m_diffuseMapGL3ShaderBuilder;
    Qt3DRender::QShaderProgram *m_diffuseMapGL2ES2Shader;
    Qt3DRender::QShaderProgramBuilder *m_diffuseMapGL2ES2ShaderBuilder;
    Qt3DRender::QShaderProgram *m_diffuseMapRHIShader;
    Qt3DRender::QShaderProgramBuilder *m_diffuseMapRHIShaderBuilder;
    Qt3DRender::QFilterKey *m_filterKey;

public:
    Q_DECLARE_PUBLIC(QDiffuseMapMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QDIFFUSEAPMATERIAL_P_H

