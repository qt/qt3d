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

#include "qscenechange.h"
#include "qscenechange_p.h"
#include <QDateTime>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*!
 * \enum Qt3DCore::ChangeFlag
 * \relates Qt3DCore::QSceneChange
 *
 * The types of change that can be sent and received by Qt3D's change notification system.
 *
 * \value NodeCreated A new instance of a QNode subclass has been created.
 * \value NodeDeleted A QNode has been deleted.
 * \value NodeUpdated A QNode property has been updated.
 * \value NodeAdded A QNode has been added to the scene.
 * \value NodeRemoved A QNode has been removed from the scene.
 * \value ComponentAdded A QComponent has been added to a QEntity.
 * \value ComponentRemoved A QComponent has been removed from a QEntity.
 * \value AllChanges Allows an observer to monitor for any of the above changes.
 */

QSceneChangePrivate::QSceneChangePrivate()
    : q_ptr(nullptr)
    , m_subjectId()
    , m_senderType(QSceneChange::Node)
    , m_type(AllChanges)
    , m_priority(QSceneChange::Standard)
    , m_timestamp(QDateTime::currentMSecsSinceEpoch())
{
}

QSceneChangePrivate::~QSceneChangePrivate()
{
}

/*!
 * \enum Qt3DCore::QSceneChange::Priority
 *
 * The priority of scene change.
 *
 * \value High
 * \value Standard
 * \value Low
 */

/*!
 * \enum Qt3DCore::QSceneChange::ObservableType
 *
 * TODO
 *
 * \value Observable
 * \value Node
 */

/*!
 * \class Qt3DCore::QSceneChange
 * \inmodule Qt3DCore
 *
 * TODO
 */

/*!
 * \typedef Qt3DCore::QSceneChangePtr
 * \relates Qt3DCore::QSceneChange
 *
 * A shared pointer for QSceneChange.
 */

/*!
 * Constructs a new QSceneChange with \a type, \a senderType, \a subjectId, and \a priority.
 */
QSceneChange::QSceneChange(ChangeFlag type, SenderType senderType, QNodeId subjectId,
                           QSceneChange::Priority priority)
    : d_ptr(new QSceneChangePrivate)
{
    d_ptr->q_ptr = this;
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subjectId = subjectId;
    d->m_senderType = senderType;
}

/*! \internal */
QSceneChange::QSceneChange(QSceneChangePrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*! \internal */
QSceneChange::QSceneChange(QSceneChangePrivate &dd, ChangeFlag type, SenderType senderType, QNodeId subjectId, QSceneChange::Priority priority)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
    Q_D(QSceneChange);
    d->m_type = type;
    d->m_priority = priority;
    d->m_timestamp = QDateTime::currentMSecsSinceEpoch();
    d->m_subjectId = subjectId;
    d->m_senderType = senderType;
}

QSceneChange::~QSceneChange()
{
    delete d_ptr;
}

/*!
 * \return scene change type.
 */
ChangeFlag QSceneChange::type() const Q_DECL_NOEXCEPT
{
    Q_D(const QSceneChange);
    return d->m_type;
}

/*!
 * \return scene change priority.
 */
QSceneChange::Priority QSceneChange::priority() const Q_DECL_NOEXCEPT
{
    Q_D(const QSceneChange);
    return d->m_priority;
}

/*!
 * \return scene change timestamp.
 */
qint64 QSceneChange::timestamp() const Q_DECL_NOEXCEPT
{
    Q_D(const QSceneChange);
    return d->m_timestamp;
}

/*!
 * \return scene change observable type.
 */
QSceneChange::SenderType QSceneChange::senderType() const Q_DECL_NOEXCEPT
{
    Q_D(const QSceneChange);
    return d->m_senderType;
}

/*!
 * \return scene change subject id.
 */
QNodeId QSceneChange::subjectId() const Q_DECL_NOEXCEPT
{
    Q_D(const QSceneChange);
    return d->m_subjectId;
}

} // Qt3D

QT_END_NAMESPACE
