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
#include "inputhandler_p.h"
#include "inputmanagers_p.h"

#include <Qt3DInput/qinputaspect.h>
#include <Qt3DInput/private/qinputaspect_p.h>

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qabstractaspect_p.h>

#include <cmath>

QT_BEGIN_NAMESPACE

namespace {

QVector<int> variantListToVector(const QVariantList &list)
{
    QVector<int> v(list.size());
    int i = 0;
    Q_FOREACH (const QVariant &e, list) {
        v[i++] = e.toInt();
    }
    return v;
}

Q_DECL_CONSTEXPR int signum(float v)
{
    return (v > 0.0f) - (v < 0.0f);
}

}

namespace Qt3DInput {

QAbstractPhysicalDeviceBackendNodePrivate::QAbstractPhysicalDeviceBackendNodePrivate(Qt3DCore::QBackendNode::Mode mode)
    : Qt3DCore::QBackendNodePrivate(mode)
    , m_axisSettings()
    , m_inputAspect(Q_NULLPTR)
    , m_enabled(false)
{
}

void QAbstractPhysicalDeviceBackendNodePrivate::addAxisSetting(int axisIdentifier, Qt3DCore::QNodeId axisSettingsId)
{
    Input::AxisIdSetting axisIdSetting;
    axisIdSetting.m_axisIdentifier = axisIdentifier;
    axisIdSetting.m_axisSettingsId = axisSettingsId;

    // Replace if already present, otherwise append
    bool replaced = false;
    QVector<Input::AxisIdSetting>::iterator it;
    QVector<Input::AxisIdSetting>::iterator end = m_axisSettings.end();
    for (it = m_axisSettings.begin(); it != end; ++it) {
        if (it->m_axisIdentifier == axisIdentifier) {
            *it = axisIdSetting;
            replaced = true;
            break;
        }
    }

    if (!replaced)
        m_axisSettings.push_back(axisIdSetting);
}

void QAbstractPhysicalDeviceBackendNodePrivate::removeAxisSetting(Qt3DCore::QNodeId axisSettingsId)
{
    QVector<Input::AxisIdSetting>::iterator it;
    for (it = m_axisSettings.begin(); it != m_axisSettings.end(); ++it) {
        if (it->m_axisSettingsId == axisSettingsId)
            m_axisSettings.erase(it);
    }
}

Input::MovingAverage &QAbstractPhysicalDeviceBackendNodePrivate::getOrCreateFilter(int axisIdentifier)
{
    QVector<Input::AxisIdFilter>::iterator it;
    QVector<Input::AxisIdFilter>::iterator end = m_axisFilters.end();
    for (it = m_axisFilters.begin(); it != end; ++it) {
        if (it->m_axisIdentifier == axisIdentifier)
            return it->m_filter;
    }

    Input::AxisIdFilter axisIdFilter;
    axisIdFilter.m_axisIdentifier = axisIdentifier;
    m_axisFilters.push_back(axisIdFilter);
    return m_axisFilters.last().m_filter;
}

Input::AxisSetting *QAbstractPhysicalDeviceBackendNodePrivate::getAxisSetting(Qt3DCore::QNodeId axisSettingId) const
{
    Q_Q(const QAbstractPhysicalDeviceBackendNode);
    QInputAspectPrivate *aspectPrivate = static_cast<QInputAspectPrivate *>(Qt3DCore::QAbstractAspectPrivate::get(q->inputAspect()));
    Input::InputHandler *handler = aspectPrivate->m_inputHandler.data();
    Input::AxisSetting *axisSetting = handler->axisSettingManager()->getOrCreateResource(axisSettingId);
    return axisSetting;
}

QAbstractPhysicalDeviceBackendNode::QAbstractPhysicalDeviceBackendNode(QBackendNode::Mode mode)
    : Qt3DCore::QBackendNode(*new QAbstractPhysicalDeviceBackendNodePrivate(mode))
{
}

QAbstractPhysicalDeviceBackendNode::QAbstractPhysicalDeviceBackendNode(QAbstractPhysicalDeviceBackendNodePrivate &dd)
    : Qt3DCore::QBackendNode(dd)
{
}

void QAbstractPhysicalDeviceBackendNode::updateFromPeer(Qt3DCore::QNode *peer)
{
    Q_D(QAbstractPhysicalDeviceBackendNode);
    QAbstractPhysicalDevice *physicalDevice = static_cast<QAbstractPhysicalDevice *>(peer);
    d->m_enabled = physicalDevice->isEnabled();
    Q_FOREACH (QAxisSetting *axisSetting, physicalDevice->axisSettings()) {
        // Each axis setting can apply to more than one axis. If an axis is
        // mentioned in more than one setting, we use the last one
        Q_FOREACH (int axisId, variantListToVector(axisSetting->axes()))
            d->addAxisSetting(axisId, axisSetting->id());
    }
}

void QAbstractPhysicalDeviceBackendNode::cleanup()
{
    Q_D(QAbstractPhysicalDeviceBackendNode);
    d->m_enabled = false;
    d->m_axisSettings.clear();
    d->m_axisFilters.clear();
    d->m_inputAspect = Q_NULLPTR;
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
        if (propertyChange->propertyName() == QByteArrayLiteral("axisSettings")) {
            const Qt3DCore::QNodeId axisSettingId = propertyChange->value().value<Qt3DCore::QNodeId>();
            Input::AxisSetting *axisSetting = d->getAxisSetting(axisSettingId);
            Q_FOREACH (int axisId, axisSetting->axes())
                d->addAxisSetting(axisId, axisSettingId);
        }
    } else if (e->type() == Qt3DCore::NodeRemoved) {
        if (propertyChange->propertyName() == QByteArrayLiteral("axisSettings"))
            d->removeAxisSetting(propertyChange->value().value<Qt3DCore::QNodeId>());
    }
}

