// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QABSTRACTPHYSICALDEVICEBACKENDNODE_P_H
#define QT3DINPUT_QABSTRACTPHYSICALDEVICEBACKENDNODE_P_H

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

#include <Qt3DInput/private/qt3dinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QBackendNodePrivate;
}

namespace Qt3DInput {

class QInputAspect;
class QAbstractPhysicalDeviceBackendNodePrivate;

class Q_3DINPUTSHARED_PRIVATE_EXPORT QAbstractPhysicalDeviceBackendNode : public Input::BackendNode
{
public:
    explicit QAbstractPhysicalDeviceBackendNode(QBackendNode::Mode mode);
    virtual void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void setInputAspect(QInputAspect *aspect);
    QInputAspect *inputAspect() const;

    float processedAxisValue(int axisIdentifier);
    virtual float axisValue(int axisIdentifier) const = 0;
    virtual bool isButtonPressed(int buttonIdentifier) const = 0;

protected:
    QAbstractPhysicalDeviceBackendNode(QAbstractPhysicalDeviceBackendNodePrivate &dd);

    Q_DECLARE_PRIVATE(QAbstractPhysicalDeviceBackendNode)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QABSTRACTPHYSICALDEVICEBACKENDNODE_H
