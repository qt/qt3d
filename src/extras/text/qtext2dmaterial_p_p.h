// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTEXT2DMATERIAL_P_P_H
#define QT3DEXTRAS_QTEXT2DMATERIAL_P_P_H

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
class QAbstractTexture;
class QEffect;
class QTechnique;
class QParameter;
class QRenderPass;
class QShaderProgram;
class QBlendEquation;
class QBlendEquationArguments;
class QDepthTest;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QText2DMaterial;

class QText2DMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
public:
    QText2DMaterialPrivate();

    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QAbstractTexture *m_distanceFieldTexture;
    Qt3DRender::QParameter *m_textureParameter;
    Qt3DRender::QParameter *m_textureSizeParameter;
    Qt3DRender::QParameter *m_colorParameter;
    Qt3DRender::QTechnique *m_gl3Technique;
    Qt3DRender::QTechnique *m_gl2Technique;
    Qt3DRender::QTechnique *m_es2Technique;
    Qt3DRender::QTechnique *m_rhiTechnique;
    Qt3DRender::QRenderPass *m_gl3RenderPass;
    Qt3DRender::QRenderPass *m_gl2RenderPass;
    Qt3DRender::QRenderPass *m_es2RenderPass;
    Qt3DRender::QRenderPass *m_rhiRenderPass;
    Qt3DRender::QShaderProgram *m_rhiShaderProgram;
    Qt3DRender::QShaderProgram *m_gl3ShaderProgram;
    Qt3DRender::QShaderProgram *m_gl2es2ShaderProgram;
    Qt3DRender::QBlendEquation *m_blend;
    Qt3DRender::QBlendEquationArguments *m_blendArgs;
    Qt3DRender::QDepthTest *m_depthTest;

    void init();

    Q_DECLARE_PUBLIC(QText2DMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXT2DMATERIAL_P_P_H
