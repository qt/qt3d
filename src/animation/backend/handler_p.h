// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_HANDLER_H
#define QT3DANIMATION_ANIMATION_HANDLER_H

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
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

#if defined(QT_BUILD_INTERNAL)
class tst_Handler;
#endif

namespace Qt3DAnimation {
namespace Animation {

class AnimationClip;
class AnimationClipLoaderManager;
class ClockManager;
class ClipAnimator;
class ClipAnimatorManager;
class BlendedClipAnimator;
class BlendedClipAnimatorManager;
class ChannelMapping;
class ChannelMappingManager;
class ChannelMapper;
class ChannelMapperManager;
class ClipBlendNodeManager;
class SkeletonManager;

class FindRunningClipAnimatorsJob;
class LoadAnimationClipJob;
class EvaluateClipAnimatorJob;
class BuildBlendTreesJob;
class EvaluateBlendClipAnimatorJob;

using BuildBlendTreesJobPtr = QSharedPointer<BuildBlendTreesJob>;
using EvaluateBlendClipAnimatorJobPtr = QSharedPointer<EvaluateBlendClipAnimatorJob>;

class Q_AUTOTEST_EXPORT Handler
{
public:
    Handler();
    ~Handler();

    enum DirtyFlag {
        AnimationClipDirty,
        ChannelMappingsDirty,
        ClipAnimatorDirty,
        BlendedClipAnimatorDirty
    };

    qint64 simulationTime() const { return m_simulationTime; }

    void setDirty(DirtyFlag flag, Qt3DCore::QNodeId nodeId);

    void setClipAnimatorRunning(const HClipAnimator &handle, bool running);
    QVector<HClipAnimator> runningClipAnimators() const { return m_runningClipAnimators; }

    void setBlendedClipAnimatorRunning(const HBlendedClipAnimator &handle, bool running);
    QVector<HBlendedClipAnimator> runningBlenndedClipAnimators() const { return m_runningBlendedClipAnimators; }

    AnimationClipLoaderManager *animationClipLoaderManager() const noexcept { return m_animationClipLoaderManager.data(); }
    ClockManager *clockManager() const noexcept { return m_clockManager.data(); }
    ClipAnimatorManager *clipAnimatorManager() const noexcept { return m_clipAnimatorManager.data(); }
    BlendedClipAnimatorManager *blendedClipAnimatorManager() const noexcept { return m_blendedClipAnimatorManager.data(); }
    ChannelMappingManager *channelMappingManager() const noexcept { return m_channelMappingManager.data(); }
    ChannelMapperManager *channelMapperManager() const noexcept { return m_channelMapperManager.data(); }
    ClipBlendNodeManager *clipBlendNodeManager() const noexcept { return m_clipBlendNodeManager.data(); }
    SkeletonManager *skeletonManager() const noexcept { return m_skeletonManager.data(); }

    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time);

    void cleanupHandleList(QVector<HAnimationClip> *clips);
    void cleanupHandleList(QVector<HClipAnimator> *animators);
    void cleanupHandleList(QVector<HBlendedClipAnimator> *animators);

private:
    QMutex m_mutex;
    QScopedPointer<AnimationClipLoaderManager> m_animationClipLoaderManager;
    QScopedPointer<ClockManager> m_clockManager;
    QScopedPointer<ClipAnimatorManager> m_clipAnimatorManager;
    QScopedPointer<BlendedClipAnimatorManager> m_blendedClipAnimatorManager;
    QScopedPointer<ChannelMappingManager> m_channelMappingManager;
    QScopedPointer<ChannelMapperManager> m_channelMapperManager;
    QScopedPointer<ClipBlendNodeManager> m_clipBlendNodeManager;
    QScopedPointer<SkeletonManager> m_skeletonManager;

    QVector<HAnimationClip> m_dirtyAnimationClips;
    QVector<HClipAnimator> m_dirtyClipAnimators;
    QVector<HBlendedClipAnimator> m_dirtyBlendedAnimators;

    QVector<HClipAnimator> m_runningClipAnimators;
    QVector<HBlendedClipAnimator> m_runningBlendedClipAnimators;

    QSharedPointer<LoadAnimationClipJob> m_loadAnimationClipJob;
    QSharedPointer<FindRunningClipAnimatorsJob> m_findRunningClipAnimatorsJob;
    QVector<QSharedPointer<EvaluateClipAnimatorJob>> m_evaluateClipAnimatorJobs;
    QVector<EvaluateBlendClipAnimatorJobPtr> m_evaluateBlendClipAnimatorJobs;
    BuildBlendTreesJobPtr m_buildBlendTreesJob;

    qint64 m_simulationTime;

#if defined(QT_BUILD_INTERNAL)
    friend class QT_PREPEND_NAMESPACE(tst_Handler);
#endif
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_HANDLER_H
