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
#include <Qt3DRenderer/scene.h>
#include <Qt3DRenderer/effect.h>
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
#include <Qt3DQuickRenderer/quick3dtechnique.h>
#include <Qt3DQuickRenderer/quick3dmaterial.h>
#include <Qt3DQuickRenderer/quick3dcameraselector.h>
#include <Qt3DQuickRenderer/quick3dtechniquefilter.h>
#include <Qt3DQuickRenderer/quick3dviewport.h>
#include <Qt3DQuickRenderer/quick3drenderpassfilter.h>
#include <Qt3DQuickRenderer/quick3drendertargetselector.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DRenderPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Qt3D::Scene>(uri, 2, 0, "Scene");
    qmlRegisterType<Qt3D::Effect>(uri, 2, 0, "Effect");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTechnique>(uri, 2, 0, "Technique");
    qmlRegisterType<Qt3D::Tag>(uri, 2, 0, "Tag");
    qmlRegisterType<Qt3D::Mesh>(uri, 2, 0, "Mesh");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DMaterial>(uri, 2, 0, "Material");
    qmlRegisterType<Qt3D::RenderPass>(uri, 2, 0, "RenderPass");
    qmlRegisterType<Qt3D::ShaderProgram>(uri, 2, 0, "ShaderProgram");
    qmlRegisterType<Qt3D::Shape>(uri, 2, 0, "Shape");
    qmlRegisterType<Qt3D::QmlTexture>(uri, 2, 0, "Texture");
    // Framegraph components - TODO RenderTarget, RenderTargetSelector
    qmlRegisterType<Qt3D::Render::Quick::Quick3DCameraSelector>(uri, 2, 0, "CameraSelector");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DRenderPassFilter>(uri, 2, 0, "RenderPassFilter");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DTechniqueFilter>(uri, 2, 0, "TechniqueFilter");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DViewport>(uri, 2, 0, "Viewport");
//    qmlRegisterType<Qt3D::FrameGraphItem>("Qt3D.Render", 2, 0, "FrameGraphNode");
    qmlRegisterType<Qt3D::Render::Quick::Quick3DRenderTargetSelector>(uri, 2, 0, "RenderTargetSelector");
    qmlRegisterType<Qt3D::FrameGraph>(uri, 2, 0, "FrameGraph");
}

QT_END_NAMESPACE


