// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractaxisinput_p.h"

#include <Qt3DInput/qabstractaxisinput.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
 * \qmltype AbstractAxisInput
 * \inqmlmodule Qt3D.Input
 * \brief QML frontend for abstract QAbstractAxisInput C++ class.
 * \since 5.5
 *
 */

/*!
 * \class Qt3DInput::QAbstractAxisInput
 * \inheaderfile Qt3DInput/QAbstractAxisInput
 * \inmodule Qt3DInput
 * \brief QAbstractActionInput is the base class for all Axis Input.
 * \since 5.5
 *
 */

/*!
    \qmlproperty AbstractPhysicalDevice Qt3D.Input::AbstractAxisInput::sourceDevice
*/

/*!
    \internal
 */
QAbstractAxisInput::QAbstractAxisInput(QAbstractAxisInputPrivate &dd, Qt3DCore::QNode *parent)
    : QNode(dd, parent)
{
}

/*!
    \property Qt3DInput::QAbstractAxisInput::sourceDevice

    The source device for the QAbstractAxisInput.
*/

void QAbstractAxisInput::setSourceDevice(QAbstractPhysicalDevice *sourceDevice)
{
    Q_D(QAbstractAxisInput);
    if (d->m_sourceDevice != sourceDevice) {

        if (d->m_sourceDevice)
            d->unregisterDestructionHelper(d->m_sourceDevice);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (sourceDevice && !sourceDevice->parent())
            sourceDevice->setParent(this);

        d->m_sourceDevice = sourceDevice;
        // Ensures proper bookkeeping
        if (d->m_sourceDevice)
            d->registerDestructionHelper(sourceDevice, &QAbstractAxisInput::setSourceDevice, d->m_sourceDevice);

        emit sourceDeviceChanged(sourceDevice);
    }
}

/*!
    \internal
 */
QAbstractAxisInput::~QAbstractAxisInput()
{
}

QAbstractPhysicalDevice *QAbstractAxisInput::sourceDevice() const
{
    Q_D(const QAbstractAxisInput);
    return d->m_sourceDevice;
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qabstractaxisinput.cpp"
