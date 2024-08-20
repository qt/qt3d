// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qadditiveclipblend.h"
#include "qadditiveclipblend_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {


/*!
    \qmltype AdditiveClipBlend
    \nativetype Qt3DAnimation::QAdditiveClipBlend
    \inqmlmodule Qt3D.Animation

    \since 5.9

    \brief Performs an additive blend of two animation clips based on an additive factor.

    QAdditiveClipBlend can be useful to create advanced animation effects based on
    individual animation clips. For example, if you:

    \list
    \li set the baseClip property to a normal walk cycle animation clip and
    \li set the additiveClip property to a shaking head difference clip,
    \endlist

    then adjusting the additiveFactor property will control how much of the additiveClip gets added
    on to the baseClip. This has he effect that with an additiveFactor of zero, this blend node will
    yield the original walk cycle clip. With an additiveFactor of 1, it will yield the walk cycle
    including a shaking head animation.

    The blending operation implemented by this class is:

    \badcode
        resultClip = baseClip + additiveFactor * additiveClip
    \endcode

    There is nothing stopping you from using values for the additiveFacor property outside the 0 to
    1 range, but please be aware that the input animation clips may not be authored in such a way
    for this to make sense.

    \sa BlendedClipAnimator
*/

/*!
    \class Qt3DAnimation::QAdditiveClipBlend
    \inherits Qt3DAnimation::QAbstractClipBlendNode

    \inmodule Qt3DAnimation
    \since 5.9

    \brief Performs an additive blend of two animation clips based on an additive factor.

    QAdditiveClipBlend can be useful to create advanced animation effects based on
    individual animation clips. For example, if you:

    \list
    \li set the baseClip property to a normal walk cycle animation clip and
    \li set the additiveClip property to a shaking head difference clip,
    \endlist

    then adjusting the additiveFactor property will control how much of the additiveClip gets added
    on to the baseClip. This has he effect that with an additiveFactor of zero, this blend node will
    yield the original walk cycle clip. With an additiveFactor of 1, it will yield the walk cycle
    including a shaking head animation.

    The blending operation implemented by this class is:

    \badcode
        resultClip = baseClip + additiveFactor * additiveClip
    \endcode

    There is nothing stopping you from using values for the additiveFacor property outside the 0 to
    1 range, but please be aware that the input animation clips may not be authored in such a way
    for this to make sense.

    \sa QBlendedClipAnimator
*/

QAdditiveClipBlendPrivate::QAdditiveClipBlendPrivate()
    : QAbstractClipBlendNodePrivate()
    , m_baseClip(nullptr)
    , m_additiveClip(nullptr)
    , m_additiveFactor(0.0f)
{
}

QAdditiveClipBlend::QAdditiveClipBlend(Qt3DCore::QNode *parent)
    : QAbstractClipBlendNode(*new QAdditiveClipBlendPrivate(), parent)
{
}

QAdditiveClipBlend::QAdditiveClipBlend(QAdditiveClipBlendPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractClipBlendNode(dd, parent)
{
}

QAdditiveClipBlend::~QAdditiveClipBlend()
{
}

/*!
    \qmlproperty real Qt3D.Animation::AdditiveClipBlend::additiveFactor

    Specifies the blending factor, typically between 0 and 1, to control the blending of
    two animation clips.
*/
/*!
    \property Qt3DAnimation::QAdditiveClipBlend::additiveFactor

    Specifies the blending factor, typically between 0 and 1, to control the blending of
    two animation clips.
 */
float QAdditiveClipBlend::additiveFactor() const
{
    Q_D(const QAdditiveClipBlend);
    return d->m_additiveFactor;
}

/*!
    \qmlproperty AbstractClipBlendNode Qt3D.Animation::AdditiveClipBlend::baseClip

    This property holds the base animation clip. When the additiveFactor is zero the \a baseClip will
    also be the resulting clip of this blend node.
*/
/*!
    \property Qt3DAnimation::QAdditiveClipBlend::baseClip

    This property holds the base animation clip. When the additiveFactor
    is zero the baseClip will also be the resulting clip of this blend node.
*/
QAbstractClipBlendNode *QAdditiveClipBlend::baseClip() const
{
    Q_D(const QAdditiveClipBlend);
    return d->m_baseClip;
}

/*!
    \qmlproperty AbstractClipBlendNode Qt3D.Animation::AdditiveClipBlend::additiveClip

    This property holds the additive clip to be blended with the baseClip. The amount of blending
    is controlled by the additiveFactor property.
*/
/*!
    \property Qt3DAnimation::QAdditiveClipBlend::additiveClip

    This property holds the additive clip to be blended with the baseClip. The amount of blending
    is controlled by the additiveFactor property.
*/
QAbstractClipBlendNode *QAdditiveClipBlend::additiveClip() const
{
    Q_D(const QAdditiveClipBlend);
    return d->m_additiveClip;
}

void QAdditiveClipBlend::setAdditiveFactor(float additiveFactor)
{
    Q_D(QAdditiveClipBlend);
    if (d->m_additiveFactor == additiveFactor)
        return;

    d->m_additiveFactor = additiveFactor;
    emit additiveFactorChanged(additiveFactor);
}

void QAdditiveClipBlend::setBaseClip(QAbstractClipBlendNode *baseClip)
{
    Q_D(QAdditiveClipBlend);
    if (d->m_baseClip == baseClip)
        return;

    if (d->m_baseClip)
        d->unregisterDestructionHelper(d->m_baseClip);

    if (baseClip && !baseClip->parent())
        baseClip->setParent(this);
    d->m_baseClip = baseClip;

    // Ensures proper bookkeeping
    if (d->m_baseClip)
        d->registerDestructionHelper(d->m_baseClip, &QAdditiveClipBlend::setBaseClip, d->m_baseClip);

    emit baseClipChanged(baseClip);
}

void QAdditiveClipBlend::setAdditiveClip(QAbstractClipBlendNode *additiveClip)
{
    Q_D(QAdditiveClipBlend);
    if (d->m_additiveClip == additiveClip)
        return;

    if (d->m_additiveClip)
        d->unregisterDestructionHelper(d->m_additiveClip);

    if (additiveClip && !additiveClip->parent())
        additiveClip->setParent(this);
    d->m_additiveClip = additiveClip;

    // Ensures proper bookkeeping
    if (d->m_additiveClip)
        d->registerDestructionHelper(d->m_additiveClip, &QAdditiveClipBlend::setAdditiveClip, d->m_additiveClip);
    emit additiveClipChanged(additiveClip);
}

} // Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qadditiveclipblend.cpp"
