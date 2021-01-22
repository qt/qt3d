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

#include "qrendererpluginfactory_p.h"

#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include <Qt3DRender/private/qrendererplugin_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QRendererPluginFactoryInterface_iid, QLatin1String("/renderers"), Qt::CaseInsensitive))
#ifndef QT_NO_LIBRARY
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, directLoader, (QRendererPluginFactoryInterface_iid, QLatin1String(""), Qt::CaseInsensitive))
#endif

QStringList QRendererPluginFactory::keys(const QString &pluginPath)
{
    QStringList list;
    if (!pluginPath.isEmpty()) {
#if QT_CONFIG(library)
        QCoreApplication::addLibraryPath(pluginPath);
        list = directLoader()->keyMap().values();
        if (!list.isEmpty()) {
            const QString postFix = QLatin1String(" (from ")
                    + QDir::toNativeSeparators(pluginPath)
                    + QLatin1Char(')');
            const QStringList::iterator end = list.end();
            for (QStringList::iterator it = list.begin(); it != end; ++it)
                (*it).append(postFix);
        }
#else
        qWarning() << QObject::tr("Cannot query QRendererPlugin plugins at %1. "
                                  "Library loading is disabled.").arg(pluginPath);
#endif
    }
    list.append(loader()->keyMap().values());
    return list;
}

AbstractRenderer *QRendererPluginFactory::create(const QString &name, QRenderAspect::RenderType renderMode, const QString &pluginPath)
{
    if (!pluginPath.isEmpty()) {
#if QT_CONFIG(library)
        QCoreApplication::addLibraryPath(pluginPath);
        if (AbstractRenderer *ret = qLoadPlugin<AbstractRenderer, QRendererPlugin>(directLoader(), name, renderMode))
            return ret;
#else
        qWarning() << QObject::tr("Cannot load QRendererPlugin plugin from %1. "
                                  "Library loading is disabled.").arg(pluginPath);
#endif
    }
    return qLoadPlugin<AbstractRenderer, QRendererPlugin>(loader(), name, renderMode);
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
