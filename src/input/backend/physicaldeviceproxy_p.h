// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_PHYSICALDEVICEPROXY_P_H
#define QT3DINPUT_INPUT_PHYSICALDEVICEPROXY_P_H

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

#include <Qt3DInput/private/backendnode_p.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDevice;

namespace Input {

class PhysicalDeviceProxyManager;

class Q_AUTOTEST_EXPORT PhysicalDeviceProxy : public BackendNode
{
public:
    PhysicalDeviceProxy();
    void cleanup();

    QString deviceName() const;

    void setManager(PhysicalDeviceProxyManager *manager);
    PhysicalDeviceProxyManager *manager() const;

    // Called from a job to update the frontend
    void setDevice(QAbstractPhysicalDevice *device);
    Qt3DCore::QNodeId physicalDeviceId() const;

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QString m_deviceName;
    PhysicalDeviceProxyManager *m_manager;
    Qt3DCore::QNodeId m_physicalDeviceId;
};

class PhysicalDeviceProxyNodeFunctor: public Qt3DCore::QBackendNodeMapper
{
public:
    explicit PhysicalDeviceProxyNodeFunctor(PhysicalDeviceProxyManager *manager);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    PhysicalDeviceProxyManager *m_manager;
};

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_PHYSICALDEVICEPROXY_P_H
