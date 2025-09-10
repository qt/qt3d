// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qbuttonaxisinput.h"
#include "qbuttonaxisinput_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
 * \qmltype ButtonAxisInput
 * \nativetype Qt3DInput::QButtonAxisInput
 * \inqmlmodule Qt3D.Input
 * \brief QML frontend for QButtonAxisInput C++ class.
 * \since 5.7
 * The axis value is controlled by buttons rather than a traditional analog input such as a joystick.
 *
 */

/*!
 * \class Qt3DInput::QButtonAxisInput
 * \inheaderfile Qt3DInput/QButtonAxisInput
 * \inmodule Qt3DInput
 * \brief An axis input controlled by buttons
 * \since 5.7
 * The axis value is controlled by buttons rather than a traditional analog input such as a joystick.
 *
 */
/*!
    \qmlproperty real ButtonAxisInput::scale
    Specifies how the axis output value is scaled. No scaling is applied by default
    so it defaults to 1.
*/

/*!
    \qmlproperty list<var> ButtonAxisInput::buttons
    Specifies the buttons which control the axis
*/

/*!
    \qmlproperty real ButtonAxisInput::acceleration
    Specifies the rate at which the axis value changes when a button is held
*/

/*!
    \qmlproperty list<var> ButtonAxisInput::deceleration
    Specifies the rate at which the axis value returns to normal when a button is released
*/

/*!
    Constructs a new QButtonAxisInput instance with \a parent.
 */
QButtonAxisInput::QButtonAxisInput(Qt3DCore::QNode *parent)
    : QAbstractAxisInput(*new QButtonAxisInputPrivate, parent)
{
}

/*! \internal */
QButtonAxisInput::~QButtonAxisInput()
{
}

/*!
    \property Qt3DInput::QButtonAxisInput::scale
    Specifies how the axis output value is scaled. No scaling is applied by default
    so it defaults to 1.
 */
void QButtonAxisInput::setScale(float scale)
{
    Q_D(QButtonAxisInput);
    if (d->m_scale != scale) {
        d->m_scale = scale;
        emit scaleChanged(scale);
    }
}

float QButtonAxisInput::scale() const
{
    Q_D(const QButtonAxisInput);
    return d->m_scale;
}

/*!
    \property Qt3DInput::QButtonAxisInput::buttons
    Specifies the buttons which control the axis
 */
void QButtonAxisInput::setButtons(const QList<int> &buttons)
{
    Q_D(QButtonAxisInput);
    if (buttons != d->m_buttons) {
        d->m_buttons = buttons;
        emit buttonsChanged(buttons);
    }
}

QList<int> QButtonAxisInput::buttons() const
{
    Q_D(const QButtonAxisInput);
    return d->m_buttons;
}

/*!
    \property Qt3DInput::QButtonAxisInput::acceleration
    Specifies the rate at which the axis value changes when a button is held
 */
float QButtonAxisInput::acceleration() const
{
    Q_D(const QButtonAxisInput);
    return d->m_acceleration;
}

void QButtonAxisInput::setAcceleration(float acceleration)
{
    Q_D(QButtonAxisInput);
    if (d->m_acceleration != acceleration) {
        d->m_acceleration = acceleration;
        emit accelerationChanged(acceleration);
    }
}

/*!
    \property Qt3DInput::QButtonAxisInput::deceleration
    Specifies the rate at which the axis value returns to normal when a button is released
 */
float QButtonAxisInput::deceleration() const
{
    Q_D(const QButtonAxisInput);
    return d->m_deceleration;
}

void QButtonAxisInput::setDeceleration(float deceleration)
{
    Q_D(QButtonAxisInput);
    if (d->m_deceleration != deceleration) {
        d->m_deceleration = deceleration;
        emit decelerationChanged(deceleration);
    }
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qbuttonaxisinput.cpp"
