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

#include "qaxisinput.h"
#include "qaxisinput_p.h"
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DCore/qnodecreatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
 * \qmltype AxisInput
 * \instantiates Qt3DInput::QAxisInput
 * \inqmlmodule Qt3D.Input
 * \since 5.5
 * \TODO
 *
 */

/*!
 * \class Qt3DInput::QAxisInput
 * \inmodule Qt3DInput
 * \since 5.5
 * \TODO
 *
 */
QAxisInput::QAxisInput(Qt3DCore::QNode *parent)
    : QNode(*new QAxisInputPrivate(), parent)
{
}

void QAxisInput::setSourceDevice(QAbstractPhysicalDevice *sourceDevice)
{
    Q_D(QAxisInput);
    if (d->m_sourceDevice != sourceDevice) {

        if (sourceDevice && !sourceDevice->parent())
            sourceDevice->setParent(this);

        d->m_sourceDevice = sourceDevice;
        emit sourceDeviceChanged(sourceDevice);
    }
}

QAbstractPhysicalDevice *QAxisInput::sourceDevice() const
{
    Q_D(const QAxisInput);
    return d->m_sourceDevice;
}

void QAxisInput::setScale(float scale)
{
    Q_D(QAxisInput);
    if (d->m_scale != scale) {
        d->m_scale = scale;
        emit scaleChanged(scale);
    }
}

float QAxisInput::scale() const
{
    Q_D(const QAxisInput);
    return d->m_scale;
}

void QAxisInput::setAxis(int axis)
{
    Q_D(QAxisInput);
    if (d->m_axis != axis) {
        d->m_axis = axis;
        emit axisChanged(axis);
    }
}

int QAxisInput::axis() const
{
    Q_D(const QAxisInput);
    return d->m_axis;
}

void QAxisInput::setButtons(const QVariantList &buttons)
{
    Q_D(QAxisInput);
    if (buttons != d->m_buttons) {
        d->m_buttons = buttons;
        emit buttonsChanged(buttons);
    }
}

QVariantList QAxisInput::buttons() const
{
    Q_D(const QAxisInput);
    return d->m_buttons;
}

Qt3DCore::QNodeCreatedChangeBasePtr QAxisInput::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QAxisInputData>::create(this);
    auto &data = creationChange->data;

    Q_D(const QAxisInput);
    data.sourceDeviceId = qIdForNode(d->m_sourceDevice);
    data.axis = d->m_axis;
    data.buttons = d->m_buttons;
    data.scale = d->m_scale;

    return creationChange;
}

} // Qt3DInput

QT_END_NAMESPACE
