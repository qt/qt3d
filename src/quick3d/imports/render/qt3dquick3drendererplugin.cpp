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

#include "qt3dquick3drendererplugin.h"
#include <Qt3DRenderer/qabstractsceneloader.h>
#include <Qt3DRenderer/qsceneloader.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qviewport.h>
#include <Qt3DRenderer/qrenderpassfilter.h>
#include <Qt3DRenderer/qtechniquefilter.h>
#include <Qt3DRenderer/qrendertargetselector.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qcameraselector.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qannotation.h>
#include <Qt3DRenderer/qparametermapping.h>
#include <Qt3DRenderer/qcuboidmesh.h>
#include <Qt3DRenderer/qcylindermesh.h>
#include <Qt3DRenderer/qplanemesh.h>
#include <Qt3DRenderer/qspheremesh.h>
#include <Qt3DRenderer/qtorusmesh.h>
#include <Qt3DRenderer/qlayer.h>
#include <Qt3DRenderer/qlayerfilter.h>
#include <Qt3DRenderer/qabstractlight.h>
#include <Qt3DRenderer/qspotlight.h>
#include <Qt3DRenderer/qdirectionallight.h>
#include <Qt3DRenderer/qpointlight.h>
#include <Qt3DRenderer/qopenglfilter.h>
#include <Qt3DRenderer/qrenderstate.h>
#include <Qt3DRenderer/qblendstate.h>
#include <Qt3DRenderer/qblendequation.h>
#include <Qt3DRenderer/qalphatest.h>
#include <Qt3DRenderer/qdepthtest.h>
#include <Qt3DRenderer/qdepthmask.h>
#include <Qt3DRenderer/qcullface.h>
#include <Qt3DRenderer/qfrontface.h>
#include <Qt3DRenderer/qstenciltest.h>
#include <Qt3DRenderer/qstenciltestseparate.h>
#include <Qt3DRenderer/qscissortest.h>
#include <Qt3DRenderer/qdithering.h>
#include <Qt3DRenderer/qrenderattachment.h>
#include <Qt3DRenderer/qclearbuffer.h>
#include <Qt3DRenderer/qsortcriterion.h>
#include <Qt3DRenderer/qalphacoverage.h>
#include <Qt3DRenderer/qpolygonoffset.h>
#include <Qt3DRenderer/qcolormask.h>
#include <Qt3DRenderer/qshaderdata.h>
#include <Qt3DRenderer/qnodraw.h>
#include <Qt3DRenderer/qclipplane.h>
#include <Qt3DRenderer/qstencilop.h>
#include <Qt3DRenderer/qstencilopseparate.h>
#include <Qt3DRenderer/qstencilmask.h>
#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qgeometry.h>
#include <Qt3DRenderer/qtorusgeometry.h>
#include <Qt3DRenderer/qspheregeometry.h>
#include <Qt3DRenderer/qcuboidgeometry.h>
#include <Qt3DRenderer/qplanegeometry.h>
#include <Qt3DRenderer/qgeometryrenderer.h>
#include <Qt3DQuickRenderer/quick3dtechnique.h>
#include <Qt3DQuickRenderer/quick3dmaterial.h>
#include <Qt3DQuickRenderer/quick3dtechniquefilter.h>
#include <Qt3DQuickRenderer/quick3dviewport.h>
#include <Qt3DQuickRenderer/quick3drenderpassfilter.h>
#include <Qt3DQuickRenderer/quick3drendertarget.h>
#include <Qt3DQuickRenderer/quick3deffect.h>
#include <Qt3DQuickRenderer/quick3dscene.h>
#include <Qt3DQuickRenderer/quick3dtexture.h>
#include <Qt3DQuickRenderer/private/shaderpropertyparser_p.h>
#include <Qt3DQuickRenderer/quick3drenderpass.h>
#include <Qt3DQuickRenderer/quick3dsortmethod.h>
#include <Qt3DQuickRenderer/quick3dparameter.h>
#include <Qt3DQuickRenderer/quick3dshaderdata.h>
#include <Qt3DQuickRenderer/quick3dshaderdataarray.h>
#include <Qt3DQuickRenderer/quick3dstateset.h>
#include <Qt3DQuickRenderer/quick3drendertargetselector.h>
#include <Qt3DQuickRenderer/quick3dgeometry.h>
#include <Qt3DQuickRenderer/quick3dbuffer.h>
#include <Qt3DQuickRenderer/private/qt3dquickrenderer_global_p.h>

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

