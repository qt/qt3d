/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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
