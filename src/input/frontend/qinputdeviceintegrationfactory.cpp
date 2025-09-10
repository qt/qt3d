// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qinputdeviceintegrationfactory_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include <Qt3DInput/private/qinputdeviceintegration_p.h>
#include <Qt3DInput/private/qinputdeviceplugin_p.h>
#include <QtCore/private/qfactoryloader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QInputDevicePlugin_iid, QLatin1String("/3dinputdevices"), Qt::CaseInsensitive))

QStringList QInputDeviceIntegrationFactory::keys()
{
    return loader->keyMap().values();
}

QInputDeviceIntegration *QInputDeviceIntegrationFactory::create(const QString &name, const QStringList &args)
{
    return qLoadPlugin<QInputDeviceIntegration, QInputDevicePlugin>(loader(), name, args);
}

} // Qt3DInput

QT_END_NAMESPACE

