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

#include "qaxisinput.h"
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisInputPrivate : public Qt3DCore::QNodePrivate
{
public:
    QAxisInputPrivate()
        : Qt3DCore::QNodePrivate()
        , m_sourceDevice(Q_NULLPTR)
        , m_scale(0.0f)
        , m_axis(-1)
    {}

    QVariantList m_keys;
    QAbstractPhysicalDevice *m_sourceDevice;
    float m_scale;
    int m_axis;
};

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

QAxisInput::~QAxisInput()
{
    QNode::cleanup();
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

void QAxisInput::setKeys(const QVariantList &keys)
{
    Q_D(QAxisInput);
    if (keys != d->m_keys) {
        d->m_keys = keys;
        emit keysChanged(keys);
    }
}

QVariantList QAxisInput::keys() const
{
    Q_D(const QAxisInput);
    return d->m_keys;
}

void QAxisInput::copy(const Qt3DCore::QNode *ref)
{
    QNode::copy(ref);
    const QAxisInput *input = static_cast<const QAxisInput *>(ref);
    d_func()->m_sourceDevice = qobject_cast<QAbstractPhysicalDevice *>(QNode::clone(input->d_func()->m_sourceDevice));
    d_func()->m_keys = input->d_func()->m_keys;
    d_func()->m_scale = input->d_func()->m_scale;
    d_func()->m_axis = input->d_func()->m_axis;
}

} // Qt3DInput

QT_END_NAMESPACE
