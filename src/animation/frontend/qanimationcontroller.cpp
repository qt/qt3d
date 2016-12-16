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

#include "qanimationcontroller.h"
#include "qanimationgroup.h"

#include <private/qanimationcontroller_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAnimationControllerPrivate::QAnimationControllerPrivate()
    : QObjectPrivate()
    , m_activeAnimationGroup(0)
    , m_position(0.0f)
    , m_scaledPosition(0.0f)
    , m_positionScale(1.0f)
    , m_positionOffset(0.0f)
    , m_entity(nullptr)
    , m_recursive(true)
{

}

void QAnimationControllerPrivate::updatePosition(float position)
{
    m_position = position;
    m_scaledPosition = scaledPosition(position);
    if (m_activeAnimationGroup >= 0 && m_activeAnimationGroup < m_animationGroups.size())
        m_animationGroups[m_activeAnimationGroup]->setPosition(m_scaledPosition);
}

float QAnimationControllerPrivate::scaledPosition(float position) const
{
    return m_positionScale * position + m_positionOffset;
}

QAnimationGroup *QAnimationControllerPrivate::findGroup(const QString &name)
{
    for (QAnimationGroup *g : m_animationGroups) {
        if (g->name() == name)
            return g;
    }
    return nullptr;
}

void QAnimationControllerPrivate::extractAnimations()
{
    Q_Q(QAnimationController);
    if (!m_entity)
        return;
    QList<Qt3DAnimation::QAbstractAnimation *> animations
            = m_entity->findChildren<Qt3DAnimation::QAbstractAnimation *>(QString(),
                m_recursive ? Qt::FindChildrenRecursively : Qt::FindDirectChildrenOnly);
    if (animations.size() > 0) {
        for (Qt3DAnimation::QAbstractAnimation *a : animations) {
            QAnimationGroup *group = findGroup(a->animationName());
            if (!group) {
                group = new QAnimationGroup(q);
                group->setName(a->animationName());
                m_animationGroups.push_back(group);
            }
            group->addAnimation(a);
        }
    }
}
void QAnimationControllerPrivate::clearAnimations()
{
    for (Qt3DAnimation::QAnimationGroup *a : m_animationGroups)
        a->deleteLater();
    m_animationGroups.clear();
    m_activeAnimationGroup = 0;
}

QAnimationController::QAnimationController(QObject *parent)
    : QObject(*new QAnimationControllerPrivate, parent)
{

}

QVector<QAnimationGroup *> QAnimationController::animationGroupList()
{
    Q_D(QAnimationController);
    return d->m_animationGroups;
}

int QAnimationController::activeAnimationGroup() const
{
    Q_D(const QAnimationController);
    return d->m_activeAnimationGroup;
}

float QAnimationController::position() const
{
    Q_D(const QAnimationController);
    return d->m_position;
}

float QAnimationController::positionScale() const
{
    Q_D(const QAnimationController);
    return d->m_positionScale;
}

float QAnimationController::positionOffset() const
{
    Q_D(const QAnimationController);
    return d->m_positionOffset;
}

Qt3DCore::QEntity *QAnimationController::entity() const
{
    Q_D(const QAnimationController);
    return d->m_entity;
}

bool QAnimationController::recursive() const
{
    Q_D(const QAnimationController);
    return d->m_recursive;
}

void QAnimationController::setAnimationGroups(const QVector<QAnimationGroup *> &animationGroups)
{
    Q_D(QAnimationController);
    d->m_animationGroups = animationGroups;
    if (d->m_activeAnimationGroup >= d->m_animationGroups.size())
        d->m_activeAnimationGroup = 0;
    d->updatePosition(d->m_position);
}

void QAnimationController::addAnimationGroup(Qt3DAnimation::QAnimationGroup *animationGroup)
{
    Q_D(QAnimationController);
    if (!d->m_animationGroups.contains(animationGroup))
        d->m_animationGroups.push_back(animationGroup);
}

void QAnimationController::removeAnimationGroup(Qt3DAnimation::QAnimationGroup *animationGroup)
{
    Q_D(QAnimationController);
    if (d->m_animationGroups.contains(animationGroup))
        d->m_animationGroups.removeAll(animationGroup);
    if (d->m_activeAnimationGroup >= d->m_animationGroups.size())
        d->m_activeAnimationGroup = 0;
}

void QAnimationController::setActiveAnimationGroup(int index)
{
    Q_D(QAnimationController);
    if (d->m_activeAnimationGroup != index) {
        d->m_activeAnimationGroup = index;
        d->updatePosition(d->m_position);
        emit activeAnimationGroupChanged(index);
    }
}
void QAnimationController::setPosition(float position)
{
    Q_D(QAnimationController);
    if (!qFuzzyCompare(d->m_scaledPosition, d->scaledPosition(position))) {
        d->updatePosition(position);
        emit positionChanged(position);
    }
}

void QAnimationController::setPositionScale(float scale)
{
    Q_D(QAnimationController);
    if (!qFuzzyCompare(d->m_positionScale, scale)) {
        d->m_positionScale = scale;
        emit positionScaleChanged(scale);
    }
}

void QAnimationController::setPositionOffset(float offset)
{
    Q_D(QAnimationController);
    if (!qFuzzyCompare(d->m_positionOffset, offset)) {
        d->m_positionOffset = offset;
        emit positionOffsetChanged(offset);
    }
}

void QAnimationController::setEntity(Qt3DCore::QEntity *entity)
{
    Q_D(QAnimationController);
    if (d->m_entity != entity) {
        d->clearAnimations();
        d->m_entity = entity;
        d->extractAnimations();
        d->updatePosition(d->m_position);
        emit entityChanged(entity);
    }
}

void QAnimationController::setRecursive(bool recursive)
{
    Q_D(QAnimationController);
    if (d->m_recursive != recursive) {
        d->m_recursive = recursive;
        emit recursiveChanged(recursive);
    }
}

int QAnimationController::getAnimationIndex(const QString &name) const
{
    Q_D(const QAnimationController);
    for (int i = 0; i < d->m_animationGroups.size(); ++i) {
        if (d->m_animationGroups[i]->name() == name)
            return i;
    }
    return -1;
}

QAnimationGroup *QAnimationController::getGroup(int index) const
{
    Q_D(const QAnimationController);
    return d->m_animationGroups.at(index);
}

} // Qt3DAnimation

QT_END_NAMESPACE
