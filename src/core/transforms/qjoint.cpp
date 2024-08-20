// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qjoint.h"
#include "qjoint_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QJointPrivate::QJointPrivate()
    : QNodePrivate()
    , m_inverseBindMatrix()
    , m_rotation()
    , m_translation()
    , m_scale(1.0f, 1.0f, 1.0f)
{
}

/*!
    \qmltype Joint
    \inqmlmodule Qt3D.Core
    \inherits Node
    \nativetype Qt3DCore::QJoint
    \since 5.10
    \brief Used to transforms parts of skinned meshes.

    The Joint node is used to build skeletons as part of the skinned mesh
    support in Qt 3D. A joint can be transformed by way of its scale, rotation
    and translation properties. Any mesh vertices that are bound to the joint
    will have their transformations updated accordingly.
*/

/*!
    \qmlproperty vector3d Joint::scale

    Holds the uniform scale of the joint.
*/

/*!
    \qmlproperty quaternion Joint::rotation

    Holds the rotation of the joint as quaternion.
*/

/*!
    \qmlproperty vector3d Joint::translation

    Holds the translation of the joint as vector3d.
*/

/*!
    \qmlproperty real Joint::rotationX

    Holds the x rotation of the joint as an Euler angle.
*/

/*!
    \qmlproperty real Joint::rotationY

    Holds the y rotation of the joint as an Euler angle.
*/

/*!
    \qmlproperty real Joint::rotationZ

    Holds the z rotation of the joint as an Euler angle.
*/

/*!
    \qmlproperty matrix4x4 Joint::inverseBindMatrix

    Holds the inverse bind matrix of the joint. This is used to transform
    vertices from model space into the space of this joint so they can
    subsequently be multiplied by the joint's global transform to perform
    the skinning operation.
*/

/*!
    \class Qt3DCore::QJoint
    \inmodule Qt3DCore
    \inherits Qt3DCore::QNode
    \since 5.10
    \brief Used to transforms parts of skinned meshes.

    The QJoint node is used to build skeletons as part of the skinned mesh
    support in Qt 3D. A joint can be transformed by way of its scale, rotation
    and translation properties. Any mesh vertices that are bound to the joint
    will have their transformations updated accordingly.
*/

/*!
    Constructs a new QJoint with \a parent.
*/
QJoint::QJoint(Qt3DCore::QNode *parent)
    : QNode(*new QJointPrivate, parent)
{
}

/*! \internal */
QJoint::~QJoint()
{
}

/*!
    \property Qt3DCore::QJoint::scale

    Holds the scale of the joint.
*/
QVector3D QJoint::scale() const
{
    Q_D(const QJoint);
    return d->m_scale;
}

/*!
    \property Qt3DCore::QJoint::rotation

    Holds the rotation of the joint as QQuaternion.
*/
QQuaternion QJoint::rotation() const
{
    Q_D(const QJoint);
    return d->m_rotation;
}

/*!
    \property Qt3DCore::QJoint::translation

    Holds the translation of the joint as QVector3D.
*/
QVector3D QJoint::translation() const
{
    Q_D(const QJoint);
    return d->m_translation;
}

/*!
    \property Qt3DCore::QJoint::inverseBindMatrix

    Holds the inverse bind matrix of the joint. This is used to transform
    vertices from model space into the space of this joint so they can
    subsequently be multiplied by the joint's global transform to perform
    the skinning operation.
*/
QMatrix4x4 QJoint::inverseBindMatrix() const
{
    Q_D(const QJoint);
    return d->m_inverseBindMatrix;
}

/*!
    \property Qt3DCore::QJoint::rotationX

    Holds the x rotation of the joint as an Euler angle.
*/
float QJoint::rotationX() const
{
    Q_D(const QJoint);
    return d->m_eulerRotationAngles.x();
}

/*!
    \property Qt3DCore::QJoint::rotationY

    Holds the y rotation of the joint as an Euler angle.
*/
float QJoint::rotationY() const
{
    Q_D(const QJoint);
    return d->m_eulerRotationAngles.y();
}

/*!
    \property Qt3DCore::QJoint::rotationZ

    Holds the z rotation of the joint as an Euler angle.
*/
float QJoint::rotationZ() const
{
    Q_D(const QJoint);
    return d->m_eulerRotationAngles.z();
}

void QJoint::setScale(const QVector3D &scale)
{
    Q_D(QJoint);
    if (scale == d->m_scale)
        return;

    d->m_scale = scale;
    emit scaleChanged(scale);
}

