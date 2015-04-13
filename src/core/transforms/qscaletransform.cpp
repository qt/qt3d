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

#include "qscaletransform.h"
#include "qscaletransform_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D
{

/*!
    \class Qt3D::QScaleTransformPrivate
    \internal
*/
QScaleTransformPrivate::QScaleTransformPrivate()
    : QAbstractTransformPrivate()
    , m_scale3D(1.0f, 1.0f, 1.0f)
{
}


QScaleTransform::QScaleTransform(QObject *parent)
    : QAbstractTransform(*new QScaleTransformPrivate, parent)
{
}

/*! \internal */
QScaleTransform::QScaleTransform(QScaleTransformPrivate &dd, QObject *parent)
    : QAbstractTransform(dd, parent)
{
}

QVector3D QScaleTransform::scale3D() const
{
    Q_D(const QScaleTransform);
    return d->m_scale3D;
}

void QScaleTransform::setScale3D(const QVector3D &scale3D)
{
    Q_D(QScaleTransform);
    if (scale3D != d->m_scale3D) {
        d->m_scale3D = scale3D;
        emit scale3DChanged();
        emit transformMatrixChanged();
    }
}

float QScaleTransform::scale() const
{
    Q_D(const QScaleTransform);
    return d->m_scale3D.x();
}

void QScaleTransform::setScale(float scale)
{
    Q_D(QScaleTransform);
    if (scale != d->m_scale3D.x()) {
        setScale3D(QVector3D(scale, scale, scale));
        emit scaleChanged();
    }
}

QMatrix4x4 QScaleTransform::transformMatrix() const
{
    Q_D(const QScaleTransform);
    QMatrix4x4 mat;
    mat.scale(d->m_scale3D);
    return mat;
}

} // Qt3D

QT_END_NAMESPACE
