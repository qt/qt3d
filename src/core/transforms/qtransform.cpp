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

#include "qtransform.h"
#include "qtransform_p.h"
#include "qabstracttransform_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <qmatrixtransform.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QTransformPrivate::QTransformPrivate(QTransform *qq)
    : QComponentPrivate(qq)
    , m_transformsDirty(false)
    , m_transforms()
    , m_matrix()
    , m_sceneMatrix()
{
}

void QTransform::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QTransform *transform = static_cast<const QTransform*>(ref);
    d_func()->m_matrix = transform->d_func()->m_matrix;
    Q_FOREACH (QAbstractTransform *t, transform->d_func()->m_transforms)
        addTransform(qobject_cast<QAbstractTransform *>(QNode::clone(t)));
}

QTransform::QTransform(QNode *parent)
    : Qt3D::QComponent(*new QTransformPrivate(this), parent)
{
}

QTransform::QTransform(QList<QAbstractTransform *> transforms, QNode *parent)
    : Qt3D::QComponent(*new QTransformPrivate(this), parent)
{
    Q_FOREACH (QAbstractTransform *t, transforms)
        addTransform(t);
}

QTransform::QTransform(QAbstractTransform *transform, QNode *parent)
    : Qt3D::QComponent(*new QTransformPrivate(this), parent)
{
    addTransform(transform);
}

QTransform::QTransform(QTransformPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

void QTransformPrivate::_q_update()
{
    if (!m_transformsDirty)
        m_transformsDirty = true;
    emit static_cast<QTransform *>(q_ptr)->matrixChanged();
}

QMatrix4x4 QTransformPrivate::applyTransforms() const
{
    QMatrix4x4 m;
    Q_FOREACH (const QAbstractTransform *t, m_transforms)
        m = t->transformMatrix() * m;
    return m;
}

QMatrix4x4 QTransform::matrix() const
{
    Q_D(const QTransform);
    if (d->m_transformsDirty) {
        d->m_matrix = d->applyTransforms();
        d->m_transformsDirty = false;
    }
    return d->m_matrix;
}

/*!
    The center of rotation for the entity. Defaults to the local origin.
*/
QVector3D QTransform::rotationCenter() const
{
    return QVector3D();
}

QList<QAbstractTransform *> QTransform::transforms() const
{
    Q_D(const QTransform);
    return d->m_transforms;
}

/*!
 *   The center of rotation for the entity. Defaults to the local origin.
 */
void QTransform::setRotationCenter(const QVector3D &rc)
{
    Q_UNUSED(rc);
}

void QTransform::addTransform(QAbstractTransform *transform)
{
    Q_D(QTransform);
    d->m_transforms.append(transform);
    QObject::connect(transform, SIGNAL(transformMatrixChanged()), this, SLOT(_q_update()));
    d->_q_update();
}

void QTransform::removeTransform(QAbstractTransform *transform)
{
    Q_D(QTransform);
    d->m_transforms.removeOne(transform);
    QObject::disconnect(transform, SIGNAL(transformMatrixChanged()), this, SLOT(_q_update()));
    d->_q_update();
}

QList<QAbstractTransform *> QTransform::transformList() const
{
    Q_D(const QTransform);
    return d->m_transforms;
}

} // namespace Qt3D

QT_END_NAMESPACE

#include "moc_qtransform.cpp"
