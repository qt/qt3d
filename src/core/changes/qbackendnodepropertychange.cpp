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

#include "qbackendnodepropertychange.h"
#include "qbackendnodepropertychange_p.h"
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/private/qbackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QBackendNodePropertyChangeBasePrivate::QBackendNodePropertyChangeBasePrivate()
    : QNodePropertyChangeBasePrivate()
{
}

QBackendNodePropertyChangeBasePrivate::~QBackendNodePropertyChangeBasePrivate()
{
}

/*!
 * \class Qt3DCore::QBackendNodePropertyChangeBase
 * \inherits Qt3DCore::QNodePropertyChangeBase
 * \inmodule Qt3DCore
 *
 * TODO
 */
QBackendNodePropertyChangeBase::QBackendNodePropertyChangeBase(QNodeId subjectId, Priority priority)
    : QNodePropertyChangeBase(*new QBackendNodePropertyChangeBasePrivate, BackendNode, subjectId, priority)
{
    setDeliveryFlags(DeliverToAll);
}

/*!
 * \internal
 */
QBackendNodePropertyChangeBase::QBackendNodePropertyChangeBase(QBackendNodePropertyChangeBasePrivate &dd,
                                                               QNodeId subjectId, Priority priority)
    : QNodePropertyChangeBase(dd, BackendNode, subjectId, priority)
{
    setDeliveryFlags(DeliverToAll);
}

QBackendNodePropertyChangeBase::~QBackendNodePropertyChangeBase()
{
}

/*!
 * \class Qt3DCore::QBackendNodePropertyChange
 * \inherits Qt3DCore::QBackendNodePropertyChangeBase
 * \inmodule Qt3DCore
 *
 * TODO
 */
QBackendNodePropertyChangePrivate::QBackendNodePropertyChangePrivate()
    : QBackendNodePropertyChangeBasePrivate()
{
}

QBackendNodePropertyChangePrivate::~QBackendNodePropertyChangePrivate()
{
}

QBackendNodePropertyChange::QBackendNodePropertyChange(QNodeId subjectId, QSceneChange::Priority priority)
    : QBackendNodePropertyChangeBase(*new QBackendNodePropertyChangePrivate, subjectId, priority)
{
}

/*!
 * \internal
 */
QBackendNodePropertyChange::QBackendNodePropertyChange(QBackendNodePropertyChangePrivate &dd,
                                                       QNodeId subjectId, QSceneChange::Priority priority)
    : QBackendNodePropertyChangeBase(dd, subjectId, priority)
{
}

QBackendNodePropertyChange::~QBackendNodePropertyChange()
{
}

/*!
 * \return property value.
 */
QVariant QBackendNodePropertyChange::value() const
{
    Q_D(const QBackendNodePropertyChange);
    return d->m_value;
}

/*!
 * Set the property change \a value.
 */
void QBackendNodePropertyChange::setValue(const QVariant &value)
{
    Q_D(QBackendNodePropertyChange);
    d->m_value = value;
}

/*!
 * \typedef Qt3DCore::QBackendNodePropertyChangePtr
 * \relates Qt3DCore::QBackendNodePropertyChange
 *
 * A shared pointer for QBackendNodePropertyChange.
 */

} // Qt3D

QT_END_NAMESPACE
