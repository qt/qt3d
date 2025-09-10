// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSKYBOXENTITY_P_H
#define QT3DEXTRAS_QSKYBOXENTITY_P_H

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

#include <QtGui/QVector3D>

#include <Qt3DCore/private/qentity_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFilterKey;
class QTextureCubeMap;
class QTextureLoader;
class QShaderProgram;
class QSkyboxEntity;
class QTextureImage;
class QRenderPass;
class QTechnique;
class QParameter;
class QMaterial;
class QEffect;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QCuboidMesh;

class QSkyboxEntityPrivate : public Qt3DCore::QEntityPrivate
{
    QSkyboxEntityPrivate();

    void init();
    void reloadTexture();

    Q_DECLARE_PUBLIC(QSkyboxEntity)

    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QMaterial *m_material;
    Qt3DRender::QTextureCubeMap *m_skyboxTexture;
    Qt3DRender::QTextureLoader *m_loadedTexture;
    Qt3DRender::QShaderProgram *m_gl3Shader;
    Qt3DRender::QShaderProgram *m_gl2es2Shader;
    Qt3DRender::QShaderProgram *m_rhiShader;
    Qt3DRender::QTechnique *m_gl2Technique;
    Qt3DRender::QTechnique *m_es2Technique;
    Qt3DRender::QTechnique *m_gl3Technique;
    Qt3DRender::QTechnique *m_rhiTechnique;
    Qt3DRender::QFilterKey *m_filterKey;
    Qt3DRender::QRenderPass *m_gl2RenderPass;
    Qt3DRender::QRenderPass *m_es2RenderPass;
    Qt3DRender::QRenderPass *m_gl3RenderPass;
    Qt3DRender::QRenderPass *m_rhiRenderPass;
    QCuboidMesh *m_mesh;
    Qt3DRender::QParameter *m_gammaStrengthParameter;
    Qt3DRender::QParameter *m_textureParameter;
    Qt3DRender::QTextureImage *m_posXImage;
    Qt3DRender:: QTextureImage *m_posYImage;
    Qt3DRender::QTextureImage *m_posZImage;
    Qt3DRender::QTextureImage *m_negXImage;
    Qt3DRender::QTextureImage *m_negYImage;
    Qt3DRender::QTextureImage *m_negZImage;
    QString m_extension;
    QString m_baseName;
    QVector3D m_position;
    bool m_hasPendingReloadTextureCall;
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSKYBOXENTITY_P_H

