// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DAnimation::QMorphTarget;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *morphTarget) {
        QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
        if (animation)
            animation->parentVertexBlendAnimation()->addMorphTarget(morphTarget);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
        if (animation)
            return animation->parentVertexBlendAnimation()->morphTargetList().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
        if (animation) {
            return qobject_cast<Qt3DAnimation::QMorphTarget *>(
                animation->parentVertexBlendAnimation()->morphTargetList().at(index));
        }
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QQuick3DVertexBlendAnimation *animation
            = qobject_cast<QQuick3DVertexBlendAnimation *>(list->object);
        if (animation)
            animation->parentVertexBlendAnimation()->setMorphTargets({});
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_quick3dvertexblendanimation_p.cpp"
