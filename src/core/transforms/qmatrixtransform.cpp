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

#include "qmatrixtransform.h"
#include "qmatrixtransform_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QMatrixTransformPrivate::QMatrixTransformPrivate(QMatrixTransform *qq)
    : QAbstractTransformPrivate(qq)
{
}

QMatrixTransform::QMatrixTransform(QMatrixTransformPrivate &dd, QNode *parent)
    : QAbstractTransform(dd, parent)
{
}

void QMatrixTransform::copy(const QNode *ref)
{
    QAbstractTransform::copy(ref);
    const QMatrixTransform *matrix = static_cast<const QMatrixTransform*>(ref);
    d_func()->m_matrix = matrix->d_func()->m_matrix;
}

QMatrixTransform::QMatrixTransform(QNode *parent)
    : QAbstractTransform(*new QMatrixTransformPrivate(this), parent)
{
}

QMatrixTransform::QMatrixTransform(const QMatrix4x4& m, QNode *parent)
    : QAbstractTransform(*new QMatrixTransformPrivate(this), parent)
{
    Q_D(QMatrixTransform);
    d->m_matrix = m;
}

QMatrix4x4 QMatrixTransform::matrix() const
{
    Q_D(const QMatrixTransform);
    return d->m_matrix;
}

void QMatrixTransform::setMatrix(const QMatrix4x4 &matrix)
{
    Q_D(QMatrixTransform);
    if (d->m_matrix != matrix) {
        d->m_matrix = matrix;
        emit matrixChanged();
        emit transformMatrixChanged();
    }
}

QMatrix4x4 QMatrixTransform::transformMatrix() const
{
    return matrix();
}

} // namespace Qt3D

QT_END_NAMESPACE
