// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrenderpluginfactoryif_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

QRenderPluginFactoryIf::QRenderPluginFactoryIf(QObject *parent)
    : QObject(parent)
{

}

QRenderPluginFactoryIf::~QRenderPluginFactoryIf()
{

}

QRenderPlugin *QRenderPluginFactoryIf::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(key);
    Q_UNUSED(paramList);
    return nullptr;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrenderpluginfactoryif_p.cpp"
