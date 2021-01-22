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

#include "quick3dvertexblendanimation_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

QQuick3DVertexBlendAnimation::QQuick3DVertexBlendAnimation(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DAnimation::QMorphTarget> QQuick3DVertexBlendAnimation::morphTargets()
{
    return QQmlListProperty<Qt3DAnimation::QMorphTarget>(this, 0,
                                       &QQuick3DVertexBlendAnimation::appendMorphTarget,
                                       &QQuick3DVertexBlendAnimation::morphTargetCount,
                                       &QQuick3DVertexBlendAnimation::morphTargetAt,
                                       &QQuick3DVertexBlendAnimation::clearMorphTargets);
}

void QQuick3DVertexBlendAnimation::appendMorphTarget(
                                            QQmlListProperty<Qt3DAnimation::QMorphTarget> *list,
                                            Qt3DAnimation::QMorphTarget *morphTarget)
{
    QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
    if (animation)
        animation->parentVertexBlendAnimation()->addMorphTarget(morphTarget);
}

int QQuick3DVertexBlendAnimation::morphTargetCount(
                                            QQmlListProperty<Qt3DAnimation::QMorphTarget> *list)
{
    QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
    if (animation)
        return animation->parentVertexBlendAnimation()->morphTargetList().count();
    return 0;
}

Qt3DAnimation::QMorphTarget *QQuick3DVertexBlendAnimation::morphTargetAt(
                                            QQmlListProperty<Qt3DAnimation::QMorphTarget> *list,
                                            int index)
{
    QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
    if (animation) {
        return qobject_cast<Qt3DAnimation::QMorphTarget *>(
                    animation->parentVertexBlendAnimation()->morphTargetList().at(index));
    }
    return nullptr;
}

void QQuick3DVertexBlendAnimation::clearMorphTargets(QQmlListProperty<Qt3DAnimation::QMorphTarget> *list)
{
    QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
    if (animation) {
        QVector<Qt3DAnimation::QMorphTarget *> emptyList;
        animation->parentVertexBlendAnimation()->setMorphTargets(emptyList);
    }
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE
