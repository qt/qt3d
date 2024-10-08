// Copyright (C) 2021 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_SCENE3DSGMATERIALSHADER_P_H
#define QT3DRENDER_SCENE3DSGMATERIALSHADER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQuick/QSGMaterialShader>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_AUTOTEST_EXPORT Scene3DSGMaterialShader : public QSGMaterialShader
{
public:
    Scene3DSGMaterialShader();
    static QSGMaterialType type;

protected:
    // QSGMaterialShader interface
    bool updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) final;
    void updateSampledImage(RenderState &state, int binding, QSGTexture **texture, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) final;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_SCENE3DSGMATERIALSHADER_P_H
