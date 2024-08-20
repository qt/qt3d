// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaxisaccumulator.h"
#include "qaxisaccumulator_p.h"

#include <Qt3DInput/qaxis.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    Constructs a new QAxisAccumulator instance with \a parent.
    \class Qt3DInput::QAxisAccumulator
    \inmodule Qt3DInput
    \inherits Qt3DCore::QComponent
    \brief QAxisAccumulator processes velocity or acceleration data from a QAxis.
    \since 5.8

    A Qt3DInput::QAxis reports the current position of an axis on an input
    device. When the axis is returned to its neutral position the value of that
    axis returns to 0. Often, it is required to have the input from an axis
    control a variable in other ways, for example treating the value from the
    Qt3DInput::QAxis as a velocity (first derivative with respect to time) or
    as an acceleration (second derivative with respect to time). This can be
    done with user code or with a Qt3DLogic::QFrameAction but those approached
    are not ideal as they add more work to the main thread and are inherently
    imperative. The Qt3DInput::QAxisAccumulator class allows for this common
    task to be performed on the Qt 3D backend and be specified in a declarative
    manner.
*/

/*!
    \qmltype AxisAccumulator
    \inqmlmodule Qt3D.Input
    \nativetype Qt3DInput::QAxisAccumulator
    \brief QML frontend for the Qt3DInput::QAxisAccumulator C++ class.
    \since 5.8

    An Axis reports the current position of an axis on an input device. When the
    axis is returned to its neutral position the value of that axis returns to 0.
    Often, it is required to have the input from an axis control a variable in
    other ways, for example treating the value from the Axis as a velocity (first
    derivative with respect to time) or as an acceleration (second derivative with
    respect to time). This can be done with user code or with a FrameAction but
    those approached are not ideal as they add more work to the main thread and
    are inherently imperative. The AxisAccumulator class allows for this common
    task to be performed on the Qt 3D backend and be specified in a declarative
    manner.
*/

/*!
    \qmlproperty int Qt3D.Input::Axis::value
    \readonly

    Holds the value accumulated from the sourceAxis.
*/

/*!
 * \enum Qt3DInput::QAxisAccumulator::SourceAxisType
 *
 * \value Velocity
 * \value Acceleration
 */

/*! \internal */
QAxisAccumulatorPrivate::QAxisAccumulatorPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_sourceAxis(nullptr)
    , m_sourceAxisType(QAxisAccumulator::Velocity)
    , m_scale(1.0f)
    , m_value(0.0f)
    , m_velocity(0.0f)
{
}

/*! \internal */
void QAxisAccumulatorPrivate::setValue(float value)
{
    if (value != m_value) {
        Q_Q(QAxisAccumulator);
        m_value = value;
        const bool wasBlocked = q->blockNotifications(true);
        emit q->valueChanged(m_value);
        q->blockNotifications(wasBlocked);
    }
}

/*! \internal */
void QAxisAccumulatorPrivate::setVelocity(float velocity)
{
    if (velocity != m_velocity) {
        Q_Q(QAxisAccumulator);
        m_velocity = velocity;
        const bool wasBlocked = q->blockNotifications(true);
        emit q->velocityChanged(m_velocity);
        q->blockNotifications(wasBlocked);
    }
}

/*!
    Constructs a new QAxisAccumulator instance with parent \a parent.
 */
QAxisAccumulator::QAxisAccumulator(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QAxisAccumulatorPrivate, parent)
{
}

/*! \internal */
QAxisAccumulator::~QAxisAccumulator()
{
}

/*!
    \qmlproperty Axis Qt3D.Input::AxisAccumulator::sourceAxis

    The Axis for which the accumulator should integrate axis values.
 */

/*!
   \return QAxis for which the accumulator should integrate axis values.
 */
QAxis *QAxisAccumulator::sourceAxis() const
{
    Q_D(const QAxisAccumulator);
    return d->m_sourceAxis;
}

/*!
    \qmlproperty SourceAxisType Qt3D.Input::AxisAccumulator::sourceAxisType

    The sourceAxisType property specifies how the accumulator treats the values
    from the source axis.
 */

/*!
   \return how the accumulator treats the value of the sourceAxis.
 */
QAxisAccumulator::SourceAxisType QAxisAccumulator::sourceAxisType() const
{
    Q_D(const QAxisAccumulator);
    return d->m_sourceAxisType;
}

/*!
    \qmlproperty real Qt3D.Input::AxisAccumulator::value

    The amount to scale the axis value by when accumulating. This can be
    thought of as the maximum velocity or acceleration the axis can
    control.
 */

/*!
   \property Qt3DInput::QAxisAccumulator::value
   Returns the accumulated (integrated) value.
 */
float QAxisAccumulator::value() const
{
    Q_D(const QAxisAccumulator);
    return d->m_value;
}

/*!
    \qmlproperty real Qt3D.Input::Axis::velocity
    \readonly

    Returns the velocity. If the sourceAxisType is set to Velocity this is
    simply the value of the source axis multiplied by the scale. If the
    sourceAxisType is set to Acceleration, the velocity is integrated using the
    source axis' value as an acceleration.
*/

/*!
   Returns the velocity. If the sourceAxisType is set to Velocity this is
   simply the value of the source axis multiplied by the scale. If the
   sourceAxisType is set to Acceleration, the velocity is integrated using
   the source axis' value as an acceleration.
 */
float QAxisAccumulator::velocity() const
{
    Q_D(const QAxisAccumulator);
    return d->m_velocity;
}

/*!
    \qmlproperty real Qt3D.Input::Axis::scale

    The amount to scale the axis value by when accumulating. This can be
    thought of as the maximum velocity or acceleration the axis can
    control.
*/

/*!
    The amount to scale the axis value by when accumulating. This can be
    thought of as the maximum velocity or acceleration the axis can
    control.

   \return the amount the input axis values are scaled by.
 */
float QAxisAccumulator::scale() const
{
    Q_D(const QAxisAccumulator);
    return d->m_scale;
}

/*!
    Sets the source axis from which the accumulator should receive values from to
    \a sourceAxis. How these values are treated is controlled by the sourceAxisType
    and scale properties.
 */
void QAxisAccumulator::setSourceAxis(QAxis *sourceAxis)
{
    Q_D(QAxisAccumulator);
    if (d->m_sourceAxis == sourceAxis)
        return;

    if (d->m_sourceAxis)
        d->unregisterDestructionHelper(d->m_sourceAxis);

    if (sourceAxis && !sourceAxis->parent())
        sourceAxis->setParent(this);
    d->m_sourceAxis = sourceAxis;

    // Ensures proper bookkeeping
    if (d->m_sourceAxis)
        d->registerDestructionHelper(d->m_sourceAxis, &QAxisAccumulator::setSourceAxis, d->m_sourceAxis);

    emit sourceAxisChanged(sourceAxis);
}

/*!
    Sets how the accumulator treats the values originating from the \a sourceAxisType.
 */
void QAxisAccumulator::setSourceAxisType(QAxisAccumulator::SourceAxisType sourceAxisType)
{
    Q_D(QAxisAccumulator);
    if (d->m_sourceAxisType == sourceAxisType)
        return;

    d->m_sourceAxisType = sourceAxisType;
    emit sourceAxisTypeChanged(sourceAxisType);
}

void QAxisAccumulator::setScale(float scale)
{
    Q_D(QAxisAccumulator);
    if (d->m_scale == scale)
        return;

    d->m_scale = scale;
    emit scaleChanged(scale);
}

} // namespace Qt3DInput

QT_END_NAMESPACE

#include "moc_qaxisaccumulator.cpp"
