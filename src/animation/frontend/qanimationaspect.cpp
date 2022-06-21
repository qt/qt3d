// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qanimationaspect.h"
#include "qanimationaspect_p.h"
#include <Qt3DAnimation/qabstractanimationclip.h>
#include <Qt3DAnimation/qabstractchannelmapping.h>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DAnimation/qclipanimator.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qlerpclipblend.h>
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/qclipblendvalue.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/nodefunctor_p.h>
#include <Qt3DAnimation/private/lerpclipblend_p.h>
#include <Qt3DAnimation/private/clipblendvalue_p.h>
#include <Qt3DAnimation/private/additiveclipblend_p.h>
#include <Qt3DAnimation/private/skeleton_p.h>
#include <Qt3DCore/qabstractskeleton.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DAnimation {

QAnimationAspectPrivate::QAnimationAspectPrivate()
    : QAbstractAspectPrivate()
    , m_handler(new Animation::Handler)
{
}

/*!
    \class Qt3DAnimation::QAnimationAspect
    \inherits Qt3DCore::QAbstractAspect
    \inmodule Qt3DAnimation
    \brief Provides key-frame animation capabilities to Qt 3D.
    \since 5.9

    QAnimationAspect provides key-frame animation to Qt 3D.
*/

/*!
 * Constructs a new QAnimationAspect with \a parent.
 */
QAnimationAspect::QAnimationAspect(QObject *parent)
    : QAnimationAspect(*new QAnimationAspectPrivate, parent)
{
}

/*! \internal */
QAnimationAspect::QAnimationAspect(QAnimationAspectPrivate &dd, QObject *parent)
    : QAbstractAspect(dd, parent)
{
    setObjectName(QStringLiteral("Animation Aspect"));
    Q_D(QAnimationAspect);
    qRegisterMetaType<Qt3DAnimation::QAnimationClipLoader*>();
    qRegisterMetaType<Qt3DAnimation::QChannelMapper*>();
    qRegisterMetaType<QList<Qt3DCore::Sqt>>();
    qRegisterMetaType<Qt3DAnimation::QAbstractAnimationClip*>();

    registerBackendType<QAbstractAnimationClip>(
        QSharedPointer<Animation::NodeFunctor<Animation::AnimationClip, Animation::AnimationClipLoaderManager>>::create(d->m_handler.data(),
                                                                                                                        d->m_handler->animationClipLoaderManager()));
    registerBackendType<QClock>(
        QSharedPointer<Animation::NodeFunctor<Animation::Clock, Animation::ClockManager>>::create(d->m_handler.data(),
                                                                                                  d->m_handler->clockManager()));
    registerBackendType<QClipAnimator>(
        QSharedPointer<Animation::NodeFunctor<Animation::ClipAnimator, Animation::ClipAnimatorManager>>::create(d->m_handler.data(),
                                                                                                                d->m_handler->clipAnimatorManager()));
    registerBackendType<QBlendedClipAnimator>(
        QSharedPointer<Animation::NodeFunctor<Animation::BlendedClipAnimator, Animation::BlendedClipAnimatorManager>>::create(d->m_handler.data(),
                                                                                                                              d->m_handler->blendedClipAnimatorManager()));
    registerBackendType<QAbstractChannelMapping>(
        QSharedPointer<Animation::NodeFunctor<Animation::ChannelMapping, Animation::ChannelMappingManager>>::create(d->m_handler.data(),
                                                                                                                    d->m_handler->channelMappingManager()));
    registerBackendType<QChannelMapper>(
        QSharedPointer<Animation::NodeFunctor<Animation::ChannelMapper, Animation::ChannelMapperManager>>::create(d->m_handler.data(),
                                                                                                                  d->m_handler->channelMapperManager()));
    registerBackendType<QLerpClipBlend>(
                QSharedPointer<Animation::ClipBlendNodeFunctor<Animation::LerpClipBlend, Animation::ClipAnimatorManager>>::create(d->m_handler.data(),
                                                                                                                                  d->m_handler->clipBlendNodeManager()));
    registerBackendType<QAdditiveClipBlend>(
                QSharedPointer<Animation::ClipBlendNodeFunctor<Animation::AdditiveClipBlend, Animation::ClipAnimatorManager>>::create(d->m_handler.data(),
                                                                                                                                      d->m_handler->clipBlendNodeManager()));
    registerBackendType<QClipBlendValue>(
                QSharedPointer<Animation::ClipBlendNodeFunctor<Animation::ClipBlendValue, Animation::ClipAnimatorManager>>::create(d->m_handler.data(),
                                                                                                                                   d->m_handler->clipBlendNodeManager()));
    registerBackendType<Qt3DCore::QAbstractSkeleton>(
        QSharedPointer<Animation::NodeFunctor<Animation::Skeleton, Animation::SkeletonManager>>::create(d->m_handler.data(),
                                                                                                        d->m_handler->skeletonManager()));
}

/*! \internal */
QAnimationAspect::~QAnimationAspect()
{
}

/*!
    \internal
 */
std::vector<QAspectJobPtr> QAnimationAspect::jobsToExecute(qint64 time)
{
    Q_D(QAnimationAspect);
    Q_ASSERT(d->m_handler);
    return d->m_handler->jobsToExecute(time);
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("animation", QT_PREPEND_NAMESPACE(Qt3DAnimation), QAnimationAspect)

#include "moc_qanimationaspect.cpp"
