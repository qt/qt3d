/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
