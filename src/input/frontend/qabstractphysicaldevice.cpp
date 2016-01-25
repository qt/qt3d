/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstractphysicaldevice.h"
#include "qabstractphysicaldevice_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QAbstractPhysicalDevicePrivate::QAbstractPhysicalDevicePrivate()
    : m_axisSettings()
{
}

QAbstractPhysicalDevice::QAbstractPhysicalDevice(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QAbstractPhysicalDevicePrivate, parent)
{
}

QAbstractPhysicalDevice::QAbstractPhysicalDevice(QAbstractPhysicalDevicePrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

QAbstractPhysicalDevice::~QAbstractPhysicalDevice()
{
    Q_ASSERT_X(Qt3DCore::QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3DInput::QAbstractPhysicalDevice subclass didn't call QNode::cleanup in its destructor");
}

int QAbstractPhysicalDevice::axisCount() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_axesHash.size();
}

int QAbstractPhysicalDevice::buttonCount() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_buttonsHash.size();
}

QStringList QAbstractPhysicalDevice::axisNames() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_axesHash.keys();
}

QStringList QAbstractPhysicalDevice::buttonNames() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_buttonsHash.keys();
}

int QAbstractPhysicalDevice::axisIdentifier(const QString &name) const
{
    Q_D(const QAbstractPhysicalDevice);
    auto it = d->m_axesHash.find(name);
    if (it != d->m_axesHash.end())
        return *it;
    return -1;
}

int QAbstractPhysicalDevice::buttonIdentifier(const QString &name) const
{
    Q_D(const QAbstractPhysicalDevice);
    auto it = d->m_buttonsHash.find(name);
    if (it != d->m_buttonsHash.end())
        return *it;
    return -1;
}

void QAbstractPhysicalDevice::addAxisSetting(QAxisSetting *axisSetting)
{
    Q_D(QAbstractPhysicalDevice);
    if (!d->m_axisSettings.contains(axisSetting))
        d->m_axisSettings.push_back(axisSetting);
}

void QAbstractPhysicalDevice::removeAxisSetting(QAxisSetting *axisSetting)
{
    Q_D(QAbstractPhysicalDevice);
    if (d->m_axisSettings.contains(axisSetting))
        d->m_axisSettings.removeOne(axisSetting);
}

QVector<QAxisSetting *> QAbstractPhysicalDevice::axisSettings() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_axisSettings;
}

void QAbstractPhysicalDevice::postAxisEvent(int axis, qreal value)
{
    Q_D(QAbstractPhysicalDevice);
    Qt3DCore::QScenePropertyChangePtr change(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, id()));
    change->setPropertyName("axisEvent");
    change->setValue(QVariant::fromValue(QPair<int, qreal>(axis, value)));
    d->notifyObservers(change);
}

void QAbstractPhysicalDevice::postButtonEvent(int button, qreal value)
{
    Q_D(QAbstractPhysicalDevice);
    Qt3DCore::QScenePropertyChangePtr change(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, id()));
    change->setPropertyName("buttonEvent");
    change->setValue(QVariant::fromValue(QPair<int, qreal>(button, value)));
    d->notifyObservers(change);
}

void QAbstractPhysicalDevice::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAbstractPhysicalDevice *physicalDevice = static_cast<const QAbstractPhysicalDevice *>(ref);
    d_func()->m_axisSettings = physicalDevice->d_func()->m_axisSettings;
    d_func()->m_axesHash = physicalDevice->d_func()->m_axesHash;
    d_func()->m_buttonsHash = physicalDevice->d_func()->m_buttonsHash;
}

}

QT_END_NAMESPACE
