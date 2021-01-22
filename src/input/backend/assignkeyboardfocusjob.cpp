/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "assignkeyboardfocusjob_p.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DInput/qkeyboardhandler.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/job_common_p.h>
#include <Qt3DInput/private/keyboarddevice_p.h>
#include <Qt3DInput/private/keyboardhandler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class AssignKeyboardFocusJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    AssignKeyboardFocusJobPrivate() { }
    ~AssignKeyboardFocusJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QVector<QPair<Qt3DCore::QNodeId, bool>> updates;
};

AssignKeyboardFocusJob::AssignKeyboardFocusJob(Qt3DCore::QNodeId keyboardDevice)
    : QAspectJob(*new AssignKeyboardFocusJobPrivate())
    , m_inputHandler(nullptr)
    , m_keyboardDevice(keyboardDevice)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::AssignKeyboardFocus, 0)
}

void AssignKeyboardFocusJob::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void AssignKeyboardFocusJob::run()
{
    Q_D(AssignKeyboardFocusJob);

    KeyboardDevice *keyboardDevice = m_inputHandler->keyboardDeviceManager()->lookupResource(m_keyboardDevice);
    const auto handles = m_inputHandler->keyboardInputManager()->activeHandles();
    d->updates.reserve(handles.size());

    for (const HKeyboardHandler &handle : handles) {
        KeyboardHandler *input = m_inputHandler->keyboardInputManager()->data(handle);
        Q_ASSERT(input);
        if (input->keyboardDevice() == m_keyboardDevice) {
            bool hasFocus = input->peerId() == keyboardDevice->lastKeyboardInputRequester();
            input->setFocus(hasFocus);
            d->updates.push_back({input->peerId(), hasFocus});
            if (hasFocus)
                keyboardDevice->setCurrentFocusItem(input->peerId());
        }
    }
}

void AssignKeyboardFocusJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    for (const auto &data: qAsConst(updates)) {
        QKeyboardHandler *node = qobject_cast<QKeyboardHandler *>(manager->lookupNode(data.first));
        if (!node)
            continue;

        const bool b = node->blockNotifications(true);
        node->setFocus(data.second);
        node->blockNotifications(b);
    }

    updates.clear();
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
