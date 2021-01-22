/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
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
        Q_UNUSED(key)
        Q_UNUSED(paramList)
        return new Qt3DRender::Render::Scene2DPlugin();
    }
};

QT_END_NAMESPACE

#include "main.moc"
