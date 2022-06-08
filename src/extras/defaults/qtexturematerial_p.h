// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTEXTUREMATERIAL_P_H
#define QT3DEXTRAS_QTEXTUREMATERIAL_P_H

#include <QMatrix3x3>

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
class QRenderPass;
class QNoDepthMask;
class QBlendEquationArguments;
class QBlendEquation;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QTextureMaterial;

class QTextureMaterialPrivate : public Qt3DRender::QMaterialPrivate
{
    QTextureMaterialPrivate();

    void init();

    void handleTextureChanged(const QVariant &var);
    void handleTextureTransformChanged(const QVariant &var);

    Qt3DRender::QEffect *m_textureEffect;
    Qt3DRender::QParameter *m_textureParameter;
    Qt3DRender::QParameter *m_textureTransformParameter;
    Qt3DRender::QTechnique *m_textureGL3Technique;
    Qt3DRender::QTechnique *m_textureGL2Technique;
    Qt3DRender::QTechnique *m_textureES2Technique;
    Qt3DRender::QTechnique *m_textureRHITechnique;
    Qt3DRender::QRenderPass *m_textureGL3RenderPass;
    Qt3DRender::QRenderPass *m_textureGL2RenderPass;
    Qt3DRender::QRenderPass *m_textureES2RenderPass;
    Qt3DRender::QRenderPass *m_textureRHIRenderPass;
    Qt3DRender::QShaderProgram *m_textureGL3Shader;
    Qt3DRender::QShaderProgram *m_textureGL2ES2Shader;
    Qt3DRender::QShaderProgram *m_textureRHIShader;
    Qt3DRender::QNoDepthMask *m_noDepthMask;
    Qt3DRender::QBlendEquationArguments *m_blendState;
    Qt3DRender::QBlendEquation *m_blendEquation;
    Qt3DRender::QFilterKey *m_filterKey;

public:
    Q_DECLARE_PUBLIC(QTextureMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXTUREMATERIAL_P_H

