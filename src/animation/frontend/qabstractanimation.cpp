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

#include "qabstractanimation.h"
#include "Qt3DAnimation/private/qabstractanimation_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

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
