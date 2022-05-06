/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
**
**
**
******************************************************************************/

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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = Qt3DAnimation::QAbstractAnimation;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *animation) {
        QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
        if (animationGroup)
            animationGroup->parentAnimationGroup()->addAnimation(animation);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
        if (animationGroup)
            return animationGroup->parentAnimationGroup()->animationList().count();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
        if (animationGroup) {
            return qobject_cast<Qt3DAnimation::QAbstractAnimation *>(
                animationGroup->parentAnimationGroup()->animationList().at(index));
        }
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
        if (animationGroup) {
            QList<Qt3DAnimation::QAbstractAnimation *> emptyList;
            animationGroup->parentAnimationGroup()->setAnimations(emptyList);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}


} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE
