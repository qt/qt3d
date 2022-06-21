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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QAnimationGroup;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *bar) {
        QQuick3DAnimationController *controller = qobject_cast<QQuick3DAnimationController *>(list->object);
        if (controller)
            controller->parentAnimationController()->addAnimationGroup(bar);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DAnimationController *controller = qobject_cast<QQuick3DAnimationController *>(list->object);
        if (controller)
            return controller->parentAnimationController()->animationGroupList().count();
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
