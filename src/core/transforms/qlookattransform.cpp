/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlookattransform.h"
#include "qlookattransform_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QLookAtTransformPrivate::QLookAtTransformPrivate(QLookAtTransform *qq)
    : QAbstractTransformPrivate(qq)
    , m_matrixDirty(true)
{
}

QLookAtTransform::QLookAtTransform(QNode *parent)
    : QAbstractTransform(*new QLookAtTransformPrivate(this), parent)
{
}

void QLookAtTransform::copy(const QNode *ref)
{
    QAbstractTransform::copy(ref);
    const QLookAtTransform *transform = static_cast<const QLookAtTransform*>(ref);
    d_func()->m_matrix = transform->d_func()->m_matrix;
    d_func()->m_position = transform->d_func()->m_position;
    d_func()->m_upVector = transform->d_func()->m_upVector;
    d_func()->m_viewCenter = transform->d_func()->m_viewCenter;
    d_func()->m_viewVector = transform->d_func()->m_viewVector;
}

QLookAtTransform::QLookAtTransform(QLookAtTransformPrivate &dd, QNode *parent)
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
