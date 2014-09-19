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

#include "qboxcollider.h"
#include "qboxcollider_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace BulletPhysics {

QBoxColliderPrivate::QBoxColliderPrivate(QBoxCollider *qq)
    : QAbstractColliderPrivate(qq)
    , m_center()
    , m_halfExtents()
{
}


QBoxCollider::QBoxCollider(Qt3D::QNode *parent)
    : QAbstractCollider(*new QBoxColliderPrivate(this), parent)
{
}

QBoxCollider::QBoxCollider(QBoxColliderPrivate &dd, QNode *parent)
    : QAbstractCollider(dd, parent)
{
}

void QBoxCollider::copy(const QNode *ref)
{
    Q_D(QBoxCollider);
    QAbstractCollider::copy(ref);
    const QBoxCollider *other = qobject_cast<const QBoxCollider *>(ref);
    if (other != Q_NULLPTR) {
        const QBoxColliderPrivate *d_other = other->d_func();
        d->m_center = d_other->m_center;
        d->m_halfExtents = d_other->m_halfExtents;
    }
}

QBoxCollider *QBoxCollider::doClone(bool isClone) const
{
    QBoxCollider *clone = new QBoxCollider();
    clone->copy(this);
    clone->d_func()->m_isClone = isClone;
    return clone;
}

void QBoxCollider::setCenter(const QVector3D &center)
{
    Q_D(QBoxCollider);
    if (d->m_center != center) {
        d->m_center = center;
        emit centerChanged();
    }
}

QVector3D QBoxCollider::center() const
{
    Q_D(const QBoxCollider);
    return d->m_center;
}

void QBoxCollider::setHalfExtents(const QVector3D &halfExtents)
{
    Q_D(QBoxCollider);
    if (d->m_halfExtents != halfExtents) {
        d->m_halfExtents = halfExtents;
        emit halfExtentsChanged();
    }
}

QVector3D QBoxCollider::halfExtents() const
{
    Q_D(const QBoxCollider);
    return d->m_halfExtents;
}

} // namespace BulletPhysics
} // namespace Qt3D

QT_END_NAMESPACE
