// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QGOOCHMATERIAL_P_H
#define QT3DEXTRAS_QGOOCHMATERIAL_P_H

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
class QRenderPass;
class QShaderProgram;
class QTechnique;

}

namespace Qt3DExtras {

class QGoochMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QGoochMaterialPrivate();

    void init();

    void handleDiffuseChanged(const QVariant &var);
    void handleSpecularChanged(const QVariant &var);
    void handleCoolChanged(const QVariant &var);
    void handleWarmChanged(const QVariant &var);
    void handleAlphaChanged(const QVariant &var);
    void handleBetaChanged(const QVariant &var);
    void handleShininessChanged(const QVariant &var);

    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QParameter *m_diffuseParameter;
    Qt3DRender::QParameter *m_specularParameter;
    Qt3DRender::QParameter *m_coolParameter;
    Qt3DRender::QParameter *m_warmParameter;
    Qt3DRender::QParameter *m_alphaParameter;
    Qt3DRender::QParameter *m_betaParameter;
    Qt3DRender::QParameter *m_shininessParameter;
    Qt3DRender::QTechnique *m_gl3Technique;
    Qt3DRender::QTechnique *m_gl2Technique;
    Qt3DRender::QTechnique *m_es2Technique;
    Qt3DRender::QTechnique *m_rhiTechnique;
    Qt3DRender::QRenderPass *m_gl3RenderPass;
    Qt3DRender::QRenderPass *m_gl2RenderPass;
    Qt3DRender::QRenderPass *m_es2RenderPass;
    Qt3DRender::QRenderPass *m_rhiRenderPass;
    Qt3DRender::QShaderProgram *m_gl3Shader;
    Qt3DRender::QShaderProgram *m_gl2ES2Shader;
    Qt3DRender::QShaderProgram *m_rhiShader;
    Qt3DRender::QFilterKey *m_filterKey;

    Q_DECLARE_PUBLIC(QGoochMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QGOOCHMATERIAL_P_H

