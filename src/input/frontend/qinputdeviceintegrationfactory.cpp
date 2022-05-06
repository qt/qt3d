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
******************************************************************************/

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

