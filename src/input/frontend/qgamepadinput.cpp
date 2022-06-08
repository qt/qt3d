// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgamepadinput_p.h"

#include <QtCore/QMetaEnum>
#include <QtGamepad/QGamepadManager>

#include <Qt3DInput/private/qabstractphysicaldevice_p.h>

QT_BEGIN_NAMESPACE
namespace Qt3DInput {
class QGamepadInputPrivate : public QAbstractPhysicalDevicePrivate {
public:
    QGamepadInputPrivate()
        : QAbstractPhysicalDevicePrivate()
        , m_deviceId(0)
    {}

    int m_deviceId;
};

static void setValuesFromEnum(QHash<QString, int> &hash, const QMetaEnum &metaEnum)
{
    hash.reserve(metaEnum.keyCount() - 1);
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        if (metaEnum.value(i) != -1)
            hash[QLatin1String(metaEnum.key(i))] = metaEnum.value(i);
    }
}

QGamepadInput::QGamepadInput(Qt3DCore::QNode *parent)
    : QAbstractPhysicalDevice(*new QGamepadInputPrivate, parent)
{
    Q_D(QGamepadInput);
    auto metaObject = QGamepadManager::instance()->metaObject();
    for (int i = metaObject->enumeratorOffset(); i < metaObject->enumeratorCount(); ++i) {
        auto metaEnum = metaObject->enumerator(i);
        if (metaEnum.name() == std::string("GamepadButton"))
            setValuesFromEnum(d->m_buttonsHash, metaEnum);
        else if (metaEnum.name() == std::string("GamepadAxis"))
            setValuesFromEnum(d->m_axesHash, metaEnum);
    }
    connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent, this, [this, d](int deviceId, QGamepadManager::GamepadAxis axis, double value) {
        if (deviceId == d_func()->m_deviceId)
            d->postAxisEvent(axis, value);
    });
    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this, [this, d](int deviceId, QGamepadManager::GamepadButton button, double value) {
        if (deviceId == d_func()->m_deviceId)
            d->postButtonEvent(button, value);
    });
    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this, [this, d](int deviceId, QGamepadManager::GamepadButton button) {
        if (deviceId == d_func()->m_deviceId)
            d->postButtonEvent(button, 0);
    });
}

int QGamepadInput::deviceId() const
{
    return d_func()->m_deviceId;
}

void QGamepadInput::setDeviceId(int deviceId)
{
    Q_D(QGamepadInput);
    if (d->m_deviceId == deviceId)
        return;

    d->m_deviceId = deviceId;
    emit deviceIdChanged();
}

} // Qt3DInput
QT_END_NAMESPACE