void QAbstractPhysicalDeviceBackendNode::setInputAspect(QInputAspect *aspect)
{
    Q_D(QAbstractPhysicalDeviceBackendNode);
    d->m_inputAspect = aspect;
}

QInputAspect *QAbstractPhysicalDeviceBackendNode::inputAspect() const
{
    Q_D(const QAbstractPhysicalDeviceBackendNode);
    Q_ASSERT_X(d->m_inputAspect, "QAbstractPhysicalDeviceBackendNode::inputAspect()" , "No input aspect set");
    return d->m_inputAspect;
}

float QAbstractPhysicalDeviceBackendNode::processedAxisValue(int axisIdentifier)
{
    // Find axis settings for this axis (if any)
    Q_D(QAbstractPhysicalDeviceBackendNode);
    Qt3DCore::QNodeId axisSettingId;
    QVector<Input::AxisIdSetting>::const_iterator it;
    QVector<Input::AxisIdSetting>::const_iterator end = d->m_axisSettings.cend();
    for (it = d->m_axisSettings.cbegin(); it != end; ++it) {
        if (it->m_axisIdentifier == axisIdentifier) {
            axisSettingId = it->m_axisSettingsId;
            break;
        }
    }

    const float rawAxisValue = axisValue(axisIdentifier);
    if (axisSettingId.isNull()) {
        // No special processing. Just return the raw value
        return rawAxisValue;
    } else {
        // Process the raw value in accordance with the settings
        Input::AxisSetting *axisSetting = d->getAxisSetting(axisSettingId);
        Q_ASSERT(axisSetting);
        float val = rawAxisValue;

        // Low pass filtering
        if (axisSetting->isFilterEnabled()) {
            // Get the filter corresponding to this axis
            Input::MovingAverage &filter = d->getOrCreateFilter(axisIdentifier);
            filter.addSample(val);
            val = filter.average();
        }

        // Deadzone handling
        const float d = axisSetting->deadZone();
        if (!qFuzzyIsNull(d)) {
            if (std::abs(val) <= d) {
                val = 0.0f;
            } else {
                // Calculate value that goes from 0 to 1 linearly from the boundary of
                // the dead zone up to 1. That is we with a dead zone value of d, we do not
                // want a step change from 0 to d when the axis leaves the deadzone. Instead
                // we want to increase the gradient of the line so that it goes from 0 to 1
                // over the range d to 1. So instead of having y = x, the equation of the
                // line becomes
                //
                // y = x / (1-d) - d / (1-d) = (x - d) / (1 - d)
                //
                // for positive values, and
                //
                // y = x / (1-d) + d / (1-d) = (x + d) / (1 - d)
                //
                // for negative values.
                val = (val - signum(val) * d) / (1.0f - d);
            }
        }

        return val;
    }

}

} // Qt3DInput

QT_END_NAMESPACE
