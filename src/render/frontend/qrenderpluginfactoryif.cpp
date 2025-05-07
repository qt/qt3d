// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

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
