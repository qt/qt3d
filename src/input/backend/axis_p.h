// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_AXIS_H
#define QT3DINPUT_INPUT_AXIS_H

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
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class Q_AUTOTEST_EXPORT Axis : public BackendNode
{
public:
    Axis();
    void cleanup();
    inline QList<Qt3DCore::QNodeId> inputs() const { return m_inputs; }
    inline float axisValue() const { return m_axisValue; }
    void setAxisValue(float axisValue);
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QList<Qt3DCore::QNodeId> m_inputs;
    float m_axisValue;
};

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_AXIS_H
