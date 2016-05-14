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

#include "qmousedevice.h"
#include "qmousedevice_p.h"
#include <Qt3DInput/qphysicaldevicecreatedchange.h>
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
/*! \internal */
QMouseDevicePrivate::QMouseDevicePrivate()
    : QAbstractPhysicalDevicePrivate()
    , m_sensitivity(0.1f)
{
}
/*!
 * \qmltype MouseDevice
 * \instantiates Qt3DInput::QMouseDevice
 * \inqmlmodule Qt3D.Input
 * \since 5.5
 * \brief Delegates mouse events to the attached MouseHandler objects.
 * \TODO
 * \sa MouseHandler
 */

/*!
 * \class Qt3DInput::QMouseDevice
 * \inmodule Qt3DInput
 *
 * \brief QMouseDevice is in charge of dispatching mouse events to
 * attached QMouseHandler objects.
 *
 * \since 5.5
 * \sa QMouseHandler
 */
QMouseDevice::QMouseDevice(QNode *parent)
    : QAbstractPhysicalDevice(*new QMouseDevicePrivate, parent)
{
}

/*! \internal */
QMouseDevice::~QMouseDevice()
{
}

int QMouseDevice::axisCount() const
{
    // TO DO: we could have mouse wheel later on
    return 2;
}

int QMouseDevice::buttonCount() const
{
    return 3;
}

QStringList QMouseDevice::axisNames() const
{
    return QStringList()
            << QStringLiteral("X")
            << QStringLiteral("Y");
}

QStringList QMouseDevice::buttonNames() const
{
    return QStringList()
            << QStringLiteral("Left")
            << QStringLiteral("Right")
            << QStringLiteral("Center");
}

int QMouseDevice::axisIdentifier(const QString &name) const
{
    if (name == QLatin1String("X"))
        return X;
    else if (name == QLatin1String("Y"))
        return Y;
    return -1;
}

float QMouseDevice::sensitivity() const
{
    Q_D(const QMouseDevice);
    return d->m_sensitivity;
}

void QMouseDevice::setSensitivity(float value)
{
    Q_D(QMouseDevice);
    if (qFuzzyCompare(value, d->m_sensitivity))
        return;

    d->m_sensitivity = value;
    emit sensitivityChanged(value);
}

void QMouseDevice::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_UNUSED(change);
    // TODO: To be completed as the mouse input aspect takes shape
}

Qt3DCore::QNodeCreatedChangeBasePtr QMouseDevice::createNodeCreationChange() const
{
    auto creationChange = QPhysicalDeviceCreatedChangePtr<QMouseDeviceData>::create(this);
    auto &data = creationChange->data;

    Q_D(const QMouseDevice);
    data.sensitivity = d->m_sensitivity;

    return creationChange;
}

} // namespace Qt3DInput

QT_END_NAMESPACE
