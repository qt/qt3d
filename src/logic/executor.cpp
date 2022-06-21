// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "executor_p.h"

#include <Qt3DLogic/qframeaction.h>
#include <Qt3DCore/qnode.h>
#include <QtCore/qsemaphore.h>

#include <Qt3DCore/private/qscene_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DLogic {
namespace Logic {

Executor::Executor(QObject *parent)
    : QObject(parent)
    , m_scene(nullptr)
{
}

/*!
   Called from context of main thread
*/
void Executor::processLogicFrameUpdates(const QList<QNodeId> &nodeIds, float dt)
{
    if (!m_scene || nodeIds.isEmpty())
        return;

    const QList<QNode *> nodes = m_scene->lookupNodes(nodeIds);
    for (QNode *node : nodes) {
        QFrameAction *frameAction = qobject_cast<QFrameAction *>(node);
        if (frameAction && frameAction->isEnabled())
            frameAction->onTriggered(dt);
    }
}

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#include "moc_executor_p.cpp"
