// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_UPDATEAXISACTIONJOB_H
#define QT3DINPUT_INPUT_UPDATEAXISACTIONJOB_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>

#include <Qt3DInput/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDeviceBackendNode;

namespace Input {

class AbstractAxisInput;
class ButtonAxisInput;
class InputHandler;
class UpdateAxisActionJobPrivate;

class UpdateAxisActionJob : public Qt3DCore::QAspectJob
{
public:
    explicit UpdateAxisActionJob(qint64 currentTime, InputHandler *handler, HLogicalDevice handle);
    void run() final;

private:
    Q_DECLARE_PRIVATE(UpdateAxisActionJob)

    void updateAction(LogicalDevice *device);
    bool processActionInput(const Qt3DCore::QNodeId actionInputId);
    void updateAxis(LogicalDevice *device);
    float processAxisInput(const Qt3DCore::QNodeId axisInputId);

    const qint64 m_currentTime;
    InputHandler *m_handler;
    HLogicalDevice m_handle;
};

typedef QSharedPointer<UpdateAxisActionJob> UpdateAxisActionJobPtr;

} // Input

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_UPDATEAXISACTIONJOB_H
