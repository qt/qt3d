// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qquaternionanimation_p.h"

#include <QtQuick/private/qquickanimation_p_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype QuaternionAnimation
    \inherits PropertyAnimation
    \inqmlmodule Qt3D.Core
    \since 5.6

    \brief A PropertyAnimation for quaternions.

    A specialized \l{PropertyAnimation} that defines an animation between two
    \l{QQuaternion}{quaternions}.

    By default spherical linear interpolation is used. This can be changed to
    the faster but less accurate normalized linear interpolation by setting the
    \a type property.

    Instead of specifying quaternions directly in the \a from and \a to
    properties, it is also possible to provide euler angles in degrees in the
    \a fromXRotation, \a toXRotation, \a fromYRotation, \a toYRotation,
    \a fromZRotation, \a toZRotation properties.

    \note Avoid mixing the quaternion and euler angle-based properties. The
    from and to values are expected to be fully specified either via a
    quaternion or the three euler angles.

    \sa {Animation and Transitions in Qt Quick} QQuaternion QQuaternion::slerp() QQuaternion::nlerp()
*/

namespace Qt3DCore {
namespace Quick {

class QQuaternionAnimationPrivate : public QQuickPropertyAnimationPrivate
{
    Q_DECLARE_PUBLIC(QQuaternionAnimation)

public:
    QQuaternionAnimationPrivate() :
        type(QQuaternionAnimation::Slerp)
    { }

    QQuaternionAnimation::Type type;
    QVector3D anglesFrom;
    QVector3D anglesTo;
};

QVariant q_quaternionInterpolator(const QQuaternion &from, const QQuaternion &to, qreal progress)
{
    return QVariant::fromValue(QQuaternion::slerp(from, to, progress));
}

QVariant q_quaternionNlerpInterpolator(const QQuaternion &from, const QQuaternion &to, qreal progress)
{
    return QVariant::fromValue(QQuaternion::nlerp(from, to, progress));
}

QQuaternionAnimation::QQuaternionAnimation(QObject *parent)
    : QQuickPropertyAnimation(*(new QQuaternionAnimationPrivate), parent)
{
    Q_D(QQuaternionAnimation);
    d->interpolatorType = qMetaTypeId<QQuaternion>();
    d->defaultToInterpolatorType = true;
    d->interpolator = QVariantAnimationPrivate::getInterpolator(d->interpolatorType);
}

QQuaternion QQuaternionAnimation::from() const
{
    Q_D(const QQuaternionAnimation);
    return d->from.value<QQuaternion>();
}

void QQuaternionAnimation::setFrom(const QQuaternion &f)
{
    QQuickPropertyAnimation::setFrom(QVariant::fromValue(f));
}

QQuaternion QQuaternionAnimation::to() const
{
    Q_D(const QQuaternionAnimation);
    return d->to.value<QQuaternion>();
}

void QQuaternionAnimation::setTo(const QQuaternion &t)
{
    QQuickPropertyAnimation::setTo(QVariant::fromValue(t));
}

QQuaternionAnimation::Type QQuaternionAnimation::type() const
{
    Q_D(const QQuaternionAnimation);
    return d->type;
}

void QQuaternionAnimation::setType(Type type)
{
    Q_D(QQuaternionAnimation);
    if (d->type == type)
        return;

    d->type = type;
    switch (type) {
    case Nlerp:
QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wcast-function-type")
        d->interpolator = reinterpret_cast<QVariantAnimation::Interpolator>(&q_quaternionNlerpInterpolator);
QT_WARNING_POP
        break;
    case Slerp:
    default:
        d->interpolator = QVariantAnimationPrivate::getInterpolator(d->interpolatorType);
        break;
    }

    emit typeChanged(type);
}

float QQuaternionAnimation::fromXRotation() const
{
    Q_D(const QQuaternionAnimation);
    return d->anglesFrom.x();
}

void QQuaternionAnimation::setFromXRotation(float f)
{
    Q_D(QQuaternionAnimation);
    if (d->anglesFrom.x() == f)
        return;
    d->anglesFrom.setX(f);
    setFrom(QQuaternion::fromEulerAngles(d->anglesFrom));
    emit fromXRotationChanged(f);
}

float QQuaternionAnimation::fromYRotation() const
{
    Q_D(const QQuaternionAnimation);
    return d->anglesFrom.y();
}

void QQuaternionAnimation::setFromYRotation(float f)
{
    Q_D(QQuaternionAnimation);
    if (d->anglesFrom.y() == f)
        return;
    d->anglesFrom.setY(f);
    setFrom(QQuaternion::fromEulerAngles(d->anglesFrom));
    emit fromYRotationChanged(f);
}

float QQuaternionAnimation::fromZRotation() const
{
    Q_D(const QQuaternionAnimation);
    return d->anglesFrom.z();
}

void QQuaternionAnimation::setFromZRotation(float f)
{
    Q_D(QQuaternionAnimation);
    if (d->anglesFrom.z() == f)
        return;
    d->anglesFrom.setZ(f);
    setFrom(QQuaternion::fromEulerAngles(d->anglesFrom));
    emit fromZRotationChanged(f);
}

float QQuaternionAnimation::toXRotation() const
{
    Q_D(const QQuaternionAnimation);
    return d->anglesTo.x();
}

void QQuaternionAnimation::setToXRotation(float f)
{
    Q_D(QQuaternionAnimation);
    if (d->anglesTo.x() == f)
        return;
    d->anglesTo.setX(f);
    setTo(QQuaternion::fromEulerAngles(d->anglesTo));
    emit toXRotationChanged(f);
}

float QQuaternionAnimation::toYRotation() const
{
    Q_D(const QQuaternionAnimation);
    return d->anglesTo.y();
}

void QQuaternionAnimation::setToYRotation(float f)
{
    Q_D(QQuaternionAnimation);
    if (d->anglesTo.y() == f)
        return;
    d->anglesTo.setY(f);
    setTo(QQuaternion::fromEulerAngles(d->anglesTo));
    emit toYRotationChanged(f);
}

float QQuaternionAnimation::toZRotation() const
{
    Q_D(const QQuaternionAnimation);
    return d->anglesTo.z();
}

void QQuaternionAnimation::setToZRotation(float f)
{
    Q_D(QQuaternionAnimation);
    if (d->anglesTo.z() == f)
        return;
    d->anglesTo.setZ(f);
    setTo(QQuaternion::fromEulerAngles(d->anglesTo));
    emit toZRotationChanged(f);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qquaternionanimation_p.cpp"
