// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "action_p.h"

#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qabstractactioninput.h>

#include <Qt3DInput/private/qaction_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

Action::Action()
    : BackendNode(ReadWrite)
    , m_actionTriggered(false)
{
}

void Action::cleanup()
{
    QBackendNode::setEnabled(false);
    m_inputs.clear();
    m_actionTriggered = false;
}

void Action::setActionTriggered(bool actionTriggered)
{
    if (isEnabled() && (actionTriggered != m_actionTriggered))
        m_actionTriggered = actionTriggered;
}

void Action::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const Qt3DInput::QAction *node = qobject_cast<const Qt3DInput::QAction *>(frontEnd);
    if (!node)
        return;

    auto ids = Qt3DCore::qIdsForNodes(node->inputs());
    m_inputs = ids;
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE

