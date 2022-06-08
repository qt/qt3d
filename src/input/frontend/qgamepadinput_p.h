// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGAMEPADINPUT_P_H
#define QGAMEPADINPUT_P_H

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

#include <Qt3DInput/qabstractphysicaldevice.h>

#include <Qt3DInput/private/qt3dinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
class QGamepadInputPrivate;

class Q_3DINPUTSHARED_PRIVATE_EXPORT QGamepadInput : public QAbstractPhysicalDevice
{
    Q_OBJECT
    Q_PROPERTY(int deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
public:
    explicit QGamepadInput(Qt3DCore::QNode *parent = nullptr);

    int deviceId() const;
    void setDeviceId(int deviceId);

Q_SIGNALS:
    void deviceIdChanged();

private:
    Q_DECLARE_PRIVATE(QGamepadInput)
};

} // Qt3DInput
QT_END_NAMESPACE

#endif // QGAMEPADINPUT_P_H
