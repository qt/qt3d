/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qinputdeviceintegrationfactory_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include <Qt3DInput/private/qinputdeviceintegration_p.h>
#include <Qt3DInput/private/qinputdeviceplugin_p.h>
#include <QtCore/private/qfactoryloader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QInputDevicePlugin_iid, QLatin1String("/3dinputdevices"), Qt::CaseInsensitive))
#if QT_CONFIG(library)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, directLoader, (QInputDevicePlugin_iid, QLatin1String(""), Qt::CaseInsensitive))
#endif

QStringList QInputDeviceIntegrationFactory::keys(const QString &pluginPath)
{
    QStringList list;
    if (!pluginPath.isEmpty()) {
#if QT_CONFIG(library)
        QCoreApplication::addLibraryPath(pluginPath);
        list = directLoader()->keyMap().values();
        if (!list.isEmpty()) {
            const QString postFix = QStringLiteral(" (from ")
                    + QDir::toNativeSeparators(pluginPath)
                    + QLatin1Char(')');
            const QStringList::iterator end = list.end();
            for (QStringList::iterator it = list.begin(); it != end; ++it)
                (*it).append(postFix);
        }
#else
    qWarning() << QInputDeviceIntegration::tr("Cannot query QInputDeviceIntegration plugins at %1. "
                                              "Library loading is disabled.").arg(pluginPath);
#endif
    }
    list.append(loader()->keyMap().values());
    return list;
}

QInputDeviceIntegration *QInputDeviceIntegrationFactory::create(const QString &name, const QStringList &args, const QString &pluginPath)
{
    if (!pluginPath.isEmpty()) {
#if QT_CONFIG(library)
        QCoreApplication::addLibraryPath(pluginPath);
        if (QInputDeviceIntegration *ret = qLoadPlugin<QInputDeviceIntegration, QInputDevicePlugin>(directLoader(), name, args))
            return ret;
#else
        qWarning() << QInputDeviceIntegration::tr("Cannot load QInputDeviceIntegration plugin from "
                                                  "%1. Library loading is disabled.")
                      .arg(pluginPath);
#endif
    }
    return qLoadPlugin<QInputDeviceIntegration, QInputDevicePlugin>(loader(), name, args);
}

} // Qt3DInput

QT_END_NAMESPACE

