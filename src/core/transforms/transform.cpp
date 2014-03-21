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

namespace Qt3D {

Transform::Transform(Node *parent)
    : Qt3D::Component(parent),
      m_transformsDirty(false)
{
}

QQmlListProperty<AbstractTransform> Transform::transformList()
{
    return QQmlListProperty<AbstractTransform>(this, 0,
                                               Transform::qmlAppendTransform,
                                               Transform::transformCount,
                                               Transform::transformAt,
                                               Transform::qmlClearTransforms);
}

QMatrix4x4 Transform::matrix() const
{
    if (m_transformsDirty) {
        m_matrix = applyTransforms();
        m_transformsDirty = false;
    }
    return m_matrix;
}

void Transform::setMatrix(const QMatrix4x4 &m)
{
    qDeleteAll(m_transforms);
    m_transforms.clear();
    appendTransfrom(new MatrixTransform(m));
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

void Transform::appendTransfrom(AbstractTransform *xform)
{
    m_transformsDirty = true;
    m_transforms.append( xform );
}

void Transform::removeTransform(AbstractTransform *xform)
{
    m_transformsDirty = true;
    m_transforms.removeOne( xform );
}

QMatrix4x4 Transform::applyTransforms() const
{
    QMatrix4x4 m;
    m.setToIdentity();
    Q_FOREACH (AbstractTransform *t, m_transforms)
        m = t->matrix() * m;
    return m;
}


void Transform::qmlAppendTransform(QQmlListProperty<AbstractTransform> *list, AbstractTransform *obj  )
{
    if ( !obj )
        return;

    Transform *self = static_cast<Transform *>(list->object);
    self->appendTransfrom(obj);
}

AbstractTransform* Transform::transformAt(QQmlListProperty<AbstractTransform> *list, int index)
{
    Transform *self = static_cast<Transform *>(list->object);
    return self->transforms().at(index);
}

int Transform::transformCount(QQmlListProperty<AbstractTransform> *list)
{
    Transform *self = static_cast<Transform *>(list->object);
    return self->transforms().count();
}

void Transform::qmlClearTransforms(QQmlListProperty<AbstractTransform> *list)
{
    Transform *self = static_cast<Transform *>(list->object);
    qDeleteAll(self->m_transforms);
    self->m_transforms.clear();
    self->m_transformsDirty = true;
}

} // namespace Qt3D
