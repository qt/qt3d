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

#include "quick3dmorphinganimation_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

QQuick3DMorphingAnimation::QQuick3DMorphingAnimation(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DAnimation::QMorphTarget> QQuick3DMorphingAnimation::morphTargets()
{
    return QQmlListProperty<Qt3DAnimation::QMorphTarget>(this, 0,
                                       &QQuick3DMorphingAnimation::appendMorphTarget,
                                       &QQuick3DMorphingAnimation::morphTargetCount,
                                       &QQuick3DMorphingAnimation::morphTargetAt,
                                       &QQuick3DMorphingAnimation::clearMorphTargets);
}

void QQuick3DMorphingAnimation::appendMorphTarget(QQmlListProperty<Qt3DAnimation::QMorphTarget> *list,
                                                  Qt3DAnimation::QMorphTarget *morphTarget)
{
    QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
    if (animation)
        animation->parentMorphingAnimation()->addMorphTarget(morphTarget);
}

int QQuick3DMorphingAnimation::morphTargetCount(QQmlListProperty<Qt3DAnimation::QMorphTarget> *list)
{
    QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
    if (animation)
        return animation->parentMorphingAnimation()->morphTargetList().count();
    return 0;
}

Qt3DAnimation::QMorphTarget *QQuick3DMorphingAnimation::morphTargetAt(QQmlListProperty<Qt3DAnimation::QMorphTarget> *list,
                                                                   int index)
{
    QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
    if (animation) {
        return qobject_cast<Qt3DAnimation::QMorphTarget *>(
                    animation->parentMorphingAnimation()->morphTargetList().at(index));
    }
    return nullptr;
}

void QQuick3DMorphingAnimation::clearMorphTargets(QQmlListProperty<Qt3DAnimation::QMorphTarget> *list)
{
    QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
    if (animation) {
        QVector<Qt3DAnimation::QMorphTarget *> emptyList;
        animation->parentMorphingAnimation()->setMorphTargets(emptyList);
    }
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE
