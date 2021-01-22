/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

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
    return QQmlListProperty<QAxis>(this, 0,
                                   &Quick3DLogicalDevice::appendAxis,
                                   &Quick3DLogicalDevice::axesCount,
                                   &Quick3DLogicalDevice::axisAt,
                                   &Quick3DLogicalDevice::clearAxes);
}

QQmlListProperty<QAction> Quick3DLogicalDevice::qmlActions()
{
    return QQmlListProperty<QAction>(this, 0,
                                     &Quick3DLogicalDevice::appendAction,
                                     &Quick3DLogicalDevice::actionCount,
                                     &Quick3DLogicalDevice::actionAt,
                                     &Quick3DLogicalDevice::clearActions);
}

void Quick3DLogicalDevice::appendAxis(QQmlListProperty<QAxis> *list, QAxis *axes)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    device->parentLogicalDevice()->addAxis(axes);
}

QAxis *Quick3DLogicalDevice::axisAt(QQmlListProperty<QAxis> *list, int index)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    return device->parentLogicalDevice()->axes().at(index);
}

int Quick3DLogicalDevice::axesCount(QQmlListProperty<QAxis> *list)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    return device->parentLogicalDevice()->axes().count();
}

void Quick3DLogicalDevice::clearAxes(QQmlListProperty<QAxis> *list)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    const auto axes = device->parentLogicalDevice()->axes();
    for (QAxis *axis : axes)
        device->parentLogicalDevice()->removeAxis(axis);
}

void Quick3DLogicalDevice::appendAction(QQmlListProperty<QAction> *list, QAction *action)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    device->parentLogicalDevice()->addAction(action);
}

QAction *Quick3DLogicalDevice::actionAt(QQmlListProperty<QAction> *list, int index)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    return device->parentLogicalDevice()->actions().at(index);
}

int Quick3DLogicalDevice::actionCount(QQmlListProperty<QAction> *list)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    return device->parentLogicalDevice()->actions().count();
}

void Quick3DLogicalDevice::clearActions(QQmlListProperty<QAction> *list)
{
    Quick3DLogicalDevice *device = qobject_cast<Quick3DLogicalDevice *>(list->object);
    const auto actions = device->parentLogicalDevice()->actions();
    for (QAction *action : actions)
        device->parentLogicalDevice()->removeAction(action);
}


} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
