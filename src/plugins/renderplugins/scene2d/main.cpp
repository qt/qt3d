// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "scene2dplugin.h"

#include <private/qrenderpluginfactoryif_p.h>
#include <private/qrenderplugin_p.h>

QT_BEGIN_NAMESPACE

class Scene2DPlugin : public Qt3DRender::Render::QRenderPluginFactoryIf
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QRenderPluginFactoryInterface_iid FILE "scene2dplugin.json")

    Qt3DRender::Render::QRenderPlugin *create(const QString &key,
                                              const QStringList &paramList) override
    {
        Q_UNUSED(key);
        Q_UNUSED(paramList);
        return new Qt3DRender::Render::Scene2DPlugin();
    }
};

QT_END_NAMESPACE

#include "main.moc"
