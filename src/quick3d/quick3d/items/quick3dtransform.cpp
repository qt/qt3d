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

#include "quick3dtransform.h"
#include <Qt3DCore/qabstracttransform.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

/*!
    \qmltype Transform
    \inqmlmodule Qt3D
    \since 5.5
*/
Quick3DTransform::Quick3DTransform(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(qobject_cast<Qt3D::QTransform *>(parent) != Q_NULLPTR);
}

/*!
    \qmlproperty matrix4x4 Qt3D::Transform::matrix
    \readonly
*/

/*!
    \qmlproperty list<QAbstractTransform> Qt3D::Transform::transforms
    \default
*/
QQmlListProperty<QAbstractTransform> Quick3DTransform::transformList()
{
    return QQmlListProperty<QAbstractTransform>(this, 0,
                                               Quick3DTransform::qmlAppendTransform,
                                               Quick3DTransform::transformCount,
                                               Quick3DTransform::transformAt,
                                               Quick3DTransform::qmlClearTransforms);
}

void Quick3DTransform::qmlAppendTransform(QQmlListProperty<QAbstractTransform> *list, QAbstractTransform *obj  )
{
    if ( !obj )
        return;

    Quick3DTransform *self = static_cast<Quick3DTransform *>(list->object);
    self->parentTransform()->addTransform(obj);
}

QAbstractTransform* Quick3DTransform::transformAt(QQmlListProperty<QAbstractTransform> *list, int index)
{
    Quick3DTransform *self = static_cast<Quick3DTransform *>(list->object);
    return self->parentTransform()->transforms().at(index);
}

int Quick3DTransform::transformCount(QQmlListProperty<QAbstractTransform> *list)
{
    Quick3DTransform *self = static_cast<Quick3DTransform *>(list->object);
    return self->parentTransform()->transforms().count();
}

void Quick3DTransform::qmlClearTransforms(QQmlListProperty<QAbstractTransform> *list)
{
    Quick3DTransform *self = static_cast<Quick3DTransform *>(list->object);
    Q_FOREACH (QAbstractTransform *trans, self->parentTransform()->transforms())
        self->parentTransform()->removeTransform(trans);
}

} //Quick

} //Qt3D

QT_END_NAMESPACE
