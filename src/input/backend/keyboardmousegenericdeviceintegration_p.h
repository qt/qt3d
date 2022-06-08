// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_KEYBOARDMOUSEGENERICDEVICEINTEGRATION_P_H
#define QT3DINPUT_INPUT_KEYBOARDMOUSEGENERICDEVICEINTEGRATION_P_H

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

#include <Qt3DInput/private/qinputdeviceintegration_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class InputHandler;

class KeyboardMouseGenericDeviceIntegration : public Qt3DInput::QInputDeviceIntegration
{
    Q_OBJECT
public:
    explicit KeyboardMouseGenericDeviceIntegration(InputHandler *handleer);
    ~KeyboardMouseGenericDeviceIntegration();

    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) final;
    QAbstractPhysicalDevice *createPhysicalDevice(const QString &name) final;
    QList<Qt3DCore::QNodeId> physicalDevices() const final;
    QAbstractPhysicalDeviceBackendNode *physicalDevice(Qt3DCore::QNodeId id) const final;
    QStringList deviceNames() const final;

private:
    void onInitialize() final;

    InputHandler *m_handler;
};

} // Input

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_KEYBOARDMOUSEGENERICDEVICEINTEGRATION_P_H
