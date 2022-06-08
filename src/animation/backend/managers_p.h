// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_MANAGERS_P_H
#define QT3DANIMATION_ANIMATION_MANAGERS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGlobal>
#include <Qt3DAnimation/private/handle_types_p.h>
#include <Qt3DAnimation/private/animationclip_p.h>
#include <Qt3DAnimation/private/clock_p.h>
#include <Qt3DAnimation/private/blendedclipanimator_p.h>
#include <Qt3DAnimation/private/clipanimator_p.h>
#include <Qt3DAnimation/private/channelmapping_p.h>
#include <Qt3DAnimation/private/channelmapper_p.h>
#include <Qt3DAnimation/private/skeleton_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class ClipBlendNode;

class AnimationClipLoaderManager : public Qt3DCore::QResourceManager<
        AnimationClip,
        Qt3DCore::QNodeId>
{
public:
    AnimationClipLoaderManager() {}
};

class ClockManager : public Qt3DCore::QResourceManager<
    Clock,
    Qt3DCore::QNodeId>
{
public:
    ClockManager() {}
};

class ClipAnimatorManager : public Qt3DCore::QResourceManager<
        ClipAnimator,
        Qt3DCore::QNodeId>
{
public:
    ClipAnimatorManager() {}
};

class BlendedClipAnimatorManager : public Qt3DCore::QResourceManager<
        BlendedClipAnimator,
        Qt3DCore::QNodeId>
{
public:
    BlendedClipAnimatorManager() {}
};

class ChannelMappingManager : public Qt3DCore::QResourceManager<
        ChannelMapping,
        Qt3DCore::QNodeId>
{
public:
    ChannelMappingManager() {}
};

class ChannelMapperManager : public Qt3DCore::QResourceManager<
        ChannelMapper,
        Qt3DCore::QNodeId>
{
public:
    ChannelMapperManager() {}
};

class Q_AUTOTEST_EXPORT ClipBlendNodeManager
{
public:
    ClipBlendNodeManager();
    ~ClipBlendNodeManager();

    bool containsNode(Qt3DCore::QNodeId id) const;
    void appendNode(Qt3DCore::QNodeId id, ClipBlendNode *node);
    ClipBlendNode *lookupNode(Qt3DCore::QNodeId id) const;
    void releaseNode(Qt3DCore::QNodeId id);

private:
    QHash<Qt3DCore::QNodeId, ClipBlendNode *> m_nodes;
};

class SkeletonManager : public Qt3DCore::QResourceManager<
        Skeleton,
        Qt3DCore::QNodeId>
{
public:
    SkeletonManager() {}
};

} // namespace Animation
} // namespace Qt3DAnimation

Q_DECLARE_RESOURCE_INFO(Qt3DAnimation::Animation::AnimationClip, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DAnimation::Animation::ClipAnimator, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DAnimation::Animation::BlendedClipAnimator, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DAnimation::Animation::ChannelMapping, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DAnimation::Animation::ChannelMapper, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DAnimation::Animation::Skeleton, Q_REQUIRES_CLEANUP)

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_MANAGERS_P_H
