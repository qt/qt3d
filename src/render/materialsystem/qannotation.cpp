/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qannotation.h"
#include "qannotation_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {


/*!
    \class Qt3DRender::QAnnotationPrivate
    \internal
*/
QAnnotationPrivate::QAnnotationPrivate()
    : QNodePrivate()
{
}

/*!
    \class Qt3DRender::QAnnotation
    \inmodule Qt3DRender
    \since 5.5
    \brief The QAnnotation class provides ...
*/

/*!
    \qmltype Annotation
    \instantiates Qt3DRender::QAnnotation
    \inherits Node
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief For annotating ...
*/

/*!
  Copies the annotation in \a ref into this annotation.
 */
void QAnnotation::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAnnotation *criterion = static_cast<const QAnnotation*>(ref);
    d_func()->m_name = criterion->d_func()->m_name;
    d_func()->m_value = criterion->d_func()->m_value;
}

/*!
  The constructor creats a new annotation under \a parent.
 */
QAnnotation::QAnnotation(QNode *parent)
    : QNode(*new QAnnotationPrivate, parent)
{
}

QAnnotation::~QAnnotation()
{
    QNode::cleanup();
}

void QAnnotation::setValue(const QVariant &value)
{
    Q_D(QAnnotation);
    if (value != d->m_value) {
        d->m_value = value;
        emit valueChanged(value);
    }
}

void QAnnotation::setName(const QString &name)
{
    Q_D(QAnnotation);
    if (name != d->m_name) {
        d->m_name = name;
        emit nameChanged(name);
    }
}

/*!
  \property QAnnotation::value

*/

/*!
  \qmlproperty variant Qt3D.Render::Annotation::value
*/

QVariant QAnnotation::value() const
{
    Q_D(const QAnnotation);
    return d->m_value;
}

/*!
  \property QAnnotation::name

 */

/*!
  \qmlproperty string Qt3D.Render::Annotation::name
 */

QString QAnnotation::name() const
{
    Q_D(const QAnnotation);
    return d->m_name;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
