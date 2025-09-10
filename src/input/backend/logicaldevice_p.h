// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_LOGICALDEVICE_H
#define QT3DINPUT_INPUT_LOGICALDEVICE_H

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

namespace Input {

class LogicalDeviceManager;

class Q_AUTOTEST_EXPORT LogicalDevice : public BackendNode
{
public:
    LogicalDevice();
    void cleanup();

    inline QList<Qt3DCore::QNodeId> axes() const { return m_axes; }
    inline QList<Qt3DCore::QNodeId> actions() const { return m_actions; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QList<Qt3DCore::QNodeId> m_axes;
    QList<Qt3DCore::QNodeId> m_actions;
};


class LogicalDeviceNodeFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit LogicalDeviceNodeFunctor(LogicalDeviceManager *manager);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    LogicalDeviceManager *m_manager;
};


} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_LOGICALDEVICE_H
