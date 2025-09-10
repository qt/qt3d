// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QDIFFUSESPECULARMAPMATERIAL_P_H
#define QT3DEXTRAS_QDIFFUSESPECULARMAPMATERIAL_P_H

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

class QDiffuseSpecularMapMaterial;

class QDiffuseSpecularMapMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QDiffuseSpecularMapMaterialPrivate();

    void init();

    void handleAmbientChanged(const QVariant &var);
    void handleDiffuseChanged(const QVariant &var);
    void handleSpecularChanged(const QVariant &var);
    void handleShininessChanged(const QVariant &var);
    void handleTextureScaleChanged(const QVariant &var);

    Qt3DRender::QEffect *m_diffuseSpecularMapEffect;
    Qt3DRender::QAbstractTexture *m_diffuseTexture;
    Qt3DRender::QAbstractTexture *m_specularTexture;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QParameter *m_textureScaleParameter;
    Qt3DRender::QTechnique *m_diffuseSpecularMapGL3Technique;
    Qt3DRender::QTechnique *m_diffuseSpecularMapGL2Technique;
    Qt3DRender::QTechnique *m_diffuseSpecularMapES2Technique;
    Qt3DRender::QTechnique *m_diffuseSpecularMapRHITechnique;
    Qt3DRender::QRenderPass *m_diffuseSpecularMapGL3RenderPass;
    Qt3DRender::QRenderPass *m_diffuseSpecularMapGL2RenderPass;
    Qt3DRender::QRenderPass *m_diffuseSpecularMapES2RenderPass;
    Qt3DRender::QRenderPass *m_diffuseSpecularMapRHIRenderPass;
    Qt3DRender::QShaderProgram *m_diffuseSpecularMapGL3Shader;
    Qt3DRender::QShaderProgramBuilder *m_diffuseSpecularMapGL3ShaderBuilder;
    Qt3DRender::QShaderProgram *m_diffuseSpecularMapGL2ES2Shader;
    Qt3DRender::QShaderProgramBuilder *m_diffuseSpecularMapGL2ES2ShaderBuilder;
    Qt3DRender::QShaderProgram *m_diffuseSpecularMapRHIShader;
    Qt3DRender::QShaderProgramBuilder *m_diffuseSpecularMapRHIShaderBuilder;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QDiffuseSpecularMapMaterial)
};

} // Qt3DExtras


QT_END_NAMESPACE

#endif // QT3DEXTRAS_QDIFFUSESPECULARMAPMATERIAL_P_H

