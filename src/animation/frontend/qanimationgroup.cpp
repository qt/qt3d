/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qanimationgroup.h"
#include "Qt3DAnimation/private/qanimationgroup_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAnimationGroupPrivate::QAnimationGroupPrivate()
    : QObjectPrivate()
    , m_position(0.0f)
    , m_duration(0.0f)
{

}

void QAnimationGroupPrivate::updatePosition(float position)
{
    m_position = position;
    for (QAbstractAnimation *aa : m_animations)
        aa->setPosition(position);
}

QAnimationGroup::QAnimationGroup(QObject *parent)
    : QObject(*new QAnimationGroupPrivate, parent)
{

}

QString QAnimationGroup::name() const
{
    Q_D(const QAnimationGroup);
    return d->m_name;
}

QVector<Qt3DAnimation::QAbstractAnimation *> QAnimationGroup::animationList()
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

void QAnimationGroup::setAnimations(const QVector<Qt3DAnimation::QAbstractAnimation *> &animations)
{
    Q_D(QAnimationGroup);
    d->m_animations = animations;
    d->m_duration = 0.0f;
    for (const Qt3DAnimation::QAbstractAnimation *a : animations)
        d->m_duration = qMax(d->m_duration, a->duration());
}

void QAnimationGroup::addAnimation(QAbstractAnimation *animation)
{
    Q_D(QAnimationGroup);
    d->m_animations.push_back(animation);
    d->m_duration = qMax(d->m_duration, animation->duration());
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
