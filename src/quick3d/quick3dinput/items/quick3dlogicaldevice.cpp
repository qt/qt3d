// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/quick3dlogicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DLogicalDevice::Quick3DLogicalDevice(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAxis> Quick3DLogicalDevice::qmlAxes()
{
    using qt_size_type = qsizetype;
    using ListContentType = QAxis;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *axes) {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        device->parentLogicalDevice()->addAxis(axes);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        return device->parentLogicalDevice()->axes().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        return device->parentLogicalDevice()->axes().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        const auto axes = device->parentLogicalDevice()->axes();
        for (QAxis *axis : axes)
            device->parentLogicalDevice()->removeAxis(axis);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

QQmlListProperty<QAction> Quick3DLogicalDevice::qmlActions()
{
    using qt_size_type = qsizetype;
    using ListContentType = QAction;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *action) {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        device->parentLogicalDevice()->addAction(action);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        return device->parentLogicalDevice()->actions().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        return device->parentLogicalDevice()->actions().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
        const auto actions = device->parentLogicalDevice()->actions();
        for (QAction *action : actions)
            device->parentLogicalDevice()->removeAction(action);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_quick3dlogicaldevice_p.cpp"
