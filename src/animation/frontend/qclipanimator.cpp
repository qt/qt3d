// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qclipanimator.h"
#include "qclipanimator_p.h"
#include <Qt3DAnimation/qabstractanimationclip.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qclock.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QClipAnimatorPrivate::QClipAnimatorPrivate()
    : Qt3DAnimation::QAbstractClipAnimatorPrivate()
    , m_clip(nullptr)
{
}

bool QClipAnimatorPrivate::canPlay() const
{
    if (m_clip && m_mapper)
        return true;

    qWarning("ClipAnimators need a clip and a mapper to be played");
    return false;
}

/*!
    \qmltype ClipAnimator
    \instantiates Qt3DAnimation::QClipAnimator
    \inqmlmodule Qt3D.Animation
    \inherits AbstractClipAnimator
    \since 5.9

    \brief ClipAnimator is a component providing simple animation playback capabilities.

    An instance of ClipAnimator can be aggregated by an Entity to add the ability to play back
    animation clips and to apply the calculated animation values to properties of QObjects.

    The animation key frame data is provided via the clip property. This can be created
    programmatically with AnimationClip or loaded from file with AnimationClipLoader.

    In order to apply the values played back from the channels of data in the animation clip, the
    clip animator needs to have a ChannelMapper object assigned to the channelMapper property.

    The properties for controlling the animator are provided by the AbstractClipAnimator base
    class.

    \sa {Qt3DAnimation::QAbstractClipAnimator}{AbstractClipAnimator}, {Qt3DAnimation::QAbstractAnimationClip}{AbstractAnimationClip}, {Qt3DAnimation::QChannelMapper}{ChannelMapper}, {Qt3DAnimation::QBlendedClipAnimator}{BlendedClipAnimator}
*/

/*!
    \class Qt3DAnimation::QClipAnimator
    \inherits Qt3DAnimation::QAbstractClipAnimator

    \inmodule Qt3DAnimation
    \since 5.9

    \brief QClipAnimator is a component providing simple animation playback capabilities.

    An instance of QClipAnimator can be aggregated by a QEntity to add the ability to play back
    animation clips and to apply the calculated animation values to properties of QObjects.

    The animation key frame data is provided via the clip property. This can be created
    programmatically with QAnimationClip or loaded from file with QAnimationClipLoader.

    In order to apply the values played back from the channels of data in the animation clip, the
    clip animator needs to have a QChannelMapper object assigned to the channelMapper property.

    The properties for controlling the animator are provided by the QAbstractClipAnimator base
    class.

    \sa Qt3DAnimation::QAbstractClipAnimator, Qt3DAnimation::QAbstractAnimationClip,
        Qt3DAnimation::QChannelMapper, Qt3DAnimation::QBlendedClipAnimator
*/

QClipAnimator::QClipAnimator(Qt3DCore::QNode *parent)
    : Qt3DAnimation::QAbstractClipAnimator(*new QClipAnimatorPrivate, parent)
{
}

/*! \internal */
QClipAnimator::QClipAnimator(QClipAnimatorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DAnimation::QAbstractClipAnimator(dd, parent)
{
}

QClipAnimator::~QClipAnimator()
{
}

/*!
    \qmlproperty var Qt3D.Animation::ClipAnimator::clip

    This property holds the animation clip which contains the key frame data to be played back.
    The key frame data can be specified in either an AnimationClip or AnimationClipLoader.
*/
/*!
    \property QClipAnimator::clip

    This property holds the animation clip which contains the key frame data to be played back.
    The key frame data can be specified in either a QAnimationClip or QAnimationClipLoader.
*/
QAbstractAnimationClip *QClipAnimator::clip() const
{
    Q_D(const QClipAnimator);
    return d->m_clip;
}

void QClipAnimator::setClip(QAbstractAnimationClip *clip)
{
    Q_D(QClipAnimator);
    if (d->m_clip == clip)
        return;

    if (d->m_clip)
        d->unregisterDestructionHelper(d->m_clip);

    if (clip && !clip->parent())
        clip->setParent(this);
    d->m_clip = clip;

    // Ensures proper bookkeeping
    if (d->m_clip)
        d->registerDestructionHelper(d->m_clip, &QClipAnimator::setClip, d->m_clip);
    emit clipChanged(clip);
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qclipanimator.cpp"
