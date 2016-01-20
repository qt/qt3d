/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstractphysicaldevice.h"
#include "qabstractphysicaldevice_p.h"
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*! \internal */
QAbstractPhysicalDevicePrivate::QAbstractPhysicalDevicePrivate()
    : m_axisSettings()
{
}

/*!
 * \qmltype AbstractPhysicalDevice
 * \instantiates Qt3DInput::QAbstractPhysicalDevice
 * \inqmlmodule Qt3D.Input
 * \since 5.6
 * \TODO
 *
 */

/*!
 * \class Qt3DInput::QAbstractPhysicalDevice
 * \inmodule Qt3DInput
 * \since 5.6
 * \TODO
 *
 */
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

void QAbstractPhysicalDevice::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAbstractPhysicalDevice *physicalDevice = static_cast<const QAbstractPhysicalDevice *>(ref);
    d_func()->m_axisSettings = physicalDevice->axisSettings();
}

}

QT_END_NAMESPACE