void QJoint::setRotation(const QQuaternion &rotation)
{
    Q_D(QJoint);
    if (rotation == d->m_rotation)
        return;

    d->m_rotation = rotation;
    const QVector3D oldRotation = d->m_eulerRotationAngles;
    d->m_eulerRotationAngles = d->m_rotation.toEulerAngles();
    emit rotationChanged(rotation);

    const bool wasBlocked = blockNotifications(true);
    if (!qFuzzyCompare(d->m_eulerRotationAngles.x(), oldRotation.x()))
        emit rotationXChanged(d->m_eulerRotationAngles.x());
    if (!qFuzzyCompare(d->m_eulerRotationAngles.y(), oldRotation.y()))
        emit rotationYChanged(d->m_eulerRotationAngles.y());
    if (!qFuzzyCompare(d->m_eulerRotationAngles.z(), oldRotation.z()))
        emit rotationZChanged(d->m_eulerRotationAngles.z());
    blockNotifications(wasBlocked);
}

void QJoint::setTranslation(const QVector3D &translation)
{
    Q_D(QJoint);
    if (translation == d->m_translation)
        return;

    d->m_translation = translation;
    emit translationChanged(translation);
}

void QJoint::setInverseBindMatrix(const QMatrix4x4 &inverseBindMatrix)
{
    Q_D(QJoint);
    if (d->m_inverseBindMatrix == inverseBindMatrix)
        return;

    d->m_inverseBindMatrix = inverseBindMatrix;
    emit inverseBindMatrixChanged(inverseBindMatrix);
}

void QJoint::setRotationX(float rotationX)
{
    Q_D(QJoint);

    if (qFuzzyCompare(d->m_eulerRotationAngles.x(), rotationX))
        return;

    const auto eulers = QVector3D(rotationX,
                                  d->m_eulerRotationAngles.y(),
                                  d->m_eulerRotationAngles.z());
    const QQuaternion r = QQuaternion::fromEulerAngles(eulers);
    setRotation(r);
}

void QJoint::setRotationY(float rotationY)
{
    Q_D(QJoint);

    if (qFuzzyCompare(d->m_eulerRotationAngles.y(), rotationY))
        return;

    const auto eulers = QVector3D(d->m_eulerRotationAngles.x(),
                                  rotationY,
                                  d->m_eulerRotationAngles.z());
    const QQuaternion r = QQuaternion::fromEulerAngles(eulers);
    setRotation(r);
}

void QJoint::setRotationZ(float rotationZ)
{
    Q_D(QJoint);
    if (qFuzzyCompare(d->m_eulerRotationAngles.z(), rotationZ))
        return;

    const auto eulers = QVector3D(d->m_eulerRotationAngles.x(),
                                  d->m_eulerRotationAngles.y(),
                                  rotationZ);
    const QQuaternion r = QQuaternion::fromEulerAngles(eulers);
    setRotation(r);
}

void QJoint::setName(const QString &name)
{
    Q_D(QJoint);
    if (d->m_name == name)
        return;

    d->m_name = name;
    emit nameChanged(name);
}

/*!
    Sets the transform matrix for this joint to the identity matrix.
*/
void QJoint::setToIdentity()
{
    setScale(QVector3D(1.0f, 1.0f, 1.0f));
    setRotation(QQuaternion());
    setTranslation(QVector3D());
}

/*!
    Adds \a joint as a child of this joint. If \a joint has no parent, then
    this joint takes ownership of it. Child joints are in the coordinate system
    of their parent joint.
*/
void QJoint::addChildJoint(QJoint *joint)
{
    Q_D(QJoint);
    if (!d->m_childJoints.contains(joint)) {
        d->m_childJoints.push_back(joint);
        // Force creation in backend by setting parent
        if (!joint->parent())
            joint->setParent(this);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(joint, &QJoint::removeChildJoint, d->m_childJoints);

        if (d->m_changeArbiter != nullptr)
            d->update();
    }
}

/*!
    Removes \a joint from this joint's list of children. The child joint is not
    destroyed.
*/
void QJoint::removeChildJoint(QJoint *joint)
{
    Q_D(QJoint);
    if (d->m_childJoints.contains(joint)) {
        if (d->m_changeArbiter != nullptr)
            d->update();

        d->m_childJoints.removeOne(joint);

        // Remove bookkeeping connection
        d->unregisterDestructionHelper(joint);
    }
}

/*!
    The vector of joints this joint has as children.
*/
QList<QJoint *> QJoint::childJoints() const
{
    Q_D(const QJoint);
    return d->m_childJoints;
}

/*!
    Returns the name of the joint.
*/
QString QJoint::name() const
{
    Q_D(const QJoint);
    return d->m_name;
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qjoint.cpp"
