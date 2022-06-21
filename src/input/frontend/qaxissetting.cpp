// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaxissetting.h"
#include "qaxissetting_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
/*!
    \class Qt3DInput::QAxisSetting
    \inmodule Qt3DInput
    \inherits Qt3DCore::QNode
    \brief QAxisSetting stores settings for the specified list of Axis.

    Stores the dead zone associated with this axis and defines if smoothing is enabled
    \since 5.5
*/

/*!

    \qmltype AxisSetting
    \inqmlmodule Qt3D.Input
    \instantiates Qt3DInput::QAxisSetting
    \brief QML frontend for the Qt3DInput::QAxisSetting C++ class.

    Stores settings for the specified list of Axis
    \since 5.5
*/

/*!
  \qmlproperty list<int> AxisSetting::axes
 */


/*!
    Constructs a new QAxisSetting instance with \a parent.
 */
QAxisSetting::QAxisSetting(Qt3DCore::QNode *parent)
    : QNode(*new QAxisSettingPrivate(), parent)
{
}

/*! \internal */
QAxisSetting::~QAxisSetting()
{
}

/*!
    \property QAxisSetting::axes
 */

/*!
 * \brief QAxisSetting::axes
 * \return the current list of Axis these settings apply to.
 */
QList<int> QAxisSetting::axes() const
{
    Q_D(const QAxisSetting);
    return d->m_axes;
}

/*!
    \property QAxisSetting::deadZoneRadius
 */

/*!
 * \brief QAxisSetting::deadZoneRadius
 * \return the set dead zone radius.
 */
float QAxisSetting::deadZoneRadius() const
{
    Q_D(const QAxisSetting);
    return d->m_deadZoneRadius;
}

/*!
    \property QAxisSetting::smooth
 */

/*!
 * \brief QAxisSetting::isSmoothEnabled
 * \return if smoothing is enabled.
 */
bool QAxisSetting::isSmoothEnabled() const
{
    Q_D(const QAxisSetting);
    return d->m_smooth;
}


/*!
  \fn Qt3DInput::QAxisSetting::deadZoneRadiusChanged(float deadZoneRadius)

  This signal is emitted when the Dead Zone radius associated with the axis setting is changed to \a deadZoneRadius.
*/

/*!
  \qmlproperty float Qt3D.Input::AxisSetting::deadZoneRadius

  The current deadZone radius of the AxisSetting
*/

/*!
    \qmlsignal Qt3D.Input::AxisSetting::deadZoneRadiusChanged()

    This signal is emitted when the dead zone associated with the axis setting is changed.

    The corresponding handler is \c onDeadZoneRadiusChanged
*/

/*!
    Set the current dead zone radius of the QAxisSetting instance to \a deadZoneRadius.
 */
void QAxisSetting::setDeadZoneRadius(float deadZoneRadius)
{
    Q_D(QAxisSetting);
    if (d->m_deadZoneRadius == deadZoneRadius)
        return;

    d->m_deadZoneRadius = deadZoneRadius;
    emit deadZoneRadiusChanged(deadZoneRadius);
}

/*!
  \fn Qt3DInput::QAxisSetting::axesChanged(const QList<int> &axes)

  This signal is emitted when the axes associated with the axis setting is changed to \a axes.
*/

/*!
  \qmlproperty QVariantList Qt3D.Input::AxisSetting::axis

  The current axis of the AxisSetting
*/

/*!
    \qmlsignal Qt3D.Input::AxisSetting::axisChanged()

    This signal is emitted when the axis associated with the axis setting is changed.

    The corresponding handler is \c onAxisChanged
*/

/*!
    Set the current axes of the QAxisSetting instance to \a axes.
 */
void QAxisSetting::setAxes(const QList<int> &axes)
{
    Q_D(QAxisSetting);
    if (d->m_axes == axes)
        return;

    d->m_axes = axes;
    emit axesChanged(axes);
}

/*!
  \fn Qt3DInput::QAxisSetting::smoothChanged(bool smooth)

  This signal is emitted when the smoothing state is changed to \a smooth.
*/

/*!
  \qmlproperty bool Qt3D.Input::AxisSetting::smooth

  The current state of smoothing
*/

/*!
    \qmlsignal Qt3D.Input::AxisSetting::smoothChanged()

    This signal is emitted when the when the smoothing state is changed.

    The corresponding handler is \c onSmoothChanged
*/

/*!
    Set the current state of the QAxisSettings smoothing to \a enabled.
 */
void QAxisSetting::setSmoothEnabled(bool enabled)
{
    Q_D(QAxisSetting);
    if (d->m_smooth == enabled)
        return;

    d->m_smooth = enabled;
    emit smoothChanged(enabled);
}

} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_qaxissetting.cpp"
