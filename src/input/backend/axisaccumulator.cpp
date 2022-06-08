// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "axisaccumulator_p.h"

#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/qaxisaccumulator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

AxisAccumulator::AxisAccumulator()
    : BackendNode(ReadWrite)
    , m_sourceAxisId()
    , m_sourceAxisType(QAxisAccumulator::Velocity)
    , m_scale(1.0f)
    , m_value(0.0f)
    , m_velocity(0.0f)
{
}

void AxisAccumulator::cleanup()
{
    QBackendNode::setEnabled(false);
    m_sourceAxisId = Qt3DCore::QNodeId();
    m_sourceAxisType = QAxisAccumulator::Velocity;
    m_scale = 1.0f;
    m_value = 0.0f;
}

void AxisAccumulator::setValue(float value)
{
    if (isEnabled() && value != m_value)
        m_value = value;
}

void AxisAccumulator::setVelocity(float velocity)
{
    if (isEnabled() && velocity != m_velocity)
        m_velocity = velocity;
}

void AxisAccumulator::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAxisAccumulator *node = qobject_cast<const QAxisAccumulator *>(frontEnd);
    if (!node)
        return;

    m_sourceAxisId = Qt3DCore::qIdForNode(node->sourceAxis());
    m_sourceAxisType = node->sourceAxisType();
    m_scale = node->scale();

    if (firstTime) {
        m_value = 0.f;
        m_velocity = 0.f;
    }
}

void AxisAccumulator::stepIntegration(AxisManager *axisManager, float dt)
{
    Axis *sourceAxis = axisManager->lookupResource(m_sourceAxisId);
    if (!sourceAxis)
        return;

    const float axisValue = sourceAxis->axisValue();
    float newVelocity = 0.0f;
    float newValue = 0.0f;
    switch (m_sourceAxisType) {
    case QAxisAccumulator::Velocity:
        newVelocity = axisValue * m_scale;
        newValue = m_value + newVelocity * dt;
        break;

    case QAxisAccumulator::Acceleration:
        newVelocity = m_velocity + axisValue * m_scale * dt;
        newValue = m_value + newVelocity * dt;
        break;
    }
    setVelocity(newVelocity);
    setValue(newValue);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
