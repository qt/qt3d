/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtQml>
#include "qt3dquick3drenderplugin.h"
#include <Qt3DRenderer/qabstractmesh.h>
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
#include <Qt3DRenderer/qscissortest.h>
#include <Qt3DRenderer/qdithering.h>
#include <Qt3DRenderer/qrenderattachment.h>
#include <Qt3DRenderer/qclearbuffer.h>
#include <Qt3DRenderer/qsortcriterion.h>
#include <Qt3DRenderer/qalphacoverage.h>
#include <Qt3DRenderer/qpolygonoffset.h>
#include <Qt3DRenderer/qcolormask.h>
#include <Qt3DRenderer/qshaderdata.h>
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
#include <Qt3DQuickRenderer/quick3dframegraphitem.h>
#include <Qt3DQuickRenderer/quick3dsortmethod.h>
#include <Qt3DQuickRenderer/quick3dparameter.h>
#include <Qt3DQuickRenderer/quick3dshaderdata.h>
#include <Qt3DQuickRenderer/quick3dshaderdataarray.h>

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
    { "DiffuseMapMaterial", 2, 0 },
    { "DiffuseSpecularMapMaterial", 2, 0 },
    { "NormalDiffuseMapAlphaMaterial", 2, 0 },
    { "NormalDiffuseMapMaterial", 2, 0 },
    { "NormalDiffuseSpecularMapMaterial", 2, 0 },
    // FrameGraphs
    { "ForwardRenderer", 2, 0 }
};

QVariantList QJSValueToVariantListConverter(const QJSValue &jsValue)
{
    QVariantList values;
    QJSValueIterator it(jsValue);
    while (it.hasNext()) {
        it.next();
        if (it.hasNext()) // The last element in an Array is the count, which we don't want
            values << it.value().toVariant();
    }
    return values;
}

QVariantList Quick3DShaderDataArrayToVariantListConverter(Qt3D::Render::Quick::Quick3DShaderDataArray *array)
{
    QList<Qt3D::QShaderData *> arrayValues = array->values();
    QVariantList values;
    values.reserve(arrayValues.size());
    Q_FOREACH (Qt3D::QShaderData *data, arrayValues)
        values.append(QVariant::fromValue(data));
    return values;
}

