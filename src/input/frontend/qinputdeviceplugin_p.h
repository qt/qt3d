// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QINPUTDEVICEPLUGIN_P_H
#define QT3DINPUT_QINPUTDEVICEPLUGIN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QObject>
#include <QtCore/QFactoryInterface>
#include <QtCore/QtPlugin>

#include <Qt3DInput/private/qt3dinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputDeviceIntegration;

class Q_3DINPUTSHARED_PRIVATE_EXPORT QInputDevicePlugin : public QObject
{
    Q_OBJECT
public:
    explicit QInputDevicePlugin(QObject *parent = nullptr);
    ~QInputDevicePlugin();

    virtual QInputDeviceIntegration *create(const QString &key, const QStringList &paramList);
};

} // Qt3DInput

#define QInputDevicePlugin_iid "org.qt-project.Qt3DInput.QInputDevicePlugin 5.6"

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTDEVICEPLUGIN_P_H
