// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "loadanimationclipjob_p.h"

#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qanimationcliploader_p.h>
#include <Qt3DAnimation/private/animationclip_p.h>
#include <Qt3DAnimation/private/qabstractanimationclip_p.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class LoadAnimationClipJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    LoadAnimationClipJobPrivate() { }
    ~LoadAnimationClipJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QList<AnimationClip *> m_updatedNodes;
};

LoadAnimationClipJob::LoadAnimationClipJob()
    : Qt3DCore::QAspectJob(*new LoadAnimationClipJobPrivate)
    , m_animationClipHandles()
    , m_handler(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LoadAnimationClip, 0)
}

void LoadAnimationClipJob::addDirtyAnimationClips(const QVector<HAnimationClip> &animationClipHandles)
{
    for (const auto &handle : animationClipHandles) {
        if (!m_animationClipHandles.contains(handle))
            m_animationClipHandles.push_back(handle);
    }
}

void LoadAnimationClipJob::clearDirtyAnimationClips()
{
    m_animationClipHandles.clear();
}

void LoadAnimationClipJob::run()
{
    Q_ASSERT(m_handler);
    Q_D(LoadAnimationClipJob);

    d->m_updatedNodes.reserve(m_animationClipHandles.size());
    AnimationClipLoaderManager *animationClipManager = m_handler->animationClipLoaderManager();
    for (const auto &animationClipHandle : std::as_const(m_animationClipHandles)) {
        AnimationClip *animationClip = animationClipManager->data(animationClipHandle);
        Q_ASSERT(animationClip);
        animationClip->loadAnimation();
        d->m_updatedNodes.push_back(animationClip);
    }

    clearDirtyAnimationClips();
}

void LoadAnimationClipJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    for (AnimationClip *clip: std::as_const(m_updatedNodes)) {
        QAbstractAnimationClip *node = qobject_cast<QAbstractAnimationClip *>(manager->lookupNode(clip->peerId()));
        if (!node)
            continue;

        QAbstractAnimationClipPrivate *dnode = static_cast<QAbstractAnimationClipPrivate *>(Qt3DCore::QNodePrivate::get(node));
        dnode->setDuration(clip->duration());

        QAnimationClipLoader *loader = qobject_cast<QAnimationClipLoader *>(node);
        if (loader) {
            QAnimationClipLoaderPrivate *dloader = static_cast<QAnimationClipLoaderPrivate *>(dnode);
            dloader->setStatus(clip->status());
        }
    }

    m_updatedNodes.clear();
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
