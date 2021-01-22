/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qpropertyvalueremovedchange.h"

#include "qpropertyvalueremovedchange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QPropertyValueRemovedChangePrivate::QPropertyValueRemovedChangePrivate()
    : QStaticPropertyValueRemovedChangeBasePrivate()
{
}


/*!
 * \class Qt3DCore::QPropertyValueRemovedChange
 * \inheaderfile Qt3DCore/QPropertyValueRemovedChange
 * \inherits Qt3DCore::QStaticPropertyValueRemovedChangeBase
 * \inmodule Qt3DCore
 * \brief Used to notify when a value is added to a property.
 *
 */

/*!
 * \typedef Qt3DCore::QPropertyValueRemovedChangePtr
 * \relates Qt3DCore::QPropertyValueRemovedChange
 *
 * A shared pointer for QPropertyValueRemovedChange.
 */

/*!
 * Constructs a new QPropertyValueRemovedChange with \a subjectId.
 */
QPropertyValueRemovedChange::QPropertyValueRemovedChange(QNodeId subjectId)
    : QStaticPropertyValueRemovedChangeBase(*new QPropertyValueRemovedChangePrivate, subjectId)
{
}

QPropertyValueRemovedChange::~QPropertyValueRemovedChange()
{
}

/*!
 * Sets the value removed from the property to \a value.
 */
void QPropertyValueRemovedChange::setRemovedValue(const QVariant &value)
{
    Q_D(QPropertyValueRemovedChange);
    d->m_RemovedValue = value;
}

/*!
 * \return the value removed from the property.
 */
QVariant QPropertyValueRemovedChange::removedValue() const
{
    Q_D(const QPropertyValueRemovedChange);
    return d->m_RemovedValue;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
