/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qpickevent.h"
#include "qpickevent_p.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QPickEvent::QPickEvent()
    : QObject(*new QPickEventPrivate())
{
}

QPickEvent::QPickEvent(const QPointF &position, const QVector3D &intersection, const QVector3D &localIntersection, float distance)
    : QObject(*new QPickEventPrivate())
{
    Q_D(QPickEvent);
    d->m_position = position;
    d->m_distance = distance;
    d->m_worldIntersection = intersection;
    d->m_localIntersection = localIntersection;
}

QPickEvent::QPickEvent(QObjectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{

}

QPickEvent::~QPickEvent()
{
}

bool QPickEvent::isAccepted() const
{
    Q_D(const QPickEvent);
    return d->m_accepted;
}

void QPickEvent::setAccepted(bool accepted)
{
    Q_D(QPickEvent);
    if (accepted != d->m_accepted) {
        d->m_accepted = accepted;
        emit acceptedChanged(accepted);
    }
}

QPointF QPickEvent::position() const
{
    Q_D(const QPickEvent);
    return d->m_position;
}

float QPickEvent::distance() const
{
    Q_D(const QPickEvent);
    return d->m_distance;
}

QVector3D QPickEvent::worldIntersection() const
{
    Q_D(const QPickEvent);
    return d->m_worldIntersection;
}

QVector3D QPickEvent::localIntersection() const
{
    Q_D(const QPickEvent);
    return d->m_localIntersection;
}

} // Qt3DRender

QT_END_NAMESPACE

