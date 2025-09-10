// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "quick3dkeyframeanimation_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

QQuick3DKeyframeAnimation::QQuick3DKeyframeAnimation(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DCore::QTransform> QQuick3DKeyframeAnimation::keyframes()
{
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DCore::QTransform;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *transform) {
        QQuick3DKeyframeAnimation *keyframeAnimation
            = qobject_cast<QQuick3DKeyframeAnimation *>(list->object);
        if (keyframeAnimation)
            keyframeAnimation->parentKeyframeAnimation()->addKeyframe(transform);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DKeyframeAnimation *keyframeAnimation
            = qobject_cast<QQuick3DKeyframeAnimation *>(list->object);
        if (keyframeAnimation)
            return keyframeAnimation->parentKeyframeAnimation()->keyframeList().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QQuick3DKeyframeAnimation *keyframeAnimation
            = qobject_cast<QQuick3DKeyframeAnimation *>(list->object);
        if (keyframeAnimation)
            return qobject_cast<Qt3DCore::QTransform *>(keyframeAnimation->parentKeyframeAnimation()->keyframeList().at(index));
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QQuick3DKeyframeAnimation *keyframeAnimation
            = qobject_cast<QQuick3DKeyframeAnimation *>(list->object);
        if (keyframeAnimation) {
            QList<Qt3DCore::QTransform *> emptyList;
            keyframeAnimation->parentKeyframeAnimation()->setKeyframes(emptyList);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_quick3dkeyframeanimation_p.cpp"
