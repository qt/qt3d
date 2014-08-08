/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRenderer/qabstractscene.h>
#include <Qt3DRenderer/qscene.h>
#include <Qt3DRenderer/tag.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qviewport.h>
#include <Qt3DRenderer/qrenderpassfilter.h>
#include <Qt3DRenderer/qtechniquefilter.h>
#include <Qt3DRenderer/qrendertargetselector.h>
#include <Qt3DRenderer/qabstractshapemesh.h>
#include <Qt3DRenderer/texture.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qcameraselector.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qcriterion.h>
#include <Qt3DRenderer/qparametermapper.h>
#include <Qt3DRenderer/qcuboidmesh.h>
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
#include <Qt3DRenderer/qdrawstate.h>
#include <Qt3DRenderer/qblendstate.h>
#include <Qt3DRenderer/qblendequation.h>
#include <Qt3DRenderer/qalphatest.h>
#include <Qt3DRenderer/qdepthtest.h>
#include <Qt3DRenderer/qdepthmask.h>
#include <Qt3DRenderer/qcullface.h>
#include <Qt3DRenderer/qfrontface.h>
#include <Qt3DRenderer/qstenciltest.h>
#include <Qt3DRenderer/qscissortest.h>
#include <Qt3DQuickRenderer/quick3dtechnique.h>
#include <Qt3DQuickRenderer/quick3dmaterial.h>
#include <Qt3DQuickRenderer/quick3dtechniquefilter.h>
#include <Qt3DQuickRenderer/quick3dviewport.h>
#include <Qt3DQuickRenderer/quick3drenderpassfilter.h>
#include <Qt3DQuickRenderer/quick3drendertargetselector.h>
#include <Qt3DQuickRenderer/quick3deffect.h>
#include <Qt3DQuickRenderer/quick3dscene.h>
#include <Qt3DQuickRenderer/quick3dtexture.h>
#include <Qt3DQuickRenderer/shaderpropertyparser.h>
#include <Qt3DQuickRenderer/quick3drenderpass.h>
#include <Qt3DQuickRenderer/quick3dframegraphitem.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DRenderPlugin::registerTypes(const char *uri)
{
    // @uri Qt3D.Render
    qmlRegisterUncreatableType<Qt3D::Render::QAbstractScene>(uri, 2, 0, "QAbstractScene", "QAbstractScene is abstract");
    qmlRegisterExtendedType<Qt3D::QScene, Qt3D::Render::Quick::Quick3DScene>(uri, 2, 0, "Scene");
//    qmlRegisterCustomType<Qt3D::Effect>(uri, 2, 0, "Effect", new Qt3D::Render::Quick::ShaderPropertyParser());
    qmlRegisterExtendedType<Qt3D::QEffect, Qt3D::Render::Quick::Quick3DEffect>(uri, 2, 0, "Effect");
    qmlRegisterExtendedType<Qt3D::QTechnique, Qt3D::Render::Quick::Quick3DTechnique>(uri, 2, 0, "Technique");
    qmlRegisterType<Qt3D::QCriterion>(uri, 2, 0, "Criterion");
    qmlRegisterType<Qt3D::QOpenGLFilter>(uri, 2, 0, "OpenGLFilter");
    qmlRegisterType<Qt3D::Tag>(uri, 2, 0, "Tag");
    qmlRegisterType<Qt3D::QParameter>(uri, 2, 0, "Parameter");
    qmlRegisterType<Qt3D::QParameterMapper>(uri, 2, 0, "ParameterMapper");
    qmlRegisterExtendedType<Qt3D::QMaterial, Qt3D::Render::Quick::Quick3DMaterial>(uri, 2, 0, "Material");
    qmlRegisterExtendedType<Qt3D::QRenderPass, Qt3D::Render::Quick::Quick3DRenderPass>(uri, 2, 0, "RenderPass");
    qmlRegisterType<Qt3D::QShaderProgram>(uri, 2, 0, "ShaderProgram");

    qmlRegisterUncreatableType<Qt3D::QAbstractMesh>(uri, 2, 0, "QAbstractMesh", QStringLiteral("QAbstractMesh is abstract"));
    qmlRegisterType<Qt3D::QMesh>(uri, 2, 0, "Mesh");
    qmlRegisterUncreatableType<Qt3D::QAbstractShapeMesh>(uri, 2, 0, "QAbstractShapeMesh", QStringLiteral("Shape is an abstract base class"));
    qmlRegisterType<Qt3D::QCuboidMesh>(uri, 2, 0, "CuboidMesh");
    qmlRegisterType<Qt3D::QPlaneMesh>(uri, 2, 0, "PlaneMesh");
    qmlRegisterType<Qt3D::QTorusMesh>(uri, 2, 0, "TorusMesh");
    qmlRegisterType<Qt3D::QSphereMesh>(uri, 2, 0, "SphereMesh");

    qmlRegisterType<Qt3D::QLayer>(uri, 2, 0, "Layer");
    qmlRegisterType<Qt3D::QLayerFilter>(uri, 2, 0, "LayerFilter");

    qmlRegisterUncreatableType<Qt3D::QAbstractLight>(uri, 2, 0, "QAbstractLight", QStringLiteral("QAbstractLight is abstract"));
    qmlRegisterType<Qt3D::QPointLight>(uri, 2, 0, "PointLight");
    qmlRegisterType<Qt3D::QDirectionalLight>(uri, 2, 0, "DirectionalLight");
    qmlRegisterType<Qt3D::QSpotLight>(uri, 2, 0, "SpotLight");

    qmlRegisterExtendedType<Qt3D::Texture, Qt3D::Render::Quick::Quick3DTexture>(uri, 2, 0, "Texture");
    // Framegraph components - TODO RenderTarget, RenderTargetSelector
    qmlRegisterExtendedType<Qt3D::QCameraSelector, Qt3D::Quick::Quick3DNode>(uri, 2, 0, "CameraSelector");
    qmlRegisterExtendedType<Qt3D::QRenderPassFilter, Qt3D::Render::Quick::Quick3DRenderPassFilter>(uri, 2, 0, "RenderPassFilter");
    qmlRegisterExtendedType<Qt3D::QTechniqueFilter, Qt3D::Render::Quick::Quick3DTechniqueFilter>(uri, 2, 0, "TechniqueFilter");
    qmlRegisterExtendedType<Qt3D::QViewport, Qt3D::Render::Quick::Quick3DViewport>(uri, 2, 0, "Viewport");
    qmlRegisterExtendedType<Qt3D::QRenderTargetSelector, Qt3D::Render::Quick::Quick3DRenderTargetSelector>(uri, 2, 0, "RenderTargetSelector");
    // Should be uncreatable extended type
    qmlRegisterExtendedUncreatableType<Qt3D::QFrameGraphItem, Qt3D::Render::Quick::Quick3DFrameGraphItem>(uri, 2, 0, "FrameGraphItem", QStringLiteral("FrameGraphItem is a base class"));
    qmlRegisterType<Qt3D::QFrameGraph>(uri, 2, 0, "FrameGraph");

    // DrawStates
    qmlRegisterUncreatableType<Qt3D::QDrawState>(uri, 2, 0, "DrawState", QStringLiteral("QDrawState is a base class"));
    qmlRegisterType<Qt3D::QBlendState>(uri, 2, 0, "BlendState");
    qmlRegisterType<Qt3D::QBlendEquation>(uri, 2, 0, "BlendEquation");
    qmlRegisterType<Qt3D::QAlphaTest>(uri, 2, 0, "AlphaTest");
    qmlRegisterType<Qt3D::QDepthTest>(uri, 2, 0, "DepthTest");
    qmlRegisterType<Qt3D::QDepthMask>(uri, 2, 0, "DepthMask");
    qmlRegisterType<Qt3D::QCullFace>(uri, 2, 0, "CullFace");
    qmlRegisterType<Qt3D::QFrontFace>(uri, 2, 0, "FrontFace");
    qmlRegisterType<Qt3D::QStencilTest>(uri, 2, 0, "StencilTest");
    qmlRegisterType<Qt3D::QScissorTest>(uri, 2, 0, "ScissorTest");
}

QT_END_NAMESPACE


