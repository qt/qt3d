// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "updateskinningpalettejob_p.h"
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

UpdateSkinningPaletteJob::UpdateSkinningPaletteJob()
    : Qt3DCore::QAspectJob()
    , m_nodeManagers(nullptr)
    , m_root()
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateSkinningPalette, 0)
}

UpdateSkinningPaletteJob::~UpdateSkinningPaletteJob()
{
}

void UpdateSkinningPaletteJob::run()
{
    auto armatureManager = m_nodeManagers->armatureManager();
    if (armatureManager->count() == 0)
        return;

    // TODO: Decompose this job across several jobs, say one per skeleton so
    // that it can be done in parallel

    // Update the local pose transforms of JointInfo's in Skeletons from
    // the set of dirty joints.
    for (const auto &jointHandle : std::as_const(m_dirtyJoints)) {
        Joint *joint = m_nodeManagers->jointManager()->data(jointHandle);
        Q_ASSERT(joint);
        Skeleton *skeleton = m_nodeManagers->skeletonManager()->data(joint->owningSkeleton());
        Q_ASSERT(skeleton);
        if (skeleton->isEnabled() && joint->isEnabled())
            skeleton->setLocalPose(jointHandle, joint->localPose());
    }

    // Find all the armature components and update their skinning palettes
    QList<HArmature> dirtyArmatures;
    m_root->traverse([&dirtyArmatures](Entity *entity) {
        const auto armatureHandle = entity->componentHandle<Armature>();
        if (!armatureHandle.isNull() && !dirtyArmatures.contains(armatureHandle))
            dirtyArmatures.push_back(armatureHandle);
    });

    // Update the skeleton for each dirty armature
    auto skeletonManager = m_nodeManagers->skeletonManager();
    for (const auto &armatureHandle : std::as_const(dirtyArmatures)) {
        auto armature = armatureManager->data(armatureHandle);
        Q_ASSERT(armature);

        auto skeletonId = armature->skeletonId();
        auto skeleton = skeletonManager->lookupResource(skeletonId);
        Q_ASSERT(skeleton);

        const QVector<QMatrix4x4> &skinningPalette = skeleton->calculateSkinningMatrixPalette();
        armature->skinningPaletteUniform().setData(skinningPalette);
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
