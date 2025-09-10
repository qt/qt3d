// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "axis_p.h"

#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qabstractaxisinput.h>

#include <Qt3DInput/private/qaxis_p.h>
#include <algorithm>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

Axis::Axis()
    : BackendNode(ReadWrite)
    , m_axisValue(0.0f)
{
}

void Axis::cleanup()
{
    QBackendNode::setEnabled(false);
    m_inputs.clear();
    m_axisValue = 0.0f;
}

void Axis::setAxisValue(float axisValue)
{
    if (isEnabled() && (!qFuzzyCompare(axisValue, m_axisValue)))
        m_axisValue = axisValue;
}

void Axis::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const Qt3DInput::QAxis *node = qobject_cast<const Qt3DInput::QAxis *>(frontEnd);
    if (!node)
        return;

    auto ids = Qt3DCore::qIdsForNodes(node->inputs());
    m_inputs = ids;
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE
