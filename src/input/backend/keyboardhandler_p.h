// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_KEYBOARDHANDLER_P_H
#define QT3DINPUT_INPUT_KEYBOARDHANDLER_P_H

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

#include <Qt3DInput/qkeyevent.h>
#include <Qt3DInput/private/backendnode_p.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class InputHandler;

class Q_AUTOTEST_EXPORT KeyboardHandler : public BackendNode
{
public:
    KeyboardHandler();

    Qt3DCore::QNodeId keyboardDevice() const;
    void setInputHandler(InputHandler *handler);
    void setFocus(bool focus);

    inline bool focus() const { return m_focus; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

protected:
    void requestFocus();
    void setSourcerDevice(Qt3DCore::QNodeId device);

private:
    InputHandler *m_inputHandler;
    Qt3DCore::QNodeId m_keyboardDevice;
    bool m_focus;
};

class KeyboardHandlerFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit KeyboardHandlerFunctor(InputHandler *handler);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    InputHandler *m_handler;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_KEYBOARDHANDLER_P_H
