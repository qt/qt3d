/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgraphicsrotation3d.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsRotation3D
    \brief The QGraphicsRotation3D class supports arbitrary rotation around an axis in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview

    \sa QGraphicsTranslation3D, QGraphicsScale3D
*/

/*!
    \qmlclass Rotation3D QGraphicsRotation3D
    \brief The Rotation3D item supports arbitrary rotation around an axis in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d

    Frequently a user will create and item in the 3d world and immediately
    wish to apply a rotation to that item before it is displayed, or,
    optionally, perform an animation on that rotation parameter based on
    user inputs, or other events.  Such an rotation can easily be defined
    in QML using the following code:

    \code
    Item3D {
        id: helicoptor
        mesh:  {source: "bellUH1.3ds"}
        effect: Effect {}
        cullFaces: "CullBackFaces"

        transform: [
                        Rotation3D {
                                id: rotate1
                                angle: 5
                                axis: Qt.vector3d(1, 0, 0)
                        },
                        Rotation3D {
                                id: rotate2
                                angle: 5
                                axis: Qt.vector3d(0, 1, 0)
                        },
                        Rotation3D {
                                id: rotate3
                                angle: 45
                                axis: Qt.vector3d(0, 0, 1)
                        }
                ]

       SequentialAnimation {
            NumberAnimation {target: rotate1; property: "angle"; to : 360.0; duration: 3000; easing:"easeOutQuad" }
        }
    }
    \endcode

    Notice here that we create a list of rotations for the \c transform
    property of the container item.  By doing this we allow rotations
    around each of the axes individually in a manner which is conducive
    to animation and interaction.

    Each of the rotations has an \c axis property which is a QVector3D.
    This vector contains a value for each of the three components
    corresponding to x, y, and z.  In the above example, we first
    rotate by 5 degrees about the x axis, then 5 degrees about the y
    axis, and finally by 45 degrees about the z axis.

    By giving each rotation a unique \c id users can then refer to these
    rotations in the QML source in order to perform rotational animations.

    \sa Translation3D, Scale3D
*/

class QGraphicsRotation3DPrivate
{
public:
    QGraphicsRotation3DPrivate() : axis(0, 0, 1), angle(0) {}

    QVector3D origin;
    QVector3D axis;
    qreal angle;
};

/*!
    Create a 3D rotation transformation and attach it to \a parent.
*/
QGraphicsRotation3D::QGraphicsRotation3D(QObject *parent)
    : QGraphicsTransform3D(parent)
    , d_ptr(new QGraphicsRotation3DPrivate)
{
}

/*!
    Destroy this 3D rotation transformation.
*/
QGraphicsRotation3D::~QGraphicsRotation3D()
{
}

/*!
    \property QGraphicsRotation3D::origin
    \brief the origin about which to rotate.

    The default value for this property is (0, 0, 0).
*/

/*!
    \qmlproperty vector3D Rotation3D::origin

    The origin about which to rotate.  The default value for this
    property is (0, 0, 0).
*/

QVector3D QGraphicsRotation3D::origin() const
{
    Q_D(const QGraphicsRotation3D);
    return d->origin;
}

void QGraphicsRotation3D::setOrigin(const QVector3D &value)
{
    Q_D(QGraphicsRotation3D);
    if (d->origin != value) {
        d->origin = value;
        emit transformChanged();
        emit originChanged();
    }
}

/*!
    \property QGraphicsRotation3D::angle
    \brief the angle to rotate around the axis, in degrees anti-clockwise.

    The default value for this property is 0.
*/

/*!
    \qmlproperty real Rotation3D::angle

    The angle to rotate around the axis, in degrees anti-clockwise.
    The default value for this property is 0.
*/

qreal QGraphicsRotation3D::angle() const
{
    Q_D(const QGraphicsRotation3D);
    return d->angle;
}

void QGraphicsRotation3D::setAngle(qreal value)
{
    Q_D(QGraphicsRotation3D);
    if (d->angle != value) {
        d->angle = value;
        emit transformChanged();
        emit angleChanged();
    }
}

/*!
    \property QGraphicsRotation3D::axis
    \brief the axis to rotate around.

    The default value for this property is (0, 0, 1); i.e. the z-axis.
*/

/*!
    \qmlproperty vector3D Rotation3D::axis

    The axis to rotate around.  The default value for this property
    is (0, 0, 1); i.e. the z-axis.
*/

QVector3D QGraphicsRotation3D::axis() const
{
    Q_D(const QGraphicsRotation3D);
    return d->axis;
}

void QGraphicsRotation3D::setAxis(const QVector3D &value)
{
    Q_D(QGraphicsRotation3D);
    if (d->axis != value) {
        d->axis = value;
        emit transformChanged();
        emit axisChanged();
    }
}

/*!
    \internal
*/
void QGraphicsRotation3D::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QGraphicsRotation3D);
    matrix->translate(d->origin);
    matrix->rotate(d->angle, d->axis.x(), d->axis.y(), d->axis.z());
    matrix->translate(-d->origin);
}

/*!
    \internal
*/
QGraphicsTransform3D *QGraphicsRotation3D::clone(QObject *parent) const
{
    Q_D(const QGraphicsRotation3D);
    QGraphicsRotation3D *copy = new QGraphicsRotation3D(parent);
    copy->setOrigin(d->origin);
    copy->setAxis(d->axis);
    copy->setAngle(d->angle);
    return copy;
}

/*!
    \fn void QGraphicsRotation3D::originChanged()

    Signal that is emitted when origin() changes.
*/

/*!
    \fn void QGraphicsRotation3D::angleChanged()

    Signal that is emitted when angle() changes.
*/

/*!
    \fn void QGraphicsRotation3D::axisChanged()

    Signal that is emitted when axis() changes.
*/

QT_END_NAMESPACE
