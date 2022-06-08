// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_AXISACCUMULATOR_H
#define QT3DINPUT_INPUT_AXISACCUMULATOR_H

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

#include <Qt3DInput/qaxisaccumulator.h>
#include <Qt3DInput/private/backendnode_p.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class AxisManager;

class Q_AUTOTEST_EXPORT AxisAccumulator : public BackendNode
{
public:
    AxisAccumulator();
    void cleanup();

    Qt3DCore::QNodeId sourceAxisId() const { return m_sourceAxisId; }
    QAxisAccumulator::SourceAxisType sourceAxisType() const { return m_sourceAxisType; }
    float scale() const { return m_scale; }

    float value() const noexcept { return m_value; }
    void setValue(float value);

    float velocity() const noexcept { return  m_velocity; }
    void setVelocity(float velocity);

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void stepIntegration(AxisManager *axisManager, float dt);

private:
    Qt3DCore::QNodeId m_sourceAxisId;
    QAxisAccumulator::SourceAxisType m_sourceAxisType;
    float m_scale;
    float m_value;
    float m_velocity;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_AXISACCUMULATOR_H
