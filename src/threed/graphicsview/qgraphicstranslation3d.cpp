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

#include "qgraphicstranslation3d.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsTranslation3D
    \brief The QGraphicsTranslation3D class supports translation of 3D items.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview

    QGraphicsTranslation3D is derived directly from QGraphicsTransform, and
    provides a \l translate property to specify the 3D vector to
    apply to incoming co-ordinates.

    The \l progress property can be used to perform animation along a
    translation vector by varying the progress value between 0 and 1.
    Overshoot animations are also possible by setting the progress
    value to something outside this range.  The default progress
    value is 1.

    \sa QGraphicsRotation3D, QGraphicsScale3D
*/

/*!
    \qmlclass Translation3D QGraphicsTranslation3D
    \brief The Translation3D item supports translation of items in 3D.
    \since 4.8
    \ingroup qt3d::qml3d

    3D items in QML are typically positioned directly as follows:

    \code
    Item3D {
        mesh: Mesh { source: "chair.3ds" }
        position: Qt.vector3d(0, 5, 10)
    }
    \endcode

    However, it can sometimes be useful to translate an object along a
    vector under the control of an animation.  The Translate3D
    element can be used for this purpose.  The following example
    translates the object along a straight-line path 5 units to
    the right of its original position, and then back again:

    \code
    Item3D {
        mesh: Mesh { source: "chair.3ds" }
        position: Qt.vector3d(0, 5, 10)
        transform: [
            Translation3D {
                translate: Qt.vector3d(5, 0, 0)
                SequentialAnimation on progress {
                    running: true
                    loops: Animation.Infinite
                    NumberAnimation { to : 1.0; duration: 300 }
                    NumberAnimation { to : 0.0; duration: 300 }
                }
            }
        ]
    }
    \endcode

    \sa Rotation3D, Scale3D
*/

class QGraphicsTranslation3DPrivate
{
public:
    QGraphicsTranslation3DPrivate() : progress(1.0f) {}

    QVector3D translate;
    qreal progress;
};

/*!
    Constructs a new translation and attaches it to \a parent.
*/
QGraphicsTranslation3D::QGraphicsTranslation3D(QObject *parent)
    : QGraphicsTransform3D(parent), d_ptr(new QGraphicsTranslation3DPrivate)
{
}

/*!
    Destroys this translation.
*/
QGraphicsTranslation3D::~QGraphicsTranslation3D()
{
}

/*!
    \property QGraphicsTranslation3D::translate
    \brief the translation to apply to incoming co-ordinates.

    The default value for this property is (0, 0, 0).
*/

/*!
    \qmlproperty vector3D Translation3D::translate

    The translation to apply to incoming co-ordinates.  The default value
    for this property is (0, 0, 0).
*/

QVector3D QGraphicsTranslation3D::translate() const
{
    Q_D(const QGraphicsTranslation3D);
    return d->translate;
}

void QGraphicsTranslation3D::setTranslate(const QVector3D &value)
{
    Q_D(QGraphicsTranslation3D);
    if (d->translate != value) {
        d->translate = value;
        emit transformChanged();
        emit translateChanged();
    }
}

/*!
    \property QGraphicsTranslation3D::progress
    \brief the progress along the translation vector, from 0 to 1.

    The default value for this property is 1.

    This property can be used to perform animation along a translation
    vector by varying the progress between 0 and 1.  Overshoot animations
    are also possible by setting the value to something outside this range.
*/

/*!
    \qmlproperty real Translation3D::progress

    The progress along the translation vector, from 0 to 1.  The default
    value for this property is 1.

    This property can be used to perform animation along a translation
    vector by varying the progress between 0 and 1.  Overshoot animations
    are also possible by setting the value to something outside this range.
*/

qreal QGraphicsTranslation3D::progress() const
{
    Q_D(const QGraphicsTranslation3D);
    return d->progress;
}

void QGraphicsTranslation3D::setProgress(qreal value)
{
    Q_D(QGraphicsTranslation3D);
    if (d->progress != value) {
        d->progress = value;
        emit transformChanged();
        emit progressChanged();
    }
}

/*!
    \internal
*/
void QGraphicsTranslation3D::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QGraphicsTranslation3D);
    matrix->translate(d->translate * d->progress);
}

/*!
    \internal
*/
QGraphicsTransform3D *QGraphicsTranslation3D::clone(QObject *parent) const
{
    Q_D(const QGraphicsTranslation3D);
    QGraphicsTranslation3D *copy = new QGraphicsTranslation3D(parent);
    copy->setTranslate(d->translate);
    copy->setProgress(d->progress);
    return copy;
}

/*!
    \fn void QGraphicsTranslation3D::translateChanged()

    Signal that is emitted when translate() changes.
*/

/*!
    \fn void QGraphicsTranslation3D::progressChanged()

    Signal that is emitted when progress() changes.
*/

QT_END_NAMESPACE
