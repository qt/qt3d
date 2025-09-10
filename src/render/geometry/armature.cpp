// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "armature_p.h"

#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qabstractskeleton.h>

#include <Qt3DCore/private/qarmature_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Armature::Armature()
    : BackendNode(Qt3DCore::QBackendNode::ReadOnly)
{
}

void Armature::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QArmature *node = qobject_cast<const QArmature *>(frontEnd);
    if (!node)
        return;

    m_skeletonId = node->skeleton() ? node->skeleton()->id() : QNodeId{};
}

void Armature::cleanup()
{
    m_skeletonId = Qt3DCore::QNodeId();
    setEnabled(false);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
