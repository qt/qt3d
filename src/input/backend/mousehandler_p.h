// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_MOUSEHANDLER_H
#define QT3DINPUT_INPUT_MOUSEHANDLER_H

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

#include <Qt3DInput/qmouseevent.h>
#include <Qt3DInput/private/backendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class InputHandler;

class MouseHandler : public BackendNode
{
public:
    MouseHandler();
    ~MouseHandler();

    Qt3DCore::QNodeId mouseDevice() const;
    void setInputHandler(InputHandler *handler);

protected:
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void setDevice(Qt3DCore::QNodeId device);

private:
    Qt3DCore::QNodeId m_mouseDevice;
    InputHandler *m_inputHandler;
};

class MouseHandlerFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit MouseHandlerFunctor(InputHandler *handler);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    InputHandler *m_handler;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_MOUSEHANDLER_H
