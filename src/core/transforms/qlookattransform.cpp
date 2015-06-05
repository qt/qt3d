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

#include "qlookattransform.h"
#include "qlookattransform_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QLookAtTransformPrivate
    \internal
*/
QLookAtTransformPrivate::QLookAtTransformPrivate()
    : QAbstractTransformPrivate()
    , m_matrixDirty(true)
{
}


QLookAtTransform::QLookAtTransform(QObject *parent)
    : QAbstractTransform(*new QLookAtTransformPrivate, parent)
{
}

/*! \internal */
QLookAtTransform::QLookAtTransform(QLookAtTransformPrivate &dd, QObject *parent)
    : QAbstractTransform(dd, parent)
{
}

QMatrix4x4 QLookAtTransform::transformMatrix() const
{
    Q_D(const QLookAtTransform);
    if (d->m_matrixDirty) {
        d->m_matrix.setToIdentity();
        d->m_matrix.lookAt(d->m_position, d->m_viewCenter, d->m_upVector);
        d->m_matrixDirty = false;
    }
    return d->m_matrix;
}

QVector3D QLookAtTransform::position() const
{
    Q_D(const QLookAtTransform);
    return d->m_position;
}

void QLookAtTransform::setPosition(const QVector3D &position)
{
    Q_D(QLookAtTransform);
    if (d->m_position != position) {
        d->m_position = position;
        d->m_viewVector = d->m_viewCenter - position;
        d->m_matrixDirty = true;
        emit positionChanged();
        emit viewVectorChanged();
        emit transformMatrixChanged();
    }
}

void QLookAtTransform::setUpVector(const QVector3D &upVector)
{
    Q_D(QLookAtTransform);
    if (d->m_upVector != upVector) {
        d->m_upVector = upVector;
        d->m_matrixDirty = true;
        emit upVectorChanged();
        emit transformMatrixChanged();
    }
}

QVector3D QLookAtTransform::upVector() const
{
    Q_D(const QLookAtTransform);
    return d->m_upVector;
}

void QLookAtTransform::setViewCenter(const QVector3D &viewCenter)
{
    Q_D(QLookAtTransform);
    if (d->m_viewCenter != viewCenter) {
        d->m_viewCenter = viewCenter;
        d->m_viewVector = viewCenter - d->m_position;
        d->m_matrixDirty = true;
        emit viewCenterChanged();
        emit viewVectorChanged();
        emit transformMatrixChanged();
    }
}

QVector3D QLookAtTransform::viewCenter() const
{
    Q_D(const QLookAtTransform);
    return d->m_viewCenter;
}

void QLookAtTransform::setViewVector(const QVector3D &viewVector)
{
    Q_D(QLookAtTransform);
    if (!qFuzzyCompare(d->m_viewVector, viewVector)) {
        d->m_viewVector = viewVector;
        // modify the view center, not the position
        d->m_viewCenter = d->m_position + viewVector;
        d->m_matrixDirty = true;
        emit viewVectorChanged();
        emit viewCenterChanged();
        emit transformMatrixChanged();
    }
}

QVector3D QLookAtTransform::viewVector() const
{
    Q_D(const QLookAtTransform);
    return d->m_viewVector;
}

} // namespace Qt3D

QT_END_NAMESPACE
