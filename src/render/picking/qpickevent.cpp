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
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPickEventPrivate : public QObjectPrivate
{
public:
    QPickEventPrivate()
        : QObjectPrivate()
        , m_accepted(true)
        , m_distance(-1.f)
    {
    }

    bool m_accepted;
    QVector3D m_worldIntersection;
    QVector3D m_localIntersection;
    float m_distance;
    uint m_triangleIndex;
    uint m_vertex1Index;
    uint m_vertex2Index;
    uint m_vertex3Index;
};

QPickEvent::QPickEvent()
    : QObject(*new QPickEventPrivate())
{
}

QPickEvent::QPickEvent(const QVector3D &intersection, const QVector3D &localIntersection, float distance,
                       uint triangleIndex, uint vertex1Index, uint vertex2Index, uint vertex3Index)
    : QObject(*new QPickEventPrivate())
{
    Q_D(QPickEvent);
    d->m_distance = distance;
    d->m_worldIntersection = intersection;
    d->m_localIntersection = localIntersection;
    d->m_triangleIndex = triangleIndex;
    d->m_vertex1Index = vertex1Index;
    d->m_vertex2Index = vertex2Index;
    d->m_vertex3Index = vertex3Index;
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

float QPickEvent::distance() const
{
    Q_D(const QPickEvent);
    return d->m_distance;
}

const QVector3D &QPickEvent::worldIntersection() const
{
    Q_D(const QPickEvent);
    return d->m_worldIntersection;
}

const QVector3D &QPickEvent::localIntersection() const
{
    Q_D(const QPickEvent);
    return d->m_localIntersection;
}

uint QPickEvent::triangleIndex() const
{
    Q_D(const QPickEvent);
    return d->m_triangleIndex;
}

uint QPickEvent::vertex1Index() const
{
    Q_D(const QPickEvent);
    return d->m_vertex1Index;
}

uint QPickEvent::vertex2Index() const
{
    Q_D(const QPickEvent);
    return d->m_vertex2Index;
}

uint QPickEvent::vertex3Index() const
{
    Q_D(const QPickEvent);
    return d->m_vertex3Index;
}

} // Qt3DRender

QT_END_NAMESPACE

