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

#include "qscenechange.h"
#include "qscenechange_p.h"
#include <QDateTime>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \enum Qt3D::ChangeFlag

    The types of change that can be sent and received by Qt3D's change notification system.

    \value NodeCreated A new instance of a QNode subclass has been created.
    \value NodeAboutToBeDeleted A QNode is about to be deleted from the scene.
    \value NodeDeleted A QNode has been deleted.
    \value NodeUpdated A QNode property has been updated.
    \value NodeAdded A QNode has been added to the scene.
    \value NodeRemoved A QNode has been removed from the scene.
    \value ComponentAdded A QComponent has been added to a QEntity.
    \value ComponentRemoved A QComponent has been removed from a QEntity.
    \value AllChanges Allows an observer to monitor for any of the above changes.
*/

/*!
    \class Qt3D::QSceneChangePrivate
    \internal
*/
QSceneChangePrivate::QSceneChangePrivate()
    : q_ptr(Q_NULLPTR)
{
}

QSceneChangePrivate::~QSceneChangePrivate()
{
}

QSceneChange::QSceneChange(ChangeFlag type, ObservableType observableType, const QNodeId &subjectId, QSceneChange::Priority priority)
    : d_ptr(new QSceneChangePrivate)
{
    d_ptr->q_ptr = this;
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subjectId = subjectId;
    d->m_subjectType = observableType;
}

QSceneChange::~QSceneChange()
{
    delete d_ptr;
}

/*! \internal */
QSceneChange::QSceneChange(QSceneChangePrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*! \internal */
QSceneChange::QSceneChange(QSceneChangePrivate &dd, ChangeFlag type, ObservableType observableType, const QNodeId &subjectId, QSceneChange::Priority priority)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subjectId = subjectId;
    d->m_subjectType = observableType;
}

ChangeFlag QSceneChange::type() const
{
    Q_D(const QSceneChange);
    return d->m_type;
}

QSceneChange::Priority QSceneChange::priority() const
{
    Q_D(const QSceneChange);
    return d->m_priority;
}

qint64 QSceneChange::timestamp() const
{
    Q_D(const QSceneChange);
    return d->m_timestamp;
}

QSceneChange::ObservableType QSceneChange::observableType() const
{
    Q_D(const QSceneChange);
    return d->m_subjectType;
}

QNodeId QSceneChange::subjectId() const
{
    Q_D(const QSceneChange);
    return d->m_subjectId;
}

} // Qt3D

QT_END_NAMESPACE
