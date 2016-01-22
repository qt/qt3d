/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qtransform.h"
#include "qtransform_p.h"
#include "qmath3d_p.h"

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QTransformPrivate::QTransformPrivate()
    : QComponentPrivate()
    , m_rotation()
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_translation()
    , m_eulerRotationAngles()
    , m_matrixDirty(false)
{
}

QTransform::QTransform(QNode *parent)
    : QComponent(*new QTransformPrivate, parent)
{
}
/*!
    \qmltype Transform
    \inqmlmodule Qt3D.Core
    \since 5.6
    \TODO
*/

/*! \internal */
QTransform::QTransform(QTransformPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

QTransform::~QTransform()
{
    QNode::cleanup();
}

void QTransform::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QTransform *transform = static_cast<const QTransform *>(ref);
    // We need to copy the matrix with all the pending
    // transformations applied
    d_func()->m_matrix = transform->matrix();
    d_func()->m_rotation = transform->rotation();
    d_func()->m_scale = transform->scale3D();
    d_func()->m_translation = transform->translation();
    d_func()->m_eulerRotationAngles = transform->d_func()->m_eulerRotationAngles;
    d_func()->m_matrixDirty = transform->d_func()->m_matrixDirty;
}

void QTransform::setMatrix(const QMatrix4x4 &m)
{
    Q_D(QTransform);
    if (m != matrix()) {
        d->m_matrix = m;
        d->m_matrixDirty = false;

        QVector3D s;
        QVector3D t;
        QQuaternion r;
        decomposeQMatrix4x4(m, t, r, s);
        d->m_scale = s;
        d->m_rotation = r;
        d->m_translation = t;
        d->m_eulerRotationAngles = d->m_rotation.toEulerAngles();
        emit scale3DChanged(s);
        emit rotationChanged(r);
        emit translationChanged(t);

        const bool wasBlocked = blockNotifications(true);
        emit matrixChanged(m);
        emit rotationXChanged(d->m_eulerRotationAngles.x());
        emit rotationYChanged(d->m_eulerRotationAngles.y());
        emit rotationZChanged(d->m_eulerRotationAngles.z());
        blockNotifications(wasBlocked);
    }
}

void QTransform::setRotationX(float rotationX)
{
    Q_D(QTransform);

    if (d->m_eulerRotationAngles.x() == rotationX)
        return;

    d->m_eulerRotationAngles.setX(rotationX);
    QQuaternion rotation = QQuaternion::fromEulerAngles(d->m_eulerRotationAngles);
    if (rotation != d->m_rotation) {
        d->m_rotation = rotation;
        d->m_matrixDirty = true;
        emit rotationChanged(rotation);
    }

    const bool wasBlocked = blockNotifications(true);
    emit rotationXChanged(rotationX);
    blockNotifications(wasBlocked);
}

void QTransform::setRotationY(float rotationY)
{
    Q_D(QTransform);

    if (d->m_eulerRotationAngles.y() == rotationY)
        return;

    d->m_eulerRotationAngles.setY(rotationY);
    QQuaternion rotation = QQuaternion::fromEulerAngles(d->m_eulerRotationAngles);
    if (rotation != d->m_rotation) {
        d->m_rotation = rotation;
        d->m_matrixDirty = true;
        emit rotationChanged(rotation);
    }

    const bool wasBlocked = blockNotifications(true);
    emit rotationYChanged(rotationY);
    blockNotifications(wasBlocked);
}

void QTransform::setRotationZ(float rotationZ)
{
    Q_D(QTransform);
    if (d->m_eulerRotationAngles.z() == rotationZ)
        return;

    d->m_eulerRotationAngles.setZ(rotationZ);
    QQuaternion rotation = QQuaternion::fromEulerAngles(d->m_eulerRotationAngles);
    if (rotation != d->m_rotation) {
        d->m_rotation = rotation;
        d->m_matrixDirty = true;
        emit rotationChanged(rotation);
    }

    const bool wasBlocked = blockNotifications(true);
    emit rotationZChanged(rotationZ);
    blockNotifications(wasBlocked);
}

