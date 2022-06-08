// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_MOUSEDEVICE_H
#define QT3DINPUT_INPUT_MOUSEDEVICE_H

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

#include <Qt3DInput/QMouseEvent>

#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputAspect;

namespace Input {

class InputHandler;

class Q_AUTOTEST_EXPORT MouseDevice : public Qt3DInput::QAbstractPhysicalDeviceBackendNode
{
public:
    struct MouseState {

        MouseState()
            : xAxis(0.0f)
            , yAxis(0.0f)
            , wXAxis(0.0f)
            , wYAxis(0.0f)
            , leftPressed(false)
            , rightPressed(false)
            , centerPressed(false)
        {}

        float xAxis;
        float yAxis;
        float wXAxis;
        float wYAxis;
        bool leftPressed;
        bool rightPressed;
        bool centerPressed;
    };

    MouseDevice();
    ~MouseDevice();

    void setInputHandler(InputHandler *handler);
    InputHandler *inputHandler() const;

    float axisValue(int axisIdentifier) const override;
    bool isButtonPressed(int buttonIdentifier) const override;

    void updateMouseEvent(QT_PREPEND_NAMESPACE(QMouseEvent) *events);
#if QT_CONFIG(wheelevent)
    void updateWheelEvent(QT_PREPEND_NAMESPACE(QWheelEvent) *events);
#endif
    void resetMouseAxisState();

    MouseState mouseState() const;
    QPointF previousPos() const;
    bool wasPressed() const;
    float sensitivity() const;
    bool updateAxesContinuously() const;

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    InputHandler *m_inputHandler;

    MouseState m_mouseState;
    QPointF m_previousPos;
    bool m_wasPressed;
    float m_sensitivity;
    bool m_updateAxesContinuously;
};

class MouseDeviceFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit MouseDeviceFunctor(Qt3DInput::QInputAspect *inputAspect, InputHandler *handler);

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

#endif // MOUSEDEVICE_H
