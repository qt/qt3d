// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "skeleton_p.h"
#include <Qt3DCore/private/qabstractskeleton_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DAnimation {
namespace Animation {

using namespace Qt3DCore;

// Rather than store backend nodes for the individual joints, the
// animation aspect operates on the vector of local poses as aggregated
// by the skeleton. This allows us to animate a skeleton even when the
// frontend QSkeletonLoader does not instantiate the frontend QJoint nodes.
// It also means we don't need a QChannelMapping for each property of each
// joint.

Skeleton::Skeleton()
    : BackendNode(Qt3DCore::QBackendNode::ReadWrite)
{
}

void Skeleton::cleanup()
{
    m_jointNames.clear();
    m_jointLocalPoses.clear();
}

void Skeleton::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    const Qt3DCore::QAbstractSkeleton *node = qobject_cast<const Qt3DCore::QAbstractSkeleton *>(frontEnd);
    if (!node)
        return;

    auto dnode = Qt3DCore::QAbstractSkeletonPrivate::get(node);

    // TODO: Mark joint info as dirty so we can rebuild any indexes used
    // by the animators and channel mappings.
    m_jointNames = dnode->m_jointNames;
    m_jointLocalPoses = dnode->m_localPoses;
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
