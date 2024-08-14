// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>
#include <Qt3DQuickRender/private/qt3dquickrendernodefactory_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdata_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdataarray_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Quick {

static QVariantList Quick3DShaderDataArrayToVariantListConverter(Qt3DRender::Render::Quick::Quick3DShaderDataArray *array)
{
    const QList<Qt3DRender::QShaderData *> arrayValues = array->values();
    QVariantList values;
    values.reserve(arrayValues.size());
    for (Qt3DRender::QShaderData *data : arrayValues)
        values.append(QVariant::fromValue(data));
    return values;
}

void Quick3DRender_initialize()
{
    Qt3DCore::QAbstractNodeFactory::registerNodeFactory(QuickRenderNodeFactory::instance());

    // Converters
    QMetaType::registerConverter<Qt3DRender::Render::Quick::Quick3DShaderDataArray*, QVariantList>(Quick3DShaderDataArrayToVariantListConverter);

    // register types in the QuickRenderNodeFactory
    Qt3DRender::Quick::Quick3DRender_registerType("QSceneLoader", "Qt3D.Render/SceneLoader", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QEffect", "Qt3D.Render/Effect", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTechnique", "Qt3D.Render/Technique", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QMaterial", "Qt3D.Render/Material", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QRenderPass", "Qt3D.Render/RenderPass", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTexture1D", "Qt3D.Render/Texture1D", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTexture1DArray", "Qt3D.Render/Texture1DArray", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTexture2D", "Qt3D.Render/Texture2D", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTexture2DArray", "Qt3D.Render/Texture2DArray", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTexture3D", "Qt3D.Render/Texture3D", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTextureCubeMap", "Qt3D.Render/TextureCubeMap", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTextureCubeMapArray", "Qt3D.Render/TextureCubeMapArray", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTexture2DMultisample", "Qt3D.Render/Texture2DMultisample", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTexture2DMultisampleArray", "Qt3D.Render/Texture2DMultisampleArray", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTextureRectangle", "Qt3D.Render/TextureRectangle", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTextureBuffer", "Qt3D.Render/TextureBuffer", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTextureLoader", "Qt3D.Render/TextureLoader", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QLayerFilter", "Qt3D.Render/LayerFilter", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QCameraSelector", "Qt3D.Render/CameraSelector", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QRenderPassFilter", "Qt3D.Render/RenderPassFilter", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QTechniqueFilter", "Qt3D.Render/TechniqueFilter", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QViewport", "Qt3D.Render/Viewport", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QRenderStateSet", "Qt3D.Render/RenderStateSet", 2, 0);
    Qt3DRender::Quick::Quick3DRender_registerType("QMemoryBarrier", "Qt3D.Render/MemoryBarrier", 2, 9);
    Qt3DRender::Quick::Quick3DRender_registerType("QRenderTarget", "Qt3D.Render/RenderTarget", 2, 0);
}

void Quick3DRender_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickRenderNodeFactory::instance()->registerType(className, quickName, major, minor);
}


Q_CONSTRUCTOR_FUNCTION(Quick3DRender_initialize)

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE
