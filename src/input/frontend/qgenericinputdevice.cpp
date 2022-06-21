// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgenericinputdevice_p.h"

#include <Qt3DInput/private/qabstractphysicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {


QGenericInputDevice::QGenericInputDevice(Qt3DCore::QNode *parent)
    : QAbstractPhysicalDevice(parent)
{}

static void setHashFromVariantMap(QHash<QString, int> &hash, const QVariantMap &map)
{
    hash.clear();
    for (QVariantMap::const_iterator it = map.cbegin(); it != map.cend(); ++it) {
        bool ok;
        int value = it.value().toInt(&ok);
        if (ok)
            hash[it.key()] = value;
    }
}

static QVariantMap hash2VariantMap(const QHash<QString, int> &hash)
{
    QVariantMap ret;
    for (QHash<QString, int>::const_iterator it = hash.cbegin(); it != hash.cend(); ++it)
        ret[it.key()] = it.value();
    return ret;
}

QVariantMap QGenericInputDevice::axesMap() const
{
    Q_D(const QAbstractPhysicalDevice);
    return hash2VariantMap(d->m_axesHash);
}

void QGenericInputDevice::setAxesMap(const QVariantMap &axesMap)
{
    Q_D(QAbstractPhysicalDevice);
    setHashFromVariantMap(d->m_axesHash, axesMap);
    emit axesMapChanged();
}

QVariantMap QGenericInputDevice::buttonsMap() const
{
    Q_D(const QAbstractPhysicalDevice);
    return hash2VariantMap(d->m_buttonsHash);
}

void QGenericInputDevice::setButtonsMap(const QVariantMap &buttonsMap)
{
    Q_D(QAbstractPhysicalDevice);
    setHashFromVariantMap(d->m_buttonsHash, buttonsMap);
    emit axesMapChanged();
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qgenericinputdevice_p.cpp"
