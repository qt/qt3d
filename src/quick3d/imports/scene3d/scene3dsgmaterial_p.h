/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode renderMode) const final;
#else
    QSGMaterialShader *createShader() const final;
#endif

    void show() { m_visible = true; }
    bool visible() const { return m_visible; }

private:
    QSGTexture *m_texture;
    bool m_visible;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // SCENE3DSGMATERIAL_H
