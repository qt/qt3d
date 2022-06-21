// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractanimationclip.h"
#include "qabstractanimationclip_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAbstractAnimationClipPrivate::QAbstractAnimationClipPrivate()
    : Qt3DCore::QNodePrivate()
    , m_duration(0.0f)
{
}

void QAbstractAnimationClipPrivate::setDuration(float duration)
{
    if (qFuzzyCompare(duration, m_duration))
        return;

    Q_Q(QAbstractAnimationClip);
    bool wasBlocked = q->blockNotifications(true);
    m_duration = duration;
    emit q->durationChanged(duration);
    q->blockNotifications(wasBlocked);
}

/*!
    \class Qt3DAnimation::QAbstractAnimationClip
    \inherits Qt3DCore::QNode

    \inmodule Qt3DAnimation
    \since 5.9

    \brief QAbstractAnimationClip is the base class for types providing key frame animation data.

    To utilise the key frame animation framework in the Qt 3D Animation module
    the animator component in use needs to be provided with the key frame animation data. The
    animation data is provided by one of the concrete subclasses of QAbstractAnimationClip:

    \list
    \li Qt3DAnimation::QAnimationClip
    \li Qt3DAnimation::QAnimationClipLoader
    \endlist

    QAnimationClip should be used when you want to create the animation data
    programmatically within your application. The actual data is set with a
    QAnimationClipData value type.

    If you are loading baked animation data from a file, e.g. as created by an
    artist, then use the QAnimationClipLoader class and set its \c source property.

    Once the animation clip has been populated with data using the above
    methods, the read-only duration property will be updated by the Qt 3D Animation
    backend.

    The typical usage of animation clips is:

    \code
    auto animator = new QClipAnimator();
    auto clip = new QAnimationClipLoader();
    clip->setSource(QUrl::fromLocalFile("bounce.json"));
    animator->setClip(clip);
    animator->setChannelMapper(...);
    animator->setRunning(true);
    \endcode

    Animation clips are also used as the leaf node values in animation blend trees:

    \code
    // Create leaf nodes of blend tree
    auto slideClipValue = new QClipBlendValue(
        new QAnimationClipLoader(QUrl::fromLocalFile("slide.json")));
    auto bounceClipValue = new QClipBlendValue(
        new QAnimationClipLoader(QUrl::fromLocalFile("bounce.json")));

    // Create blend tree inner node
    auto additiveNode = new QAdditiveClipBlend();
    additiveNode->setBaseClip(slideClipValue);
    additiveNode->setAdditiveClip(bounceClipValue);
    additiveNode->setAdditiveFactor(0.5f);

    // Run the animator
    auto animator = new QBlendedClipAnimator();
    animator->setBlendTree(additiveNode);
    animator->setChannelMapper(...);
    animator->setRunning(true);
    \endcode

    \sa Qt3DAnimation::QAnimationClip, Qt3DAnimation::QAnimationClipLoader
*/

/*!
    \internal
*/
QAbstractAnimationClip::QAbstractAnimationClip(QAbstractAnimationClipPrivate &dd,
                                               Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

/*!
    Destroys this animation clip.
*/
QAbstractAnimationClip::~QAbstractAnimationClip()
{
}

/*!
    \property QAbstractAnimationClip::duration

    Holds the duration of the animation clip in seconds. Gets updated once the
    animation data is provided to Qt 3D using one of the concrete subclasses.
*/
float QAbstractAnimationClip::duration() const
{
    Q_D(const QAbstractAnimationClip);
    return d->m_duration;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qabstractanimationclip.cpp"
