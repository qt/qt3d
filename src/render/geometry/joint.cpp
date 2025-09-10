// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "joint_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DCore/QJoint>
#include <Qt3DCore/private/qjoint_p.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Joint::Joint()
    : BackendNode(Qt3DCore::QBackendNode::ReadOnly)
    , m_localPose()
    , m_jointManager(nullptr)
    , m_skeletonManager(nullptr)
{
}

void Joint::cleanup()
{
    m_inverseBindMatrix.setToIdentity();
    m_localPose = Sqt();
    m_childJointIds.clear();
    m_name.clear();
    m_owningSkeleton = HSkeleton();
    setEnabled(false);
}

void Joint::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const Qt3DCore::QJoint *joint = qobject_cast<const Qt3DCore::QJoint *>(frontEnd);
    if (!joint)
        return;

    bool jointDirty = firstTime;
    if (m_localPose.scale != joint->scale()) {
        m_localPose.scale = joint->scale();
        jointDirty = true;
    }
    if (m_localPose.rotation != joint->rotation()) {
        m_localPose.rotation = joint->rotation();
        jointDirty = true;
    }
    if (m_localPose.translation != joint->translation()) {
        m_localPose.translation = joint->translation();
        jointDirty = true;
    }
    if (m_inverseBindMatrix != joint->inverseBindMatrix()) {
        // Setting the inverse bind matrix should be a rare operation. Usually it is
        // set once and then remains constant for the duration of the skeleton. So just
        // trigger a rebuild of the skeleton's SkeletonData which will include obtaining
        // the inverse bind matrix.
        m_inverseBindMatrix = joint->inverseBindMatrix();
        m_skeletonManager->addDirtySkeleton(SkeletonManager::SkeletonDataDirty, m_owningSkeleton);
    }
    if (m_name != joint->name()) {
        // Joint name doesn't affect anything in the render aspect so no need
        // to mark anything as dirty.
        m_name = joint->name();

        // TODO: Notify other aspects (animation) about the name change.
    }

    Qt3DCore::QNodeIdVector childIds = qIdsForNodes(joint->childJoints());
    std::sort(std::begin(childIds), std::end(childIds));
    if (m_childJointIds != childIds)
        m_childJointIds = childIds;

    if (jointDirty) {
        markDirty(AbstractRenderer::JointDirty);
        m_jointManager->addDirtyJoint(peerId());
    }

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
}


JointFunctor::JointFunctor(AbstractRenderer *renderer,
                           JointManager *jointManager,
                           SkeletonManager *skeletonManager)
    : m_renderer(renderer)
    , m_jointManager(jointManager)
    , m_skeletonManager(skeletonManager)
{
}

Qt3DCore::QBackendNode *JointFunctor::create(Qt3DCore::QNodeId id) const
{
    Joint *backend = m_jointManager->getOrCreateResource(id);
    backend->setRenderer(m_renderer);
    backend->setJointManager(m_jointManager);
    backend->setSkeletonManager(m_skeletonManager);
    return backend;
}

Qt3DCore::QBackendNode *JointFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_jointManager->lookupResource(id);
}

void JointFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_jointManager->removeDirtyJoint(id);
    m_jointManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
