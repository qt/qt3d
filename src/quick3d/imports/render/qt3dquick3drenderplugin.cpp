/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qt3dquick3drenderplugin.h"
#include <Qt3DRender/qabstractsceneloader.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qframegraph.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qannotation.h>
#include <Qt3DRender/qparametermapping.h>
#include <Qt3DRender/qcuboidmesh.h>
#include <Qt3DRender/qcylindermesh.h>
#include <Qt3DRender/qplanemesh.h>
#include <Qt3DRender/qspheremesh.h>
#include <Qt3DRender/qtorusmesh.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qlight.h>
#include <Qt3DRender/qspotlight.h>
#include <Qt3DRender/qdirectionallight.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qblendstate.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qalphatest.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qdepthmask.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qfrontface.h>
#include <Qt3DRender/qstenciltest.h>
#include <Qt3DRender/qstenciltestseparate.h>
#include <Qt3DRender/qscissortest.h>
#include <Qt3DRender/qdithering.h>
#include <Qt3DRender/qrenderattachment.h>
#include <Qt3DRender/qclearbuffer.h>
#include <Qt3DRender/qsortcriterion.h>
#include <Qt3DRender/qalphacoverage.h>
#include <Qt3DRender/qpointsize.h>
#include <Qt3DRender/qpolygonoffset.h>
#include <Qt3DRender/qcolormask.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qclipplane.h>
#include <Qt3DRender/qstencilop.h>
#include <Qt3DRender/qstencilopseparate.h>
#include <Qt3DRender/qstencilmask.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qtorusgeometry.h>
#include <Qt3DRender/qspheregeometry.h>
#include <Qt3DRender/qcuboidgeometry.h>
#include <Qt3DRender/qplanegeometry.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qcylindergeometry.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qboundingvolumespecifier.h>
#include <Qt3DRender/qboundingvolumedebug.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qlighting.h>
#include <Qt3DRender/qdispatchcompute.h>
#include <Qt3DQuickRender/private/quick3dtechnique_p.h>
#include <Qt3DQuickRender/private/quick3dmaterial_p.h>
#include <Qt3DQuickRender/private/quick3dtechniquefilter_p.h>
#include <Qt3DQuickRender/private/quick3dviewport_p.h>
#include <Qt3DQuickRender/private/quick3drenderpassfilter_p.h>
#include <Qt3DQuickRender/private/quick3drendertarget_p.h>
#include <Qt3DQuickRender/private/quick3deffect_p.h>
#include <Qt3DQuickRender/private/quick3dscene_p.h>
#include <Qt3DQuickRender/private/quick3dtexture_p.h>
#include <Qt3DQuickRender/private/quick3drenderpass_p.h>
#include <Qt3DQuickRender/private/quick3dsortmethod_p.h>
#include <Qt3DQuickRender/private/quick3dparameter_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdata_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdataarray_p.h>
#include <Qt3DQuickRender/private/quick3dstateset_p.h>
#include <Qt3DQuickRender/private/quick3drendertargetselector_p.h>
#include <Qt3DQuickRender/private/quick3dgeometry_p.h>
#include <Qt3DQuickRender/private/quick3dbuffer_p.h>
#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>

static void initResources()
{
    Q_INIT_RESOURCE(defaults);
}

QT_BEGIN_NAMESPACE

static const struct {
    const char *type;
    int major, minor;
} qmldir [] = {
    // Materials
    { "PhongMaterial", 2, 0 },
    { "PhongAlphaMaterial", 2, 0 },
    { "DiffuseMapMaterial", 2, 0 },
    { "DiffuseSpecularMapMaterial", 2, 0 },
    { "NormalDiffuseMapAlphaMaterial", 2, 0 },
    { "NormalDiffuseMapMaterial", 2, 0 },
    { "NormalDiffuseSpecularMapMaterial", 2, 0 },
    { "PerVertexColorMaterial", 2, 0 },
    { "GoochMaterial", 2, 0 },
    { "TextureMaterial", 2, 0 },
    // FrameGraphs
    { "ForwardRenderer", 2, 0 },
    // Entities
    { "SkyboxEntity", 2, 0 }
};

QVariantList Quick3DShaderDataArrayToVariantListConverter(Qt3DRender::Render::Quick::Quick3DShaderDataArray *array)
{
    QList<Qt3DRender::QShaderData *> arrayValues = array->values();
    QVariantList values;
    values.reserve(arrayValues.size());
    Q_FOREACH (Qt3DRender::QShaderData *data, arrayValues)
        values.append(QVariant::fromValue(data));
    return values;
}

