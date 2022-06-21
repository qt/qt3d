// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmousedevice.h"
#include "qmousedevice_p.h"

#include <Qt3DInput/qmouseevent.h>
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*! \internal */
QMouseDevicePrivate::QMouseDevicePrivate()
    : QAbstractPhysicalDevicePrivate()
    , m_sensitivity(0.1f)
    , m_updateContinuously(false)
{
}

/*!
    \qmltype MouseDevice
    \instantiates Qt3DInput::QMouseDevice
    \inqmlmodule Qt3D.Input
    \since 5.5
    \brief Delegates mouse events to the attached MouseHandler objects.

    A MouseDevice delegates mouse events from physical mouse device to
    MouseHandler objects. The sensitivity of the mouse can be controlled
    with the \l MouseDevice::sensitivity property, which specifies the rate
    in which the logical mouse coordinates change in response to physical
    movement of the mouse.

    \sa MouseHandler
 */

/*!
    \class Qt3DInput::QMouseDevice
    \inmodule Qt3DInput
    \since 5.5
    \brief Delegates mouse events to the attached MouseHandler objects.

    A QMouseDevice delegates mouse events from physical mouse device to
    QMouseHandler objects.  The sensitivity of the mouse can be controlled
    with the \l QMouseDevice::sensitivity property, which specifies the rate
    in which the logical mouse coordinates change in response to physical
    movement of the mouse.

    \sa QMouseHandler
 */

/*!
    \enum QMouseDevice::Axis

    The mouse axis.

    \value X
    \value Y
    \value WheelX
    \value WheelY

    \sa Qt3DInput::QAnalogAxisInput::setAxis
 */

/*!
    \qmlproperty real MouseDevice::sensitivity

    Holds the current sensitivity of the mouse device.
    Default is 0.1.
 */

/*!
    \property Qt3DInput::QMouseDevice::updateAxesContinuously

    If \c true, axes will be updated anytime they change regardless of whether
    any mouse button is being pressed. Otherwise, axes are updated only when
    one of the mouse buttons is being pressed.

    The default value is \c false.
    \since 5.15
*/

/*!
    \qmlproperty bool MouseDevice::updateAxesContinuously

    If \c true, axes will be updated anytime they change regardless of whether
    any mouse button is being pressed. Otherwise, axes are updated only when
    one of the mouse buttons is being pressed.

    The default value is \c false.
    \since 5.15
*/

/*!
    Constructs a new QMouseDevice instance with parent \a parent.
 */
QMouseDevice::QMouseDevice(QNode *parent)
    : QAbstractPhysicalDevice(*new QMouseDevicePrivate, parent)
{
}

/*! \internal */
QMouseDevice::~QMouseDevice()
{
}

/*!
    \return the axis count.

    \note Currently always returns 4.
 */
int QMouseDevice::axisCount() const
{
    return 4;
}

/*!
    \return the button count.

    \note Currently always returns 3.
 */
int QMouseDevice::buttonCount() const
{
    return 3;
}

/*!
    \return the names of the axis.

    \note Currently always returns StringList["X", "Y"]
 */
QStringList QMouseDevice::axisNames() const
{
    return QStringList()
            << QStringLiteral("X")
            << QStringLiteral("Y")
            << QStringLiteral("WheelX")
            << QStringLiteral("WheelY");
}

/*!
    \return the names of the buttons.

    \note Currently always returns StringList["Left", "Right", "Center"]
 */
QStringList QMouseDevice::buttonNames() const
{
    return QStringList()
            << QStringLiteral("Left")
            << QStringLiteral("Right")
            << QStringLiteral("Center");
}

/*!
    Convert axis \a name to axis identifier.
 */
int QMouseDevice::axisIdentifier(const QString &name) const
{
    if (name == QLatin1String("X"))
        return X;
    else if (name == QLatin1String("Y"))
        return Y;
    else if (name == QLatin1String("WheelX"))
        return WheelX;
    else if (name == QLatin1String("WheelY"))
        return WheelY;
    return -1;
}

/*!
    Returns the button identifier that corresponds with
    the specified \a name. The possible return values are
    documented in \l {Qt3DInput::}{QMouseEvent::Buttons}.
*/
int QMouseDevice::buttonIdentifier(const QString &name) const
{
    if (name == QLatin1String("Left"))
        return Qt3DInput::QMouseEvent::LeftButton;
    if (name == QLatin1String("Right"))
        return Qt3DInput::QMouseEvent::RightButton;
    if (name == QLatin1String("Center"))
        return Qt3DInput::QMouseEvent::MiddleButton;
    return -1;
}

/*!
  \property Qt3DInput::QMouseDevice::sensitivity

  Holds the sensitivity of the mouse device.
  The default is 0.1.
*/
float QMouseDevice::sensitivity() const
{
    Q_D(const QMouseDevice);
    return d->m_sensitivity;
}

bool QMouseDevice::updateAxesContinuously() const
{
    Q_D(const QMouseDevice);
    return d->m_updateContinuously;
}

void QMouseDevice::setSensitivity(float value)
{
    Q_D(QMouseDevice);
    if (qFuzzyCompare(value, d->m_sensitivity))
        return;

    d->m_sensitivity = value;
    emit sensitivityChanged(value);
}

void QMouseDevice::setUpdateAxesContinuously(bool updateAxesContinuously)
{
    Q_D(QMouseDevice);
    if (d->m_updateContinuously == updateAxesContinuously)
        return;

    d->m_updateContinuously = updateAxesContinuously;
    emit updateAxesContinuouslyChanged(updateAxesContinuously);
}

} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_qmousedevice.cpp"
