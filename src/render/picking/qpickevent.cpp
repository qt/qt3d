/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
};

QPickEvent::QPickEvent()
    : QObject(*new QPickEventPrivate())
{
}

QPickEvent::QPickEvent(const QVector3D &intersection, const QVector3D &localIntersection, float distance)
    : QObject(*new QPickEventPrivate())
{
    Q_D(QPickEvent);
    d->m_distance = distance;
    d->m_worldIntersection = intersection;
    d->m_localIntersection = localIntersection;
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

} // Qt3DRender

QT_END_NAMESPACE

