// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_QKEYBOARDHANDLER_P_H
#define QT3DINPUT_INPUT_QKEYBOARDHANDLER_P_H

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

#include <Qt3DCore/private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QKeyboardHandler;
class QKeyboardDevice;

class QKeyboardHandlerPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QKeyboardHandlerPrivate();
    ~QKeyboardHandlerPrivate();

    void keyEvent(QKeyEvent *event);

    Q_DECLARE_PUBLIC(QKeyboardHandler)
    QKeyboardDevice *m_keyboardDevice;
    bool m_focus;
};

struct QKeyboardHandlerData
{
    Qt3DCore::QNodeId keyboardDeviceId;
    bool focus;
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_QKEYBOARDHANDLER_P_H
