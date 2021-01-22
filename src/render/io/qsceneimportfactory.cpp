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

#include "qsceneimportfactory_p.h"
#include "qsceneimportplugin_p.h"
#include "qsceneimporter_p.h"

#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QSceneImportFactoryInterface_iid, QLatin1String("/sceneparsers"), Qt::CaseInsensitive))
#ifndef QT_NO_LIBRARY
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, directLoader, (QSceneImportFactoryInterface_iid, QLatin1String(""), Qt::CaseInsensitive))
#endif

QStringList QSceneImportFactory::keys(const QString &pluginPath)
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
        qWarning() << QSceneImporter::tr("Cannot query QSceneImporter plugins at %1. "
                                         "Library loading is disabled.").arg(pluginPath);
#endif
    }
    list.append(loader()->keyMap().values());
    return list;
}

QSceneImporter *QSceneImportFactory::create(const QString &name, const QStringList &args, const QString &pluginPath)
{
    if (!pluginPath.isEmpty()) {
#if QT_CONFIG(library)
        QCoreApplication::addLibraryPath(pluginPath);
        if (QSceneImporter *ret = qLoadPlugin<QSceneImporter, QSceneImportPlugin>(directLoader(), name, args))
            return ret;
#else
        qWarning() << QSceneImporter::tr("Cannot load QSceneImporter plugin from %1. "
                                         "Library loading is disabled.").arg(pluginPath);
#endif
    }
    return qLoadPlugin<QSceneImporter, QSceneImportPlugin>(loader(), name, args);
}

} // namespace Qt3DRender

QT_END_NAMESPACE
