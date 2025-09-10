// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrenderpluginfactory_p.h"
#include "qrenderpluginfactoryif_p.h"
#include "qrenderplugin_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>

#include <private/qfactoryloader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QRenderPluginFactoryInterface_iid, QLatin1String("/renderplugins"), Qt::CaseInsensitive))

QStringList QRenderPluginFactory::keys()
{
    return loader()->keyMap().values();
}

QRenderPlugin *QRenderPluginFactory::create(const QString &name, const QStringList &args)
{
    return qLoadPlugin<QRenderPlugin, QRenderPluginFactoryIf>(loader(), name, args);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