void Qt3DQuick3DRenderPlugin::registerTypes(const char *uri)
{
    // Init resources for defaults QML files
    initResources();

    // Converters from QJSValue
    QMetaType::registerConverter<QJSValue, QVariantList>(QJSValueToVariantListConverter);
    QMetaType::registerConverter<Qt3D::Render::Quick::Quick3DShaderDataArray*, QVariantList>(Quick3DShaderDataArrayToVariantListConverter);

    // @uri Qt3D.Render
    qmlRegisterUncreatableType<Qt3D::Render::QAbstractSceneLoader>(uri, 2, 0, "QAbstractSceneLoader", "QAbstractScene is abstract");
    qmlRegisterExtendedType<Qt3D::QSceneLoader, Qt3D::Render::Quick::Quick3DScene>(uri, 2, 0, "SceneLoader");
//    qmlRegisterCustomType<Qt3D::Effect>(uri, 2, 0, "Effect", new Qt3D::Render::Quick::ShaderPropertyParser());
    qmlRegisterExtendedType<Qt3D::QEffect, Qt3D::Render::Quick::Quick3DEffect>(uri, 2, 0, "Effect");
    qmlRegisterExtendedType<Qt3D::QTechnique, Qt3D::Render::Quick::Quick3DTechnique>(uri, 2, 0, "Technique");
    qmlRegisterType<Qt3D::QAnnotation>(uri, 2, 0, "Annotation");
    qmlRegisterType<Qt3D::QOpenGLFilter>(uri, 2, 0, "OpenGLFilter");
    qmlRegisterUncreatableType<Qt3D::QParameter>(uri, 2, 0, "QParameter", "Quick3D should instantiate Quick3DParameter only");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DParameter>(uri, 2, 0, "Parameter");
    qmlRegisterType<Qt3D::QParameterMapping>(uri, 2, 0, "ParameterMapping");
    qmlRegisterExtendedType<Qt3D::QMaterial, Qt3D::Render::Quick::Quick3DMaterial>(uri, 2, 0, "Material");
    qmlRegisterExtendedType<Qt3D::QRenderPass, Qt3D::Render::Quick::Quick3DRenderPass>(uri, 2, 0, "RenderPass");
    qmlRegisterType<Qt3D::QShaderProgram>(uri, 2, 0, "ShaderProgram");
    qmlRegisterUncreatableType<Qt3D::QShaderData>(uri, 2, 0, "QShaderData", "Quick3D should instantiate Quick3DShaderData only");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DShaderDataArray>(uri, 2, 0, "ShaderDataArray");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DShaderData>(uri, 2, 0, "ShaderData");

    // Textures
    qmlRegisterType<Qt3D::QTextureWrapMode>(uri, 2, 0, "WrapMode");//, QStringLiteral("QTextureWrapMode cannot be created from QML"));
    qmlRegisterUncreatableType<Qt3D::QTexture>(uri, 2, 0, "Texture", QStringLiteral("Texture should be created from one of the subclasses"));
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTexture1D>(uri, 2, 0, "Texture1D");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTexture1DArray>(uri, 2, 0, "Texture1DArray");
    qmlRegisterExtendedType<Qt3D::Render::Quick::Quick3DTexture2D, Qt3D::Render::Quick::Quick3DTexture2DExtension>(uri, 2, 0, "Texture2D");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTexture2DArray>(uri, 2, 0, "Texture2DArray");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTexture3D>(uri, 2, 0, "Texture3D");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTextureCubeMap>(uri, 2, 0, "TextureCubeMap");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTextureCubeMapArray>(uri, 2, 0, "TextureCubeMapArray");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTexture2DMultisample>(uri, 2, 0, "Texture2DMultisample");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTexture2DMultisampleArray>(uri, 2, 0, "Texture2DMultisampleArray");

    // Meshes
    qmlRegisterUncreatableType<Qt3D::QAbstractMesh>(uri, 2, 0, "QAbstractMesh", QStringLiteral("QAbstractMesh is abstract"));
    qmlRegisterType<Qt3D::QMesh>(uri, 2, 0, "Mesh");
    qmlRegisterType<Qt3D::QCuboidMesh>(uri, 2, 0, "CuboidMesh");
    qmlRegisterType<Qt3D::QCylinderMesh>(uri, 2, 0, "CylinderMesh");
    qmlRegisterType<Qt3D::QPlaneMesh>(uri, 2, 0, "PlaneMesh");
    qmlRegisterType<Qt3D::QTorusMesh>(uri, 2, 0, "TorusMesh");
    qmlRegisterType<Qt3D::QSphereMesh>(uri, 2, 0, "SphereMesh");

    // Layers
    qmlRegisterType<Qt3D::QLayer>(uri, 2, 0, "Layer");
    qmlRegisterType<Qt3D::QLayerFilter>(uri, 2, 0, "LayerFilter");

    // Lights
    qmlRegisterUncreatableType<Qt3D::QAbstractLight>(uri, 2, 0, "QAbstractLight", QStringLiteral("QAbstractLight is abstract"));
    qmlRegisterType<Qt3D::QPointLight>(uri, 2, 0, "PointLight");
    qmlRegisterType<Qt3D::QDirectionalLight>(uri, 2, 0, "DirectionalLight");
    qmlRegisterType<Qt3D::QSpotLight>(uri, 2, 0, "SpotLight");

    // FrameGraph
    qmlRegisterExtendedType<Qt3D::QCameraSelector, Qt3D::Quick::Quick3DNode>(uri, 2, 0, "CameraSelector");
    qmlRegisterExtendedType<Qt3D::QRenderPassFilter, Qt3D::Render::Quick::Quick3DRenderPassFilter>(uri, 2, 0, "RenderPassFilter");
    qmlRegisterExtendedType<Qt3D::QTechniqueFilter, Qt3D::Render::Quick::Quick3DTechniqueFilter>(uri, 2, 0, "TechniqueFilter");
    qmlRegisterExtendedType<Qt3D::QViewport, Qt3D::Render::Quick::Quick3DViewport>(uri, 2, 0, "Viewport");
    qmlRegisterType<Qt3D::QRenderTargetSelector>(uri, 2, 0, "RenderTargetSelector");
    qmlRegisterType<Qt3D::QClearBuffer>(uri, 2, 0, "ClearBuffer");
    qmlRegisterExtendedUncreatableType<Qt3D::QFrameGraphNode, Qt3D::Render::Quick::Quick3DFrameGraphItem>(uri, 2, 0, "FrameGraphNode", QStringLiteral("FrameGraphNode is a base class"));
    qmlRegisterType<Qt3D::QFrameGraph>(uri, 2, 0, "FrameGraph");

    // RenderTarget
    qmlRegisterType<Qt3D::QRenderAttachment>(uri, 2, 0, "RenderAttachment");
    qmlRegisterExtendedType<Qt3D::QRenderTarget, Qt3D::Render::Quick::Quick3DRenderTarget>(uri, 2, 0, "RenderTarget");

    // Sorting
    qmlRegisterType<Qt3D::QSortCriterion>(uri, 2, 0, "SortCriterion");
    qmlRegisterExtendedType<Qt3D::QSortMethod, Qt3D::Render::Quick::Quick3DSortMethod>(uri, 2, 0, "SortMethod");

    // RenderStates
    qmlRegisterUncreatableType<Qt3D::QRenderState>(uri, 2, 0, "RenderState", QStringLiteral("QRenderState is a base class"));
    qmlRegisterType<Qt3D::QBlendState>(uri, 2, 0, "BlendState");
    qmlRegisterType<Qt3D::QBlendEquation>(uri, 2, 0, "BlendEquation");
    qmlRegisterType<Qt3D::QAlphaTest>(uri, 2, 0, "AlphaTest");
    qmlRegisterType<Qt3D::QDepthTest>(uri, 2, 0, "DepthTest");
    qmlRegisterType<Qt3D::QDepthMask>(uri, 2, 0, "DepthMask");
    qmlRegisterType<Qt3D::QCullFace>(uri, 2, 0, "CullFace");
    qmlRegisterType<Qt3D::QFrontFace>(uri, 2, 0, "FrontFace");
    qmlRegisterType<Qt3D::QStencilTest>(uri, 2, 0, "StencilTest");
    qmlRegisterType<Qt3D::QScissorTest>(uri, 2, 0, "ScissorTest");
    qmlRegisterType<Qt3D::QDithering>(uri, 2, 0, "Dithering");
    qmlRegisterType<Qt3D::QAlphaCoverage>(uri, 2, 0, "AlphaCoverage");
    qmlRegisterType<Qt3D::QPolygonOffset>(uri, 2, 0, "PolygonOffset");
    qmlRegisterType<Qt3D::QColorMask>(uri, 2, 0, "ColorMask");

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


