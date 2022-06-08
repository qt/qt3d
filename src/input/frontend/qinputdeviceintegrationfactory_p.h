// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QINPUTDEVICEINTEGRATIONFACTORY_P_H
#define QT3DINPUT_QINPUTDEVICEINTEGRATIONFACTORY_P_H

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

#include <Qt3DInput/qt3dinput_global.h>
#include <QtCore/QStringList>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputDeviceIntegration;

class QInputDeviceIntegrationFactory
{
public:
    static QStringList keys();
    static QInputDeviceIntegration *create(const QString &name, const QStringList &args);
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTDEVICEINTEGRATIONFACTORY_P_H
