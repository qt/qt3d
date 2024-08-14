// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "scene3dsgmaterial_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::Scene3DSGMaterial
    \internal
    \inherits QSGMaterial

    \brief The Scene3DSGMaterial class is a custom QSGMaterial subclass used to
    render a Scene3DSGNode

    The Scene3DSGMaterial class renders a texture using premultiplied
    alpha unlike the QSGSimpleTextureMaterial.

    This is needed to properly integrate alpha blending from a Qt3D scene
    within a QtQuick 2 scene.
 */
Scene3DSGMaterial::Scene3DSGMaterial()
    : QSGMaterial()
    , m_texture(nullptr)
    , m_visible(false)
{
}

QSGMaterialShader *Scene3DSGMaterial::createShader(QSGRendererInterface::RenderMode renderMode) const
{
    Q_UNUSED(renderMode);
    return new Scene3DSGMaterialShader();
}

} // namespace Qt3DRender

QT_END_NAMESPACE
