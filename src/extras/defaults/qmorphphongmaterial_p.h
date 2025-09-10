// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QMORPHPHONGMATERIAL_P_H
#define QT3DEXTRAS_QMORPHPHONGMATERIAL_P_H

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
class QRenderPass;
class QShaderProgramBuilder;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QMorphPhongMaterial;

class QMorphPhongMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QMorphPhongMaterialPrivate();

    void init();

    void handleAmbientChanged(const QVariant &var);
    void handleDiffuseChanged(const QVariant &var);
    void handleSpecularChanged(const QVariant &var);
    void handleShininessChanged(const QVariant &var);
    void handleInterpolatorChanged(const QVariant &var);

    Qt3DRender::QEffect *m_phongEffect;
    Qt3DRender::QParameter *m_ambientParameter;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QParameter *m_interpolatorParameter;
    Qt3DRender::QTechnique *m_phongGL3Technique;
    Qt3DRender::QTechnique *m_phongGL2Technique;
    Qt3DRender::QTechnique *m_phongES2Technique;
    Qt3DRender::QTechnique *m_phongRHITechnique;
    Qt3DRender::QRenderPass *m_phongGL3RenderPass;
    Qt3DRender::QRenderPass *m_phongGL2RenderPass;
    Qt3DRender::QRenderPass *m_phongES2RenderPass;
    Qt3DRender::QRenderPass *m_phongRHIRenderPass;
    Qt3DRender::QShaderProgram *m_phongGL3Shader;
    Qt3DRender::QShaderProgram *m_phongGL2ES2Shader;
    Qt3DRender::QShaderProgram *m_phongRHIShader;
    Qt3DRender::QShaderProgramBuilder *m_phongGL3ShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_phongGL2ES2ShaderBuilder;
    Qt3DRender::QShaderProgramBuilder *m_phongRHIShaderBuilder;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QMorphPhongMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPHONGMATERIAL_P_H


