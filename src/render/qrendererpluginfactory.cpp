// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrendererpluginfactory_p.h"

#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include <Qt3DRender/private/qrendererplugin_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QRendererPluginFactoryInterface_iid, QLatin1String("/renderers"), Qt::CaseInsensitive))

QStringList QRendererPluginFactory::keys()
{
    return loader->keyMap().values();
}

AbstractRenderer *QRendererPluginFactory::create(const QString &name)
{
    return qLoadPlugin<AbstractRenderer, QRendererPlugin>(loader(), name);
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
