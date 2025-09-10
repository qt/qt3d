// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "axissetting_p.h"

#include <Qt3DInput/qaxissetting.h>

#include <Qt3DInput/private/qaxissetting_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

AxisSetting::AxisSetting()
    : BackendNode()
    , m_deadZoneRadius(0.0f)
    , m_smooth(false)
{
}

void AxisSetting::cleanup()
{
    QBackendNode::setEnabled(false);
    m_deadZoneRadius = 0.0f;
    m_axes.clear();
    m_smooth = false;
}

void AxisSetting::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAxisSetting *node = qobject_cast<const QAxisSetting *>(frontEnd);
    if (!node)
        return;

    m_deadZoneRadius = node->deadZoneRadius();
    m_axes = node->axes();
    m_smooth = node->isSmoothEnabled();
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
