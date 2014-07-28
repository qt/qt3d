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

#include "qscaletransform.h"
#include "qscaletransform_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D
{

QScaleTransformPrivate::QScaleTransformPrivate(QScaleTransform *qq)
    : QAbstractTransformPrivate(qq)
    , m_scale3D(1.0f, 1.0f, 1.0f)
{
}

QScaleTransform::QScaleTransform(QNode *parent) :
    QAbstractTransform(*new QScaleTransformPrivate(this), parent)
{
}

void QScaleTransform::copy(const QNode *ref)
{
    Q_D(QScaleTransform);
    QAbstractTransform::copy(ref);
    const QScaleTransform *transform = qobject_cast<const QScaleTransform *>(ref);
    if (ref != Q_NULLPTR) {
        d->m_scale3D = transform->scale3D();
    }
}

QScaleTransform::QScaleTransform(QScaleTransformPrivate &dd, QNode *parent)
    : QAbstractTransform(dd, parent)
{
}

QScaleTransform *QScaleTransform::doClone(QNode *clonedParent) const
{
    return new QScaleTransform(clonedParent);
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
        emit transformUpdated();
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

QMatrix4x4 QScaleTransform::matrix() const
{
    Q_D(const QScaleTransform);
    QMatrix4x4 mat;
    mat.scale(d->m_scale3D);
    return mat;
}

} // Qt3D

QT_END_NAMESPACE