QMatrix4x4 QTransform::matrix() const
{
    Q_D(const QTransform);
    if (d->m_matrixDirty) {
        composeQMatrix4x4(d->m_translation, d->m_rotation, d->m_scale, d->m_matrix);
        d->m_matrixDirty = false;
    }
    return d->m_matrix;
}

float QTransform::rotationX() const
{
    Q_D(const QTransform);
    return d->m_eulerRotationAngles.x();
}

float QTransform::rotationY() const
{
    Q_D(const QTransform);
    return d->m_eulerRotationAngles.y();
}

float QTransform::rotationZ() const
{
    Q_D(const QTransform);
    return d->m_eulerRotationAngles.z();
}

void QTransform::setScale3D(const QVector3D &scale)
{
    Q_D(QTransform);
    if (scale != d->m_scale) {
        d->m_scale = scale;
        d->m_matrixDirty = true;
        emit scale3DChanged(scale);
    }
}

QVector3D QTransform::scale3D() const
{
    Q_D(const QTransform);
    return d->m_scale;
}

void QTransform::setScale(float scale)
{
    Q_D(QTransform);
    if (scale != d->m_scale.x()) {
        setScale3D(QVector3D(scale, scale, scale));
        const bool wasBlocked = blockNotifications(true);
        emit scaleChanged(scale);
        blockNotifications(wasBlocked);
    }
}

float QTransform::scale() const
{
    Q_D(const QTransform);
    return d->m_scale.x();
}

void QTransform::setRotation(const QQuaternion &rotation)
{
    Q_D(QTransform);
    if (rotation != d->m_rotation) {
        d->m_rotation = rotation;
        d->m_eulerRotationAngles = d->m_rotation.toEulerAngles();
        d->m_matrixDirty = true;
        emit rotationChanged(rotation);
    }
}

QQuaternion QTransform::rotation() const
{
    Q_D(const QTransform);
    return d->m_rotation;
}

void QTransform::setTranslation(const QVector3D &translation)
{
    Q_D(QTransform);
    if (translation != d->m_translation) {
        d->m_translation = translation;
        d->m_matrixDirty = true;
        emit translationChanged(translation);
    }
}

QVector3D QTransform::translation() const
{
    Q_D(const QTransform);
    return d->m_translation;
}

QQuaternion QTransform::fromAxisAndAngle(const QVector3D &axis, float angle)
{
    return QQuaternion::fromAxisAndAngle(axis, angle);
}

QQuaternion QTransform::fromAxisAndAngle(float x, float y, float z, float angle)
{
    return QQuaternion::fromAxisAndAngle(x, y, z, angle);
}

QQuaternion QTransform::fromAxesAndAngles(const QVector3D &axis1, float angle1,
                                          const QVector3D &axis2, float angle2)
{
    const QQuaternion q1 = QQuaternion::fromAxisAndAngle(axis1, angle1);
    const QQuaternion q2 = QQuaternion::fromAxisAndAngle(axis2, angle2);
    return q2 * q1;
}

QQuaternion QTransform::fromAxesAndAngles(const QVector3D &axis1, float angle1,
                                          const QVector3D &axis2, float angle2,
                                          const QVector3D &axis3, float angle3)
{
    const QQuaternion q1 = QQuaternion::fromAxisAndAngle(axis1, angle1);
    const QQuaternion q2 = QQuaternion::fromAxisAndAngle(axis2, angle2);
    const QQuaternion q3 = QQuaternion::fromAxisAndAngle(axis3, angle3);
    return q3 * q2 * q1;
}

QQuaternion QTransform::fromEulerAngles(const QVector3D &eulerAngles)
{
    return QQuaternion::fromEulerAngles(eulerAngles);
}

QQuaternion QTransform::fromEulerAngles(float pitch, float yaw, float roll)
{
    return QQuaternion::fromEulerAngles(pitch, yaw, roll);
}

QMatrix4x4 QTransform::rotateAround(const QVector3D &point, float angle, const QVector3D &axis)
{
    QMatrix4x4 m;
    m.translate(point);
    m.rotate(angle, axis);
    m.translate(-point);
    return m;
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qtransform.cpp"
