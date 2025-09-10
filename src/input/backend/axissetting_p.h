// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_AXISSETTING_H
#define QT3DINPUT_INPUT_AXISSETTING_H

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

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class Q_AUTOTEST_EXPORT AxisSetting : public BackendNode
{
public:
    AxisSetting();
    void cleanup();

    inline float deadZoneRadius() const { return m_deadZoneRadius; }
    inline QList<int> axes() const { return m_axes; }
    inline bool isSmoothEnabled() const { return m_smooth; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:

    float m_deadZoneRadius;
    QList<int> m_axes;
    bool m_smooth;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_AXISSETTING_H
