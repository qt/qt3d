// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef GENERICDEVICEBACKENDNODE_H
#define GENERICDEVICEBACKENDNODE_H

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

#include <QtCore/QHash>
#include <QtCore/QMutex>

#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDevice;

namespace Input {
class InputHandler;
class GenericDeviceBackendNodeData;
class GenericDeviceBackendNode : public QAbstractPhysicalDeviceBackendNode
{
public:
    explicit GenericDeviceBackendNode(QBackendNode::Mode mode = QBackendNode::ReadWrite);
    ~GenericDeviceBackendNode();
    void updateEvents();

    // QAbstractPhysicalDeviceBackendNode interface
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void cleanup() override;
    float axisValue(int axisIdentifier) const override;
    bool isButtonPressed(int buttonIdentifier) const override;

private:
    QHash<int, qreal> m_axesValues;
    QHash<int, qreal> m_buttonsValues;
    mutable QMutex m_mutex;
};

class GenericDeviceBackendFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit GenericDeviceBackendFunctor(QInputAspect *inputaspect, InputHandler *handler);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    QInputAspect *m_inputAspect;
    InputHandler *m_handler;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // GENERICDEVICEBACKENDNODE_H
