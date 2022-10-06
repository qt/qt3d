// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qanimationgroup.h"
#include "Qt3DAnimation/private/qanimationgroup_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

/*!
    \class Qt3DAnimation::QAnimationGroup
    \brief A class grouping animations together.
    \inmodule Qt3DAnimation
    \since 5.9
    \inherits QObject

    Qt3DAnimation::QAnimationGroup class is used to group multiple animations so that
    they can act as one animation. The position set to the group is also set to
    all animations in a group. The duration is the maximum of the individual animations.
    The animations can be any supported animation type and do not have to have the same name.
*/
/*!
    \qmltype AnimationGroup
    \brief A type grouping animations together.
    \inqmlmodule Qt3D.Animation
    \since 5.9
    \inherits QObject

    AnimationGroup type is used to group multiple animations so that
    they can act as one animation. The position set to the group is also set to
    all animations in a group. The duration is the maximum of the individual animations.
    The animations can be any supported animation type and do not have to have the same name.
*/

/*!
    \property Qt3DAnimation::QAnimationGroup::name
    Holds the name of the animation group.
*/
/*!
    \property Qt3DAnimation::QAnimationGroup::position
    Holds the animation position.
*/
/*!
    \property Qt3DAnimation::QAnimationGroup::duration
    Holds the maximum duration of the animations in the group.
    \readonly
*/

/*!
    \qmlproperty string AnimationGroup::name
    Holds the name of the animation group.
*/
/*!
    \qmlproperty real AnimationGroup::position
    Holds the animation position.
*/
/*!
    \qmlproperty real AnimationGroup::duration
    Holds the maximum duration of the animations in the group.
    \readonly
*/
/*!
    \qmlproperty list<AbstractAnimation> AnimationGroup::animations
    Holds the list of animations in the animation group.
*/

QAnimationGroupPrivate::QAnimationGroupPrivate()
    : QObjectPrivate()
    , m_position(0.0f)
    , m_duration(0.0f)
{

}

void QAnimationGroupPrivate::updatePosition(float position)
{
    m_position = position;
    for (QAbstractAnimation *aa : std::as_const(m_animations))
        aa->setPosition(position);
}

/*!
    Constructs an QAnimationGroup with \a parent.
*/
QAnimationGroup::QAnimationGroup(QObject *parent)
    : QObject(*new QAnimationGroupPrivate, parent)
{

}

QString QAnimationGroup::name() const
{
    Q_D(const QAnimationGroup);
    return d->m_name;
}

/*!
    Returns the list of animations in the group.
 */
QList<Qt3DAnimation::QAbstractAnimation *> QAnimationGroup::animationList()
{
    Q_D(QAnimationGroup);
    return d->m_animations;
}

float QAnimationGroup::position() const
{
    Q_D(const QAnimationGroup);
    return d->m_position;
}

float QAnimationGroup::duration() const
{
    Q_D(const QAnimationGroup);
    return d->m_duration;
}

void QAnimationGroup::setName(const QString &name)
{
    Q_D(QAnimationGroup);
    if (d->m_name != name) {
        d->m_name = name;
        emit nameChanged(name);
    }
}

/*!
    Sets the \a animations to the group. Old animations are removed.
 */
void QAnimationGroup::setAnimations(const QList<Qt3DAnimation::QAbstractAnimation *> &animations)
{
    Q_D(QAnimationGroup);
    d->m_animations = animations;
    d->m_duration = 0.0f;
    for (const Qt3DAnimation::QAbstractAnimation *a : animations)
        d->m_duration = qMax(d->m_duration, a->duration());
}

/*!
    Adds new \a animation to the group.
 */
void QAnimationGroup::addAnimation(Qt3DAnimation::QAbstractAnimation *animation)
{
    Q_D(QAnimationGroup);
    if (!d->m_animations.contains(animation)) {
        d->m_animations.push_back(animation);
        d->m_duration = qMax(d->m_duration, animation->duration());
    }
}

/*!
    Removes \a animation from the group.
 */
void QAnimationGroup::removeAnimation(Qt3DAnimation::QAbstractAnimation *animation)
{
    Q_D(QAnimationGroup);
    if (!d->m_animations.contains(animation)) {
        d->m_animations.removeAll(animation);
        if (qFuzzyCompare(d->m_duration, animation->duration())) {
            d->m_duration = 0.0f;
            for (const Qt3DAnimation::QAbstractAnimation *a : std::as_const(d->m_animations))
                d->m_duration = qMax(d->m_duration, a->duration());
        }
    }
}

void QAnimationGroup::setPosition(float position)
{
    Q_D(QAnimationGroup);
    if (!qFuzzyCompare(d->m_position, position)) {
        d->updatePosition(position);
        emit positionChanged(position);
    }
}

} // Qt3DAnimation

QT_END_NAMESPACE

#include "moc_qanimationgroup.cpp"
