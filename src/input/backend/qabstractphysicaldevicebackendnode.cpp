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

#include "qabstractphysicaldevicebackendnode.h"
#include "qabstractphysicaldevicebackendnode_p.h"
#include "qabstractphysicaldevice.h"
#include "qaxissetting.h"

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QAbstractPhysicalDeviceBackendNodePrivate::QAbstractPhysicalDeviceBackendNodePrivate(Qt3DCore::QBackendNode::Mode mode)
    : Qt3DCore::QBackendNodePrivate(mode)
    , m_axisSettings()
    , m_enabled(false)
{
}

QAbstractPhysicalDeviceBackendNode::QAbstractPhysicalDeviceBackendNode(QBackendNode::Mode mode)
    : Qt3DCore::QBackendNode(*new QAbstractPhysicalDeviceBackendNodePrivate(mode))
{
}

void QAbstractPhysicalDeviceBackendNode::updateFromPeer(Qt3DCore::QNode *peer)
{
    Q_D(QAbstractPhysicalDeviceBackendNode);
    QAbstractPhysicalDevice *physicalDevice = static_cast<QAbstractPhysicalDevice *>(peer);
    d->m_enabled = physicalDevice->isEnabled();
    Q_FOREACH (QAxisSetting *axisSetting, physicalDevice->axisSettings())
        d->m_axisSettings.push_back(axisSetting->id());
}

void QAbstractPhysicalDeviceBackendNode::cleanup()
{
    Q_D(QAbstractPhysicalDeviceBackendNode);
    d->m_enabled = false;
    d->m_axisSettings.clear();
}

void QAbstractPhysicalDeviceBackendNode::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    Q_D(QAbstractPhysicalDeviceBackendNode);
    Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
    if (e->type() == Qt3DCore::NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            d->m_enabled = propertyChange->value().toBool();
        }
    } else if (e->type() == Qt3DCore::NodeAdded) {
        if (propertyChange->propertyName() == QByteArrayLiteral("axisSettings"))
            d->m_axisSettings.push_back(propertyChange->value().value<Qt3DCore::QNodeId>());
    } else if (e->type() == Qt3DCore::NodeRemoved) {
        if (propertyChange->propertyName() == QByteArrayLiteral("axisSettings"))
            d->m_axisSettings.removeOne(propertyChange->value().value<Qt3DCore::QNodeId>());
    }
}

QAbstractPhysicalDeviceBackendNode::QAbstractPhysicalDeviceBackendNode(QAbstractPhysicalDeviceBackendNodePrivate &dd)
    : Qt3DCore::QBackendNode(dd)
{
}

} // Qt3DInput

QT_END_NAMESPACE
