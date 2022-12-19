// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef SCENE3DSGMATERIAL_H
#define SCENE3DSGMATERIAL_H

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

#include <QtQuick/QSGMaterial>
#include <QtQuick/qsgtexture.h>

#include <scene3dsgmaterialshader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Scene3DSGMaterial : public QSGMaterial
{
public:
    Scene3DSGMaterial();

    void setTexture(QSGTexture *texture)
    {
        m_texture = texture;
        setFlag(Blending, m_texture ? m_texture->hasAlphaChannel() : false);
    }

    QSGTexture *texture() const noexcept { return m_texture; }
    QSGMaterialType *type() const final { return &Scene3DSGMaterialShader::type; }
    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode renderMode) const final;

    void show() { m_visible = true; }
    bool visible() const { return m_visible; }

private:
    QSGTexture *m_texture;
    bool m_visible;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // SCENE3DSGMATERIAL_H
