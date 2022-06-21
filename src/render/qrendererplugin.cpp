// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrendererplugin_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

QRendererPlugin::QRendererPlugin(QObject *parent)
    : QObject(parent)
{
}

QRendererPlugin::~QRendererPlugin()
{
}

AbstractRenderer *QRendererPlugin::create(const QString &key)
{
    Q_UNUSED(key);
    return nullptr;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qrendererplugin_p.cpp"
