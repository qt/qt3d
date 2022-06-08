// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_BUTTONAXISINPUT_H
#define QT3DINPUT_INPUT_BUTTONAXISINPUT_H

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

#include <Qt3DInput/private/abstractaxisinput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class Q_AUTOTEST_EXPORT ButtonAxisInput : public AbstractAxisInput
{
public:
    enum UpdateType {
        Accelerate,
        Decelerate
    };

    ButtonAxisInput();
    void cleanup() final;

    inline float scale() const { return m_scale; }
    inline QList<int> buttons() const { return m_buttons; }

    inline float acceleration() const { return m_acceleration < 0.0f ? qInf() : m_acceleration; }
    inline float deceleration() const { return m_deceleration < 0.0f ? qInf() : m_deceleration; }
    inline float speedRatio() const { return m_speedRatio; }
    inline qint64 lastUpdateTime() const { return m_lastUpdateTime; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) final;

    float process(InputHandler *inputHandler, qint64 currentTime) override;

private:
    void updateSpeedRatio(qint64 currentTime, UpdateType type);

    QList<int> m_buttons;
    float m_scale;
    float m_acceleration;
    float m_deceleration;
    float m_speedRatio;
    qint64 m_lastUpdateTime;
};

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_BUTTONAXISINPUT_H
