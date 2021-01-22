/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "quick3danimationgroup_p.h"

#include <Qt3DAnimation/qabstractanimation.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

QQuick3DAnimationGroup::QQuick3DAnimationGroup(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DAnimation::QAbstractAnimation> QQuick3DAnimationGroup::animations()
{
    return QQmlListProperty<Qt3DAnimation::QAbstractAnimation>(this, 0,
                                       &QQuick3DAnimationGroup::appendAnimation,
                                       &QQuick3DAnimationGroup::animationCount,
                                       &QQuick3DAnimationGroup::animationAt,
                                       &QQuick3DAnimationGroup::clearAnimation);
}


void QQuick3DAnimationGroup::appendAnimation(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list,
                                             Qt3DAnimation::QAbstractAnimation *animation)
{
    QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
    if (animationGroup)
        animationGroup->parentAnimationGroup()->addAnimation(animation);
}

int QQuick3DAnimationGroup::animationCount(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list)
{
    QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
    if (animationGroup)
        return animationGroup->parentAnimationGroup()->animationList().count();
    return 0;
}

Qt3DAnimation::QAbstractAnimation *QQuick3DAnimationGroup::animationAt(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list, int index)
{
    QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
    if (animationGroup) {
        return qobject_cast<Qt3DAnimation::QAbstractAnimation *>(
                    animationGroup->parentAnimationGroup()->animationList().at(index));
    }
    return nullptr;
}

void QQuick3DAnimationGroup::clearAnimation(QQmlListProperty<Qt3DAnimation::QAbstractAnimation> *list)
{
    QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
    if (animationGroup) {
        QVector<Qt3DAnimation::QAbstractAnimation *> emptyList;
        animationGroup->parentAnimationGroup()->setAnimations(emptyList);
    }
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE
