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

#include "qtransform.h"
#include "qtransform_p.h"
#include "qabstracttransform_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <qmatrixtransform.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QTransformPrivate::QTransformPrivate(QTransform *qq)
    : QComponentPrivate(qq)
    , m_transforms()
    , m_matrix()
    , m_sceneMatrix()
{
}

void QTransformPrivate::copy(const QNodePrivate *ref)
{
    QComponentPrivate::copy(ref);
    const QTransformPrivate *transform = static_cast<const QTransformPrivate *>(ref);
    m_matrix = transform->m_matrix;
}

QTransform::QTransform(QNode *parent)
    : Qt3D::QComponent(*new QTransformPrivate(this), parent)
{
    Q_D(QTransform);
    d->m_transformsDirty.fetchAndStoreOrdered(0);
}

QTransform::QTransform(QList<QAbstractTransform *> transforms, QNode *parent)
    : Qt3D::QComponent(*new QTransformPrivate(this), parent)
{
    Q_D(QTransform);
    d->m_transformsDirty.fetchAndStoreOrdered(0);
    Q_FOREACH (QAbstractTransform *t, transforms)
        appendTransform(t);
}

QTransform::QTransform(QAbstractTransform *transform, QNode *parent)
    : Qt3D::QComponent(*new QTransformPrivate(this), parent)
{
    Q_D(QTransform);
    d->m_transformsDirty.fetchAndStoreOrdered(0);
    appendTransform(transform);
}

QTransform::QTransform(QTransformPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

QTransform *QTransform::doClone() const
{
    Q_D(const QTransform);
    QTransform *clone = new QTransform();
    clone->d_func()->copy(d_func());
    Q_FOREACH (QAbstractTransform *t, d->m_transforms)
        clone->appendTransform(qobject_cast<QAbstractTransform *>(QNodePrivate::get(t)->clone()));
    return clone;
}

void QTransform::setTransformsDirty()
{
    Q_D(QTransform);
    if (!d->m_transformsDirty.loadAcquire()) {
        d->m_transformsDirty.fetchAndStoreOrdered(1);
    }
    emit matrixChanged();
}

QMatrix4x4 QTransform::matrix() const
{
    Q_D(const QTransform);
    if (d->m_transformsDirty.loadAcquire()) {
        d->m_matrix = applyTransforms();
        d->m_transformsDirty.fetchAndStoreOrdered(0);
    }
    return d->m_matrix;
}

void QTransform::setMatrix(const QMatrix4x4 &m)
{
    Q_D(QTransform);
    qDeleteAll(d->m_transforms);
    d->m_transforms.clear();
    appendTransform(new QMatrixTransform(m));
    setTransformsDirty();
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

void QTransform::appendTransform(QAbstractTransform *transform)
{
    Q_D(QTransform);
    d->m_transforms.append(transform);
    QObject::connect(transform, SIGNAL(transformMatrixChanged()), this, SLOT(setTransformsDirty()));
    setTransformsDirty();
}

void QTransform::removeTransform(QAbstractTransform *transform)
{
    Q_D(QTransform);
    d->m_transforms.removeOne(transform);
    QObject::disconnect(transform, SIGNAL(transformMatrixChanged()), this, SLOT(setTransformsDirty()));
    setTransformsDirty();
}

QMatrix4x4 QTransform::applyTransforms() const
{
    Q_D(const QTransform);
    QMatrix4x4 m;
    Q_FOREACH (const QAbstractTransform *t, d->m_transforms)
        m = t->transformMatrix() * m;
    return m;
}

QList<QAbstractTransform *> QTransform::transformList() const
{
    Q_D(const QTransform);
    return d->m_transforms;
}

} // namespace Qt3D

QT_END_NAMESPACE
