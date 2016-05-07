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

#include "qnodepropertychange.h"
#include "qnodepropertychange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*!
    \class Qt3DCore::QNodePropertyChange
    \inmodule Qt3DCore
*/
QNodePropertyChangePrivate::QNodePropertyChangePrivate()
    : QNodePropertyChangeBasePrivate()
{
}

QNodePropertyChangePrivate::~QNodePropertyChangePrivate()
{
}

/*!
 * \class Qt3DCore::QNodePropertyChange
 * \inmodule Qt3DCore
 *
 * TODO
 */

/*!
 * \typedef Qt3DCore::QNodePropertyChangePtr
 * \relates Qt3DCore::QNodePropertyChange
 *
 * A shared pointer for QNodePropertyChange.
 */

/*!
 * Constructs a new QNodePropertyChange with \a type, \a senderType, \a subjectId, and
 * \a priority.
 */
QNodePropertyChange::QNodePropertyChange(ChangeFlag type, SenderType senderType,
                                         QNodeId subjectId, QSceneChange::Priority priority)
    : QNodePropertyChangeBase(*new QNodePropertyChangePrivate, type, senderType, subjectId, priority)
{
}

/*!
 * Constructs a new QNodePropertyChange with \a subjectId and \a priority.
 */
QNodePropertyChange::QNodePropertyChange(QNodeId subjectId, Priority priority)
    : QNodePropertyChangeBase(*new QNodePropertyChangePrivate, PropertyUpdated, Node, subjectId, priority)
{
}

QNodePropertyChange::QNodePropertyChange(QNodePropertyChangePrivate &dd,
                                         ChangeFlag type, SenderType senderType,
                                         QNodeId subjectId, QSceneChange::Priority priority)
    : QNodePropertyChangeBase(dd, type, senderType, subjectId, priority)
{
}

/*!
 * \internal
 */
QNodePropertyChange::QNodePropertyChange(QNodePropertyChangePrivate &dd,
                                         QNodeId subjectId, QSceneChange::Priority priority)
    : QNodePropertyChangeBase(dd, PropertyUpdated, Node, subjectId, priority)
{
}

QNodePropertyChange::~QNodePropertyChange()
{
}

/*!
 * \return property value.
 */
QVariant QNodePropertyChange::value() const
{
    Q_D(const QNodePropertyChange);
    return d->m_value;
}

/*!
 * Set the property change \a value.
 */
void QNodePropertyChange::setValue(const QVariant &value)
{
    Q_D(QNodePropertyChange);
    d->m_value = value;
}

} // Qt3D

QT_END_NAMESPACE