void Qt3DQuick3DRendererPlugin::registerTypes(const char *uri)
{
    // Init resources for defaults QML files
    initResources();

    Qt3DRender::Quick::Quick3DRenderer_initialize();

    // Converters
    QMetaType::registerConverter<Qt3DRender::Render::Quick::Quick3DShaderDataArray*, QVariantList>(Quick3DShaderDataArrayToVariantListConverter);

    // @uri Qt3D.Renderer
    qmlRegisterUncreatableType<Qt3DRender::QAbstractSceneLoader>(uri, 2, 0, "QAbstractSceneLoader", "QAbstractScene is abstract");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QSceneLoader, Qt3DRender::Render::Quick::Quick3DScene>("QSceneLoader", "Qt3D.Renderer/SceneLoader", uri, 2, 0, "SceneLoader");
//    qmlRegisterCustomType<Qt3DRender::Effect>(uri, 2, 0, "Effect", new Qt3DRender::Render::Quick::ShaderPropertyParser());
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QEffect, Qt3DRender::Render::Quick::Quick3DEffect>("QEffect", "Qt3D.Renderer/Effect", uri, 2, 0, "Effect");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTechnique, Qt3DRender::Render::Quick::Quick3DTechnique>("QTechnique", "Qt3D.Renderer/Technique", uri, 2, 0, "Technique");
    qmlRegisterType<Qt3DRender::QAnnotation>(uri, 2, 0, "Annotation");
    qmlRegisterType<Qt3DRender::QOpenGLFilter>(uri, 2, 0, "OpenGLFilter");
    qmlRegisterUncreatableType<Qt3DRender::QParameter>(uri, 2, 0, "QParameter", "Quick3D should instantiate Quick3DParameter only");
    qmlRegisterType<Qt3DRender::Render::Quick::Quick3DParameter>(uri, 2, 0, "Parameter");
    qmlRegisterType<Qt3DRender::QParameterMapping>(uri, 2, 0, "ParameterMapping");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QMaterial, Qt3DRender::Render::Quick::Quick3DMaterial>("QMaterial", "Qt3D.Renderer/Material", uri, 2, 0, "Material");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderPass, Qt3DRender::Render::Quick::Quick3DRenderPass>("QRenderPass", "Qt3D.Renderer/RenderPass", uri, 2, 0, "RenderPass");
    qmlRegisterType<Qt3DRender::QShaderProgram>(uri, 2, 0, "ShaderProgram");
    qmlRegisterUncreatableType<Qt3DRender::QShaderData>(uri, 2, 0, "QShaderData", "Quick3D should instantiate Quick3DShaderData only");
    qmlRegisterType<Qt3DRender::Render::Quick::Quick3DShaderDataArray>(uri, 2, 0, "ShaderDataArray");
    qmlRegisterType<Qt3DRender::Render::Quick::Quick3DShaderData>(uri, 2, 0, "ShaderData");

    // Textures
    qmlRegisterType<Qt3DRender::QTextureWrapMode>(uri, 2, 0, "WrapMode");//, QStringLiteral("QTextureWrapMode cannot be created from QML"));
    qmlRegisterUncreatableType<Qt3DRender::QAbstractTextureProvider>(uri, 2, 0, "Texture", QStringLiteral("Texture should be created from one of the subclasses"));
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture1D, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture1D", "Qt3D.Renderer/Texture1D", uri, 2, 0, "Texture1D");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture1DArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture1DArray", "Qt3D.Renderer/Texture1DArray", uri, 2, 0, "Texture1DArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2D, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2D", "Qt3D.Renderer/Texture2D", uri, 2, 0, "Texture2D");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2DArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2DArray", "Qt3D.Renderer/Texture2DArray", uri, 2, 0, "Texture2DArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture3D, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture3D", "Qt3D.Renderer/Texture3D", uri, 2, 0, "Texture3D");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureCubeMap, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureCubeMap", "Qt3D.Renderer/TextureCubeMap", uri, 2, 0, "TextureCubeMap");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureCubeMapArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureCubeMapArray", "Qt3D.Renderer/TextureCubeMapArray", uri, 2, 0, "TextureCubeMapArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2DMultisample, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2DMultisample", "Qt3D.Renderer/Texture2DMultisample", uri, 2, 0, "Texture2DMultisample");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTexture2DMultisampleArray, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTexture2DMultisampleArray", "Qt3D.Renderer/Texture2DMultisampleArray", uri, 2, 0, "Texture2DMultisampleArray");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureRectangle, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureRectangle", "Qt3D.Renderer/TextureRectangle", uri, 2, 0, "TextureRectangle");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTextureBuffer, Qt3DRender::Render::Quick::Quick3DTextureExtension>("QTextureBuffer", "Qt3D.Renderer/TextureBuffer", uri, 2, 0, "TextureBuffer");
    qmlRegisterUncreatableType<Qt3DRender::QAbstractTextureImage>(uri, 2, 0, "QAbstractTextureImage", QStringLiteral("QAbstractTextureImage is abstract"));
    qmlRegisterType<Qt3DRender::QTextureImage>(uri, 2, 0, "TextureImage");

    // Geometry
    qmlRegisterUncreatableType<Qt3DCore::QAbstractAttribute>(uri, 2, 0, "QAbstractAttribute", QStringLiteral("QAbstractAttribute is abstract"));
    qmlRegisterUncreatableType<Qt3DCore::QAbstractBuffer>(uri, 2, 0, "QAbstractBuffer", QStringLiteral("QAbstractBuffer is abstract"));
    qmlRegisterType<Qt3DRender::QAttribute>(uri, 2, 0, "Attribute");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QBuffer, Qt3DRender::Render::Quick::Quick3DBuffer>("QBuffer", "Qt3D.Renderer/Buffer", uri, 2, 0, "Buffer");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QGeometry, Qt3DRender::Render::Quick::Quick3DGeometry>("QGeometry", "Qt3D.Renderer/Geometry", uri, 2, 0, "Geometry");
    qmlRegisterType<Qt3DRender::QGeometryRenderer>(uri, 2, 0, "GeometryRenderer");

    // Meshes
    qmlRegisterType<Qt3DRender::QMesh>(uri, 2, 0, "Mesh");
    qmlRegisterType<Qt3DRender::QCuboidMesh>(uri, 2, 0, "CuboidMesh");
    qmlRegisterType<Qt3DRender::QCuboidGeometry>(uri, 2, 0, "CuboidGeometry");
    qmlRegisterType<Qt3DRender::QCylinderMesh>(uri, 2, 0, "CylinderMesh");
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
    qmlRegisterUncreatableType<Qt3DRender::QAbstractLight>(uri, 2, 0, "QAbstractLight", QStringLiteral("QAbstractLight is abstract"));
    qmlRegisterType<Qt3DRender::QPointLight>(uri, 2, 0, "PointLight");
    qmlRegisterType<Qt3DRender::QDirectionalLight>(uri, 2, 0, "DirectionalLight");
    qmlRegisterType<Qt3DRender::QSpotLight>(uri, 2, 0, "SpotLight");

    // FrameGraph
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QCameraSelector, Qt3DCore::Quick::Quick3DNode>("QCameraSelector", "Qt3D.Renderer/CameraSelector", uri, 2, 0, "CameraSelector");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderPassFilter, Qt3DRender::Render::Quick::Quick3DRenderPassFilter>("QRenderPassFilter", "Qt3D.Renderer/RenderPassFilter", uri, 2, 0, "RenderPassFilter");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QTechniqueFilter, Qt3DRender::Render::Quick::Quick3DTechniqueFilter>("QTechniqueFilter", "Qt3D.Renderer/TechniqueFilter", uri, 2, 0, "TechniqueFilter");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QViewport, Qt3DRender::Render::Quick::Quick3DViewport>("QViewport", "Qt3D.Renderer/Viewport", uri, 2, 0, "Viewport");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderTargetSelector, Qt3DRender::Render::Quick::Quick3DRenderTargetSelector>("QRenderTargetSelector", "Qt3D.Renderer/RenderTargetSelector", uri, 2, 0, "RenderTargetSelector");
    qmlRegisterType<Qt3DRender::QClearBuffer>(uri, 2, 0, "ClearBuffer");
    qmlRegisterUncreatableType<Qt3DRender::QFrameGraphNode>(uri, 2, 0, "FrameGraphNode", QStringLiteral("FrameGraphNode is a base class"));
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QStateSet, Qt3DRender::Render::Quick::Quick3DStateSet>("QStateSet", "Qt3D.Renderer/StateSet", uri, 2, 0, "StateSet");
    qmlRegisterType<Qt3DRender::QNoDraw>(uri, 2, 0, "NoDraw");
    qmlRegisterType<Qt3DRender::QFrameGraph>(uri, 2, 0, "FrameGraph");

    // RenderTarget
    qmlRegisterType<Qt3DRender::QRenderAttachment>(uri, 2, 0, "RenderAttachment");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QRenderTarget, Qt3DRender::Render::Quick::Quick3DRenderTarget>("QRenderTarget", "Qt3D.Renderer/RenderTarget", uri, 2, 0, "RenderTarget");

    // Sorting
    qmlRegisterType<Qt3DRender::QSortCriterion>(uri, 2, 0, "SortCriterion");
    Qt3DRender::Quick::registerExtendedType<Qt3DRender::QSortMethod, Qt3DRender::Render::Quick::Quick3DSortMethod>("QSortMethod", "Qt3D.Renderer/SortMethod", uri, 2, 0, "SortMethod");

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


