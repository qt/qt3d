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

#include "qtranslatetransform.h"
#include "qtranslatetransform_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {


/*!
    \class Qt3D::QTranslateTransformPrivate
    \internal
*/
QTranslateTransformPrivate::QTranslateTransformPrivate()
    : QAbstractTransformPrivate()
{
}


QTranslateTransform::QTranslateTransform(QObject *parent)
    : QAbstractTransform(*new QTranslateTransformPrivate, parent)
{
}

/*! \internal */
QTranslateTransform::QTranslateTransform(QTranslateTransformPrivate &dd, QObject *parent)
    : QAbstractTransform(dd, parent)
{
}

float QTranslateTransform::dx() const
{
    Q_D(const QTranslateTransform);
    return d->m_translation.x();
}

float QTranslateTransform::dy() const
{
    Q_D(const QTranslateTransform);
    return d->m_translation.y();
}

float QTranslateTransform::dz() const
{
    Q_D(const QTranslateTransform);
    return d->m_translation.z();
}

QVector3D QTranslateTransform::translation() const
{
    Q_D(const QTranslateTransform);
    return d->m_translation;
}

QMatrix4x4 QTranslateTransform::transformMatrix() const
{
    Q_D(const QTranslateTransform);
    QMatrix4x4 r;
    r.translate(d->m_translation);
    return r;
}

void QTranslateTransform::setDx(float arg)
{
    Q_D(QTranslateTransform);
    if (arg != d->m_translation.x()) {
        d->m_translation.setX(arg);
        emit translateChanged();
        emit transformMatrixChanged();
    }
}

void QTranslateTransform::setDy(float arg)
{
    Q_D(QTranslateTransform);
    if (arg != d->m_translation.y()) {
        d->m_translation.setY(arg);
        emit translateChanged();
        emit transformMatrixChanged();
    }
}

void QTranslateTransform::setDz(float arg)
{
    Q_D(QTranslateTransform);
    if (arg != d->m_translation.z()) {
        d->m_translation.setZ(arg);
        emit translateChanged();
        emit transformMatrixChanged();
    }
}

void QTranslateTransform::setTranslation(const QVector3D &arg)
{
    Q_D(QTranslateTransform);
    if (d->m_translation != arg) {
        d->m_translation = arg;
        emit translateChanged();
        emit transformMatrixChanged();
    }
}

} // namespace Qt3D

QT_END_NAMESPACE
