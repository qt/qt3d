// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DAnimation::QAbstractAnimation;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *animation) {
        QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
        if (animationGroup)
            animationGroup->parentAnimationGroup()->addAnimation(animation);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DAnimationGroup *animationGroup = qobject_cast<QQuick3DAnimationGroup *>(list->object);
        if (animationGroup)
            return animationGroup->parentAnimationGroup()->animationList().size();
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

#include "moc_quick3danimationgroup_p.cpp"
