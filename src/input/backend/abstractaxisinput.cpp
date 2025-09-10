// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractaxisinput.h"
#include "qabstractaxisinput_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>

#include <Qt3DInput/private/abstractaxisinput_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/qinputdeviceintegration_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

AbstractAxisInput::AbstractAxisInput()
    : BackendNode()
{
}

void AbstractAxisInput::cleanup()
{
    QBackendNode::setEnabled(false);
    m_sourceDevice = Qt3DCore::QNodeId();
}

void AbstractAxisInput::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAbstractAxisInput *node = qobject_cast<const QAbstractAxisInput *>(frontEnd);
    if (!node)
        return;

    m_sourceDevice = Qt3DCore::qIdForNode(node->sourceDevice());
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
