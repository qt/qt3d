// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/quick3dphysicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

Quick3DPhysicalDevice::Quick3DPhysicalDevice(QObject *parent) : QObject(parent)
{

}

QQmlListProperty<QAxisSetting> Quick3DPhysicalDevice::axisSettings()
{
    using qt_size_type = qsizetype;
    using ListContentType = QAxisSetting;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *axisSetting) {
        Quick3DPhysicalDevice *device = qobject_cast<Quick3DPhysicalDevice *>(list->object);
        device->parentPhysicalDevice()->addAxisSetting(axisSetting);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DPhysicalDevice *device = qobject_cast<Quick3DPhysicalDevice *>(list->object);
        return device->parentPhysicalDevice()->axisSettings().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DPhysicalDevice *device = qobject_cast<Quick3DPhysicalDevice *>(list->object);
        return device->parentPhysicalDevice()->axisSettings().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DPhysicalDevice *device = qobject_cast<Quick3DPhysicalDevice *>(list->object);
        const auto axisSettings = device->parentPhysicalDevice()->axisSettings();
        for (QAxisSetting *axisSetting : axisSettings)
            device->parentPhysicalDevice()->removeAxisSetting(axisSetting);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_quick3dphysicaldevice_p.cpp"
