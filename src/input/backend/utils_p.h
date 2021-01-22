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

#ifndef QT3DINPUT_INPUT_UTILS_P_H
#define QT3DINPUT_INPUT_UTILS_P_H

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

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/physicaldeviceproxy_p.h>
#include <Qt3DInput/private/qinputdeviceintegration_p.h>
#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

namespace Utils {

template<class InputClass>
QAbstractPhysicalDeviceBackendNode *physicalDeviceForInput(InputClass *input, InputHandler *handler)
{
    // Note: the source device can be a proxy
    // in which case sourceDeviceId should be the proxy's device id
    Qt3DCore::QNodeId sourceDeviceId = input->sourceDevice();

    {
        PhysicalDeviceProxy *deviceProxy = nullptr;
        if ((deviceProxy = handler->physicalDeviceProxyManager()->lookupResource(sourceDeviceId)) != nullptr)
            sourceDeviceId = deviceProxy->physicalDeviceId();
    }

    // Early return if the sourceDeviceId is null
    if (!sourceDeviceId.isNull()) {
        const auto integrations = handler->inputDeviceIntegrations();
        for (QInputDeviceIntegration *integration : integrations) {
            QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend = integration->physicalDevice(sourceDeviceId);
            if (physicalDeviceBackend)
                return physicalDeviceBackend;
        }
    }

    return nullptr;
}

} // Utils

} // Input

} // Qt3DInput

QT_END_NAMESPACE


#endif // QT3DINPUT_INPUT_UTILS_P_H
