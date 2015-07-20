/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qrotatetransform.h"
#include "qrotatetransform_p.h"

#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QRotateTransformPrivate
    \internal
*/
QRotateTransformPrivate::QRotateTransformPrivate()
    : QAbstractTransformPrivate()
    , m_angleDeg(0)
    , m_axis(0.0f, 1.0f, 0.0f)
{
    updateMatrix();
}

void QRotateTransformPrivate::updateMatrix()
{
    QMatrix4x4 m;
    m.rotate(m_angleDeg, m_axis);
    m_matrix = m;
}


QRotateTransform::QRotateTransform(QObject *parent)
    : QAbstractTransform(*new QRotateTransformPrivate, parent)
{
}

/*! \internal */
QRotateTransform::QRotateTransform(QRotateTransformPrivate &dd, QObject *parent)
    : QAbstractTransform(dd, parent)
{
}

float QRotateTransform::angleDeg() const
{
    Q_D(const QRotateTransform);
    return d->m_angleDeg;
}

float Qt3D::QRotateTransform::angleRad() const
{
    Q_D(const QRotateTransform);
    return qDegreesToRadians(d->m_angleDeg);
}

QVector3D QRotateTransform::axis() const
{
    Q_D(const QRotateTransform);
    return d->m_axis;
}

QMatrix4x4 QRotateTransform::transformMatrix() const
{
    Q_D(const QRotateTransform);
    return d->m_matrix;
}

void QRotateTransform::setAngleDeg(float arg)
{
    Q_D(QRotateTransform);
    if (d->m_angleDeg != arg) {
        d->m_angleDeg = arg;
        d->updateMatrix();
        emit angleChanged();
        emit transformMatrixChanged();
    }
}

void QRotateTransform::setAngleRad(float arg)
{
    setAngleDeg(qRadiansToDegrees(arg));
}

void QRotateTransform::setAxis(const QVector3D& arg)
{
    Q_D(QRotateTransform);
    if (d->m_axis != arg) {
        d->m_axis = arg;
        d->updateMatrix();
        emit axisChanged();
        emit transformMatrixChanged();
    }
}

} // namespace Qt3D

QT_END_NAMESPACE
