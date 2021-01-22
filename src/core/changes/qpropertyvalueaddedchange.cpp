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

#include "qpropertyvalueaddedchange.h"
#include "qpropertyvalueaddedchange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QPropertyValueAddedChangePrivate::QPropertyValueAddedChangePrivate()
    : QStaticPropertyValueAddedChangeBasePrivate()
{
}


/*!
 * \class Qt3DCore::QPropertyValueAddedChange
 * \inheaderfile Qt3DCore/QPropertyValueAddedChange
 * \inherits Qt3DCore::QStaticPropertyValueAddedChangeBase
 * \inmodule Qt3DCore
 * \brief Used to notify when a value is added to a property.
 *
 */

/*!
 * \typedef Qt3DCore::QPropertyValueAddedChangePtr
 * \relates Qt3DCore::QPropertyValueAddedChange
 *
 * A shared pointer for QPropertyValueAddedChange.
 */

/*!
 * Constructs a new QPropertyValueAddedChange with \a subjectId.
 */
QPropertyValueAddedChange::QPropertyValueAddedChange(QNodeId subjectId)
    : QStaticPropertyValueAddedChangeBase(*new QPropertyValueAddedChangePrivate, subjectId)
{
}

QPropertyValueAddedChange::~QPropertyValueAddedChange()
{
}

/*!
 * Sets the value added to the property to \a value.
 */
void QPropertyValueAddedChange::setAddedValue(const QVariant &value)
{
    Q_D(QPropertyValueAddedChange);
    d->m_addedValue = value;
}

/*!
 * \return the value added to the property.
 */
QVariant QPropertyValueAddedChange::addedValue() const
{
    Q_D(const QPropertyValueAddedChange);
    return d->m_addedValue;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
