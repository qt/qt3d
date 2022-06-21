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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = Qt3DAnimation::QMorphTarget;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *morphTarget) {
        QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
        if (animation)
            animation->parentMorphingAnimation()->addMorphTarget(morphTarget);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DMorphingAnimation *animation = qobject_cast<QQuick3DMorphingAnimation *>(list->object);
        if (animation)
            return animation->parentMorphingAnimation()->morphTargetList().count();
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
