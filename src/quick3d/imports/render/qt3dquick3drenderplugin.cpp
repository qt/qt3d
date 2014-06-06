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
#include <Qt3DRenderer/scene.h>
#include <Qt3DRenderer/tag.h>
#include <Qt3DRenderer/mesh.h>
#include <Qt3DRenderer/renderpass.h>
#include <Qt3DRenderer/shaderprogram.h>
#include <Qt3DRenderer/viewport.h>
#include <Qt3DRenderer/renderpassfilter.h>
#include <Qt3DRenderer/techniquefilter.h>
#include <Qt3DRenderer/rendertargetselector.h>
#include <Qt3DRenderer/shape.h>
#include <Qt3DRenderer/texture.h>
#include <Qt3DRenderer/framegraph.h>
#include <Qt3DRenderer/technique.h>
#include <Qt3DRenderer/cameraselector.h>
#include <Qt3DRenderer/effect.h>
#include <Qt3DRenderer/parameter.h>
#include <Qt3DRenderer/renderpasscriterion.h>
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

QT_BEGIN_NAMESPACE

void Qt3DQuick3DRenderPlugin::registerTypes(const char *uri)
{
    qmlRegisterInterface<Qt3D::Render::QAbstractScene>("QAbstractScene");
    qmlRegisterExtendedType<Qt3D::Scene, Qt3D::Render::Quick::Quick3DScene>(uri, 2, 0, "Scene");
//    qmlRegisterCustomType<Qt3D::Effect>(uri, 2, 0, "Effect", new Qt3D::Render::Quick::ShaderPropertyParser());
    qmlRegisterExtendedType<Qt3D::Effect, Qt3D::Render::Quick::Quick3DEffect>(uri, 2, 0, "Effect");
    qmlRegisterExtendedType<Qt3D::Technique, Qt3D::Render::Quick::Quick3DTechnique>(uri, 2, 0, "Technique");
    qmlRegisterType<Qt3D::TechniqueCriterion>(uri, 2, 0, "TechniqueCriterion");
    qmlRegisterType<Qt3D::RenderPassCriterion>(uri, 2, 0, "RenderPassCriterion");
    qmlRegisterType<Qt3D::Tag>(uri, 2, 0, "Tag");
    qmlRegisterType<Qt3D::Parameter>(uri, 2, 0, "Parameter");
    qmlRegisterType<Qt3D::Mesh>(uri, 2, 0, "Mesh");
    qmlRegisterExtendedType<Qt3D::Material, Qt3D::Render::Quick::Quick3DMaterial>(uri, 2, 0, "Material");
    qmlRegisterExtendedType<Qt3D::RenderPass, Qt3D::Render::Quick::Quick3DRenderPass>(uri, 2, 0, "RenderPass");
    qmlRegisterType<Qt3D::ShaderProgram>(uri, 2, 0, "ShaderProgram");
    qmlRegisterType<Qt3D::Shape>(uri, 2, 0, "Shape");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTexture>(uri, 2, 0, "Texture");
    // Framegraph components - TODO RenderTarget, RenderTargetSelector
    qmlRegisterExtendedType<Qt3D::CameraSelector, Qt3D::Quick::Quick3DNode>(uri, 2, 0, "CameraSelector");
    qmlRegisterExtendedType<Qt3D::RenderPassFilter, Qt3D::Render::Quick::Quick3DRenderPassFilter>(uri, 2, 0, "RenderPassFilter");
    qmlRegisterExtendedType<Qt3D::TechniqueFilter, Qt3D::Render::Quick::Quick3DTechniqueFilter>(uri, 2, 0, "TechniqueFilter");
    qmlRegisterExtendedType<Qt3D::Viewport, Qt3D::Render::Quick::Quick3DViewport>(uri, 2, 0, "Viewport");
    qmlRegisterExtendedType<Qt3D::RenderTargetSelector, Qt3D::Render::Quick::Quick3DRenderTargetSelector>(uri, 2, 0, "RenderTargetSelector");
    qmlRegisterType<Qt3D::FrameGraph>(uri, 2, 0, "FrameGraph");
}

QT_END_NAMESPACE


