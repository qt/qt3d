// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "quick3danimationcontroller_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

QQuick3DAnimationController::QQuick3DAnimationController(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAnimationGroup> QQuick3DAnimationController::animationGroups()
{
    using qt_size_type = qsizetype;
    using ListContentType = QAnimationGroup;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *bar) {
        QQuick3DAnimationController *controller = qobject_cast<QQuick3DAnimationController *>(list->object);
        if (controller)
            controller->parentAnimationController()->addAnimationGroup(bar);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DAnimationController *controller = qobject_cast<QQuick3DAnimationController *>(list->object);
        if (controller)
            return controller->parentAnimationController()->animationGroupList().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QQuick3DAnimationController *controller = qobject_cast<QQuick3DAnimationController *>(list->object);
        if (controller)
            return qobject_cast<QAnimationGroup *>(controller->parentAnimationController()->getGroup(index));
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QQuick3DAnimationController *controller = qobject_cast<QQuick3DAnimationController *>(list->object);
        if (controller)
            controller->parentAnimationController()->setAnimationGroups({});
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}


} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#include "moc_quick3danimationcontroller_p.cpp"
