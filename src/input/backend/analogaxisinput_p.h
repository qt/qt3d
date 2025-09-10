// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_ANALOGAXISINPUT_H
#define QT3DINPUT_INPUT_ANALOGAXISINPUT_H

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

class Q_AUTOTEST_EXPORT AnalogAxisInput : public AbstractAxisInput
{
public:
    AnalogAxisInput();
    void cleanup() final;

    inline int axis() const { return m_axis; }
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) final;

    float process(InputHandler *inputHandler, qint64 currentTime) override;

private:
    int m_axis;
};

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_ANALOGAXISINPUT_H
