// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DAnimation::QMorphTarget;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *morphTarget) {
        QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
        if (animation)
            animation->parentMorphingAnimation()->addMorphTarget(morphTarget);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
        if (animation)
            return animation->parentMorphingAnimation()->morphTargetList().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
        if (animation) {
            return qobject_cast<Qt3DAnimation::QMorphTarget *>(
                animation->parentMorphingAnimation()->morphTargetList().at(index));
        }
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
        if (animation)
            animation->parentMorphingAnimation()->setMorphTargets({});
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_quick3dmorphinganimation_p.cpp"
