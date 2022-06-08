// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DINPUT_INPUT_ASSIGNKEYBOARDFOCUSJOB_P_H
#define QT3DINPUT_INPUT_ASSIGNKEYBOARDFOCUSJOB_P_H

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

#include <Qt3DInput/qkeyboarddevice.h>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class InputHandler;
class AssignKeyboardFocusJobPrivate;

class AssignKeyboardFocusJob : public Qt3DCore::QAspectJob
{
public:
    explicit AssignKeyboardFocusJob(Qt3DCore::QNodeId keyboardDevice);
    void setInputHandler(InputHandler *handler);
    void run() override;

private:
    Q_DECLARE_PRIVATE(AssignKeyboardFocusJob)

    InputHandler *m_inputHandler;
    const Qt3DCore::QNodeId m_keyboardDevice;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_ASSIGNKEYBOARDFOCUSJOB_P_H
