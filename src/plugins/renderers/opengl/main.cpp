/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#include <Qt3DRender/private/qrendererplugin_p.h>
#include <renderer_p.h>

QT_BEGIN_NAMESPACE

class OpenGLRendererPlugin : public Qt3DRender::Render::QRendererPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QRendererPluginFactoryInterface_iid FILE "openglrenderer.json")

    Qt3DRender::Render::AbstractRenderer *create(const QString &key,  Qt3DRender::QRenderAspect::RenderType renderMode) override
    {
        Q_UNUSED(key)
        return new Qt3DRender::Render::OpenGL::Renderer(renderMode);
    }
};

QT_END_NAMESPACE

#include "main.moc"