void Qt3DQuick3DRenderPlugin::registerTypes(const char *uri)
{
    // Init resources for defaults QML files
    initResources();

    Qt3DRender::Quick::Quick3DRender_initialize();

    // Converters
    QMetaType::registerConverter<Qt3DRender::Render::Quick::Quick3DShaderDataArray*, QVariantList>(Quick3DShaderDataArrayToVariantListConverter);

    // @uri Qt3D.Render
    qmlRegisterUncreatableType<Qt3DRender::QAbstractSceneLoader>(uri, 2, 0, "QAbstractSceneLoader", "QAbstractScene is abstract");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QSceneLoader, Qt3DRender::Render::Quick::Quick3DScene>("QSceneLoader", "Qt3D.Render/SceneLoader", uri, 2, 0, "SceneLoader");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QEffect, Qt3DRender::Render::Quick::Quick3DEffect>("QEffect", "Qt3D.Render/Effect", uri, 2, 0, "Effect");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTechnique, Qt3DRender::Render::Quick::Quick3DTechnique>("QTechnique", "Qt3D.Render/Technique", uri, 2, 0, "Technique");
    qmlRegisterType<Qt3DRender::QAnnotation>(uri, 2, 0, "Annotation");
    qmlRegisterType<Qt3DRender::QGraphicsApiFilter>(uri, 2, 0, "GraphicsApiFilter");
    qmlRegisterUncreatableType<Qt3DRender::QParameter>(uri, 2, 0, "QParameter", "Quick3D should instantiate Quick3DParameter only");
    qmlRegisterType<Qt3DRender::Render::Quick::Quick3DParameter>(uri, 2, 0, "Parameter");
    qmlRegisterType<Qt3DRender::QParameterMapping>(uri, 2, 0, "ParameterMapping");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QMaterial, Qt3DRender::Render::Quick::Quick3DMaterial>("QMaterial", "Qt3D.Render/Material", uri, 2, 0, "Material");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderPass, Qt3DRender::Render::Quick::Quick3DRenderPass>("QRenderPass", "Qt3D.Render/RenderPass", uri, 2, 0, "RenderPass");
    qmlRegisterType<Qt3DRender::QShaderProgram>(uri, 2, 0, "ShaderProgram");
    qmlRegisterUncreatableType<Qt3DRender::QShaderData>(uri, 2, 0, "QShaderData", "Quick3D should instantiate Quick3DShaderData only");
    qmlRegisterType<Qt3DRender::Render::Quick::Quick3DShaderDataArray>(uri, 2, 0, "ShaderDataArray");
    qmlRegisterType<Qt3DRender::Render::Quick::Quick3DShaderData>(uri, 2, 0, "ShaderData");

    // Textures
    qmlRegisterType<Qt3DRender::QTextureWrapMode>(uri, 2, 0, "WrapMode");//, QStringLiteral("QTextureWrapMode cannot be created from QML"));
    qmlRegisterUncreatableType<Qt3DRender::QAbstractTextureProvider>(uri, 2, 0, "Texture", QStringLiteral("Texture should be created from one of the subclasses"));
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture1D, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture1D", "Qt3D.Render/Texture1D", uri, 2, 0, "Texture1D");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture1DArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture1DArray", "Qt3D.Render/Texture1DArray", uri, 2, 0, "Texture1DArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2D, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2D", "Qt3D.Render/Texture2D", uri, 2, 0, "Texture2D");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2DArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2DArray", "Qt3D.Render/Texture2DArray", uri, 2, 0, "Texture2DArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture3D, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture3D", "Qt3D.Render/Texture3D", uri, 2, 0, "Texture3D");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureCubeMap, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureCubeMap", "Qt3D.Render/TextureCubeMap", uri, 2, 0, "TextureCubeMap");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureCubeMapArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureCubeMapArray", "Qt3D.Render/TextureCubeMapArray", uri, 2, 0, "TextureCubeMapArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2DMultisample, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2DMultisample", "Qt3D.Render/Texture2DMultisample", uri, 2, 0, "Texture2DMultisample");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2DMultisampleArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2DMultisampleArray", "Qt3D.Render/Texture2DMultisampleArray", uri, 2, 0, "Texture2DMultisampleArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureRectangle, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureRectangle", "Qt3D.Render/TextureRectangle", uri, 2, 0, "TextureRectangle");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureBuffer, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureBuffer", "Qt3D.Render/TextureBuffer", uri, 2, 0, "TextureBuffer");
    qmlRegisterUncreatableType<Qt3DRender::QAbstractTextureImage>(uri, 2, 0, "QAbstractTextureImage", QStringLiteral("QAbstractTextureImage is abstract"));
    qmlRegisterType<Qt3DRender::QTextureImage>(uri, 2, 0, "TextureImage");

    // Geometry
    qmlRegisterUncreatableType<Qt3DRender::QAbstractAttribute>(uri, 2, 0, "QAbstractAttribute", QStringLiteral("QAbstractAttribute is abstract"));
    qmlRegisterUncreatableType<Qt3DRender::QAbstractBuffer>(uri, 2, 0, "QAbstractBuffer", QStringLiteral("QAbstractBuffer is abstract"));
    qmlRegisterType<Qt3DRender::QAttribute>(uri, 2, 0, "Attribute");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QBuffer, Qt3DRender::Render::Quick::Quick3DBuffer>("QBuffer", "Qt3D.Render/Buffer", uri, 2, 0, "Buffer");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QGeometry, Qt3DRender::Render::Quick::Quick3DGeometry>("QGeometry", "Qt3D.Render/Geometry", uri, 2, 0, "Geometry");
    qmlRegisterType<Qt3DRender::QGeometryRenderer>(uri, 2, 0, "GeometryRenderer");
    qmlRegisterUncreatableType<Qt3DRender::QBoundingVolumeSpecifier>(uri, 2, 0, "QBoundingVolumeSpecifier", QStringLiteral("QBoundingVolumeSpecifier is used as a grouped property"));

    // Debug components
    qmlRegisterType<Qt3DRender::QBoundingVolumeDebug>(uri, 2, 0, "BoundingVolumeDebug");

    // Picking
    qmlRegisterType<Qt3DRender::QObjectPicker>(uri, 2, 0, "ObjectPicker");

    // Meshes
    qmlRegisterType<Qt3DRender::QMesh>(uri, 2, 0, "Mesh");
    qmlRegisterType<Qt3DRender::QCuboidMesh>(uri, 2, 0, "CuboidMesh");
    qmlRegisterType<Qt3DRender::QCuboidGeometry>(uri, 2, 0, "CuboidGeometry");
    qmlRegisterType<Qt3DRender::QCylinderMesh>(uri, 2, 0, "CylinderMesh");
    qmlRegisterType<Qt3DRender::QCylinderGeometry>(uri, 2, 0, "CylinderGeometry");
    qmlRegisterType<Qt3DRender::QPlaneMesh>(uri, 2, 0, "PlaneMesh");
    qmlRegisterType<Qt3DRender::QPlaneGeometry>(uri, 2, 0, "PlaneGeometry");
    qmlRegisterType<Qt3DRender::QTorusMesh>(uri, 2, 0, "TorusMesh");
    qmlRegisterType<Qt3DRender::QTorusGeometry>(uri, 2, 0, "TorusGeometry");
    qmlRegisterType<Qt3DRender::QSphereMesh>(uri, 2, 0, "SphereMesh");
    qmlRegisterType<Qt3DRender::QSphereGeometry>(uri, 2, 0, "SphereGeometry");

    // Layers
    qmlRegisterType<Qt3DRender::QLayer>(uri, 2, 0, "Layer");
    qmlRegisterType<Qt3DRender::QLayerFilter>(uri, 2, 0, "LayerFilter");

    // Lights
    qmlRegisterType<Qt3DRender::QLight>(uri, 2, 0, "Light");
    qmlRegisterType<Qt3DRender::QPointLight>(uri, 2, 0, "PointLight");
    qmlRegisterType<Qt3DRender::QDirectionalLight>(uri, 2, 0, "DirectionalLight");
    qmlRegisterType<Qt3DRender::QSpotLight>(uri, 2, 0, "SpotLight");

    // FrameGraph
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QCameraSelector, Qt3DCore::Quick::Quick3DNode>("QCameraSelector", "Qt3D.Render/CameraSelector", uri, 2, 0, "CameraSelector");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderPassFilter, Qt3DRender::Render::Quick::Quick3DRenderPassFilter>("QRenderPassFilter", "Qt3D.Render/RenderPassFilter", uri, 2, 0, "RenderPassFilter");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTechniqueFilter, Qt3DRender::Render::Quick::Quick3DTechniqueFilter>("QTechniqueFilter", "Qt3D.Render/TechniqueFilter", uri, 2, 0, "TechniqueFilter");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QViewport, Qt3DRender::Render::Quick::Quick3DViewport>("QViewport", "Qt3D.Render/Viewport", uri, 2, 0, "Viewport");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderTargetSelector, Qt3DRender::Render::Quick::Quick3DRenderTargetSelector>("QRenderTargetSelector", "Qt3D.Render/RenderTargetSelector", uri, 2, 0, "RenderTargetSelector");
    qmlRegisterType<Qt3DRender::QClearBuffer>(uri, 2, 0, "ClearBuffer");
    qmlRegisterUncreatableType<Qt3DRender::QFrameGraphNode>(uri, 2, 0, "FrameGraphNode", QStringLiteral("FrameGraphNode is a base class"));
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QStateSet, Qt3DRender::Render::Quick::Quick3DStateSet>("QStateSet", "Qt3D.Render/StateSet", uri, 2, 0, "StateSet");
    qmlRegisterType<Qt3DRender::QNoDraw>(uri, 2, 0, "NoDraw");
    qmlRegisterType<Qt3DRender::QFrameGraph>(uri, 2, 0, "FrameGraph");
    qmlRegisterType<Qt3DRender::QFrustumCulling>(uri, 2, 0, "FrustumCulling");
    qmlRegisterType<Qt3DRender::QLighting>(uri, 2, 0, "Lighting");
    qmlRegisterType<Qt3DRender::QDispatchCompute>(uri, 2, 0, "DispatchCompute");

    // RenderTarget
    qmlRegisterType<Qt3DRender::QRenderAttachment>(uri, 2, 0, "RenderAttachment");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderTarget, Qt3DRender::Render::Quick::Quick3DRenderTarget>("QRenderTarget", "Qt3D.Render/RenderTarget", uri, 2, 0, "RenderTarget");

    // Sorting
    qmlRegisterType<Qt3DRender::QSortCriterion>(uri, 2, 0, "SortCriterion");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QSortMethod, Qt3DRender::Render::Quick::Quick3DSortMethod>("QSortMethod", "Qt3D.Render/SortMethod", uri, 2, 0, "SortMethod");

    // RenderStates
    qmlRegisterUncreatableType<Qt3DRender::QRenderState>(uri, 2, 0, "RenderState", QStringLiteral("QRenderState is a base class"));
    qmlRegisterType<Qt3DRender::QBlendState>(uri, 2, 0, "BlendState");
    qmlRegisterType<Qt3DRender::QBlendStateSeparate>(uri, 2, 0, "BlendStateSeparate");
    qmlRegisterType<Qt3DRender::QBlendEquation>(uri, 2, 0, "BlendEquation");
    qmlRegisterType<Qt3DRender::QAlphaTest>(uri, 2, 0, "AlphaTest");
    qmlRegisterType<Qt3DRender::QDepthTest>(uri, 2, 0, "DepthTest");
    qmlRegisterType<Qt3DRender::QDepthMask>(uri, 2, 0, "DepthMask");
    qmlRegisterType<Qt3DRender::QCullFace>(uri, 2, 0, "CullFace");
    qmlRegisterType<Qt3DRender::QFrontFace>(uri, 2, 0, "FrontFace");
    qmlRegisterUncreatableType<Qt3DRender::QStencilTestSeparate>(uri, 2, 0, "StencilTestSeparate", QStringLiteral("QStencilTestSeparate cannot be instantiated on its own"));
    qmlRegisterType<Qt3DRender::QStencilTest>(uri, 2, 0, "StencilTest");
    qmlRegisterType<Qt3DRender::QScissorTest>(uri, 2, 0, "ScissorTest");
    qmlRegisterType<Qt3DRender::QDithering>(uri, 2, 0, "Dithering");
    qmlRegisterType<Qt3DRender::QAlphaCoverage>(uri, 2, 0, "AlphaCoverage");
    qmlRegisterType<Qt3DRender::QPointSize>(uri, 2, 0, "PointSize");
    qmlRegisterType<Qt3DRender::QPolygonOffset>(uri, 2, 0, "PolygonOffset");
    qmlRegisterType<Qt3DRender::QColorMask>(uri, 2, 0, "ColorMask");
    qmlRegisterType<Qt3DRender::QClipPlane>(uri, 2, 0, "ClipPlane");
    qmlRegisterUncreatableType<Qt3DRender::QStencilOpSeparate>(uri, 2, 0, "StencilOpSeparate", QStringLiteral("StencilOpSeparate cannot be instanciated on its own"));
    qmlRegisterType<Qt3DRender::QStencilOp>(uri, 2, 0, "StencilOp");
    qmlRegisterType<Qt3DRender::QStencilMask>(uri, 2, 0, "StencilMask");

    // Register types provided as QML files compiled into the plugin
    for (int i = 0; i < int(sizeof(qmldir) / sizeof(qmldir[0])); i++) {
        QString path = QStringLiteral("qrc:/Qt3D/Render/defaults/qml/");
        qmlRegisterType(QUrl(path + qmldir[i].type + QStringLiteral(".qml")),
                        uri,
                        qmldir[i].major, qmldir[i].minor,
                        qmldir[i].type);
    }
}

QT_END_NAMESPACE


