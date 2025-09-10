// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qinputdeviceplugin_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QInputDevicePlugin::QInputDevicePlugin(QObject *parent)
    : QObject(parent)
{
}

QInputDevicePlugin::~QInputDevicePlugin()
{
}

QInputDeviceIntegration *QInputDevicePlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(key);
    Q_UNUSED(paramList);
    return nullptr;
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qinputdeviceplugin_p.cpp"
