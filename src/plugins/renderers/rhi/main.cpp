// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/qrendererplugin_p.h>
#include <renderer_p.h>

QT_BEGIN_NAMESPACE

class RhiRendererPlugin : public Qt3DRender::Render::QRendererPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QRendererPluginFactoryInterface_iid FILE "rhirenderer.json")

    Qt3DRender::Render::AbstractRenderer *
    create(const QString &key) override
    {
        Q_UNUSED(key);
        return new Qt3DRender::Render::Rhi::Renderer();
    }
};

QT_END_NAMESPACE

#include "main.moc"
