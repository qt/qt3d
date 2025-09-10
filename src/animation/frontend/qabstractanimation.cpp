// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractanimation.h"
#include "Qt3DAnimation/private/qabstractanimation_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

/*!
    \class Qt3DAnimation::QAbstractAnimation
    \brief An abstract base class for Qt3D animations.
    \inmodule Qt3DAnimation
    \since 5.9
    \inherits QObject

    Qt3DAnimation::QAbstractAnimation is an abstract base class for all animations.
    Qt3DAnimation::QAbstractAnimation can not be directly instantiated, but rather
    through its subclasses. QAbstractAnimation specifies common properties
    for all Qt3D animations, such as animation name and type, current position and animation
    duration, while leaving the actual animating for the subclasses.
*/

/*!
    \qmltype AbstractAnimation
    \brief An abstract base type for Qt3D animations.
    \inqmlmodule Qt3D.Animation
    \since 5.9
    \nativetype Qt3DAnimation::QAbstractAnimation

    AbstractAnimation is an abstract base type for all animations.
    AbstractAnimation can not be directly instantiated, but rather
    through its subtypes. AbstractAnimation specifies common properties
    for all Qt3D animations, such as animation type, current position and animation
    duration, while leaving the actual animating for the subtypes.
*/
/*!
    \enum Qt3DAnimation::QAbstractAnimation::AnimationType

    This enumeration specifies the type of the animation
    \value KeyframeAnimation Simple keyframe animation implementation for QTransform
    \value MorphingAnimation Blend-shape morphing animation
    \value VertexBlendAnimation Vertex-blend animation
*/
/*!
    \property Qt3DAnimation::QAbstractAnimation::animationName
    Holds the name of the animation.
*/
/*!
    \property Qt3DAnimation::QAbstractAnimation::animationType
    Holds the type of the animation.
*/
/*!
    \property Qt3DAnimation::QAbstractAnimation::position
    Holds the current position of the animation.
*/
/*!
    \property Qt3DAnimation::QAbstractAnimation::duration
    Holds the duration of the animation.
*/

/*!
    \qmlproperty string Qt3D.Animation::AbstractAnimation::animationName
    Holds the name of the animation.
*/
/*!
    \qmlproperty enumeration Qt3D.Animation::AbstractAnimation::animationType
    Holds the type of the animation.
    \list
    \li KeyframeAnimation
    \li MorphingAnimation
    \li VertexBlendAnimation
    \endlist
*/
/*!
    \qmlproperty real Qt3D.Animation::AbstractAnimation::position
    Holds the current position of the animation.
*/
/*!
    \qmlproperty real Qt3D.Animation::AbstractAnimation::duration
    Holds the duration of the animation.
*/

QAbstractAnimationPrivate::QAbstractAnimationPrivate(QAbstractAnimation::AnimationType type)
    : QObjectPrivate()
    , m_animationType(type)
    , m_position(0.0f)
    , m_duration(0.0f)
{

}

QAbstractAnimation::QAbstractAnimation(QAbstractAnimationPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{

}

QString QAbstractAnimation::animationName() const
{
    Q_D(const QAbstractAnimation);
    return d->m_animationName;
}

QAbstractAnimation::AnimationType QAbstractAnimation::animationType() const
{
    Q_D(const QAbstractAnimation);
    return d->m_animationType;
}

float QAbstractAnimation::position() const
{
    Q_D(const QAbstractAnimation);
    return d->m_position;
}

float QAbstractAnimation::duration() const
{
    Q_D(const QAbstractAnimation);
    return d->m_duration;
}

void QAbstractAnimation::setAnimationName(const QString &name)
{
    Q_D(QAbstractAnimation);
    if (name != d->m_animationName) {
        d->m_animationName = name;
        emit animationNameChanged(name);
    }
}

void QAbstractAnimation::setPosition(float position)
{
    Q_D(QAbstractAnimation);
    if (!qFuzzyCompare(position, d->m_position)) {
        d->m_position = position;
        emit positionChanged(position);
    }
}

/*!
    Sets the \a duration of the animation.
*/
void QAbstractAnimation::setDuration(float duration)
{
    Q_D(QAbstractAnimation);
    if (!qFuzzyCompare(duration, d->m_duration)) {
        d->m_duration = duration;
        emit durationChanged(duration);
    }
}

} // Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qabstractanimation.cpp"
