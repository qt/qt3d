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

#include "transform.h"

#include <matrixtransform.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

Transform::Transform(Node *parent)
    : Qt3D::Component(parent)
{
    m_transformsDirty.fetchAndStoreOrdered(0);
}

void Transform::setTransformsDirty()
{
    if (!m_transformsDirty.loadAcquire()) {
        m_transformsDirty.fetchAndStoreOrdered(1);
        QScenePropertyChangePtr e(new QScenePropertyChange(ComponentUpdated, this));
        e->m_propertyName = QByteArrayLiteral("matrix");
        e->m_value = matrix();
        notifySceneChange(e);
    }
}

QMatrix4x4 Transform::matrix() const
{
    if (m_transformsDirty.loadAcquire()) {
        m_matrix = applyTransforms();
        m_transformsDirty.fetchAndStoreOrdered(0);
    }
    return m_matrix;
}

void Transform::setMatrix(const QMatrix4x4 &m)
{
    qDeleteAll(m_transforms);
    m_transforms.clear();
    appendTransform(new MatrixTransform(m));
    setTransformsDirty();
}

/*!
    The center of rotation for the entity. Defaults to the local origin.
*/
QVector3D Transform::rotationCenter() const
{
    return QVector3D();
}

QList<AbstractTransform *> Transform::transforms() const
{
    return m_transforms;
}

/*!
 *   The center of rotation for the entity. Defaults to the local origin.
 */
void Transform::setRotationCenter(const QVector3D &rc)
{
    Q_UNUSED(rc);
}

void Transform::appendTransform(AbstractTransform *xform)
{
    m_transforms.append( xform );
    QObject::connect(xform, SIGNAL(transformUpdated()), this, SLOT(setTransformsDirty()));
    setTransformsDirty();
}

void Transform::removeTransform(AbstractTransform *xform)
{
    m_transforms.removeOne( xform );
    QObject::disconnect(xform, SIGNAL(transformUpdated()), this, SLOT(setTransformsDirty()));
    setTransformsDirty();
}

QMatrix4x4 Transform::applyTransforms() const
{
    QMatrix4x4 m;
    m.setToIdentity();
    Q_FOREACH (AbstractTransform *t, m_transforms)
        m = t->matrix() * m;
    return m;
}

} // namespace Qt3D

QT_END_NAMESPACE
