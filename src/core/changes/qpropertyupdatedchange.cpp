/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qpropertyupdatedchange.h"
#include "qpropertyupdatedchange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QPropertyUpdatedChangePrivate::QPropertyUpdatedChangePrivate()
    : QStaticPropertyUpdatedChangeBasePrivate()
{
}

QPropertyUpdatedChangePrivate::~QPropertyUpdatedChangePrivate()
{
}

/*!
 * \class Qt3DCore::QPropertyUpdatedChange
 * \inheaderfile Qt3DCore/QPropertyUpdatedChange
 * \inherits Qt3DCore::QStaticPropertyUpdatedChangeBase
 * \inmodule Qt3DCore
 * \brief The QPropertyUpdatedChange class is used to notify when a property value is updated.
 *
 */

/*!
 * \typedef Qt3DCore::QPropertyUpdatedChangePtr
 * \relates Qt3DCore::QPropertyUpdatedChange
 *
 * A shared pointer for QPropertyUpdatedChange.
 */

/*!
 * Constructs a new QPropertyUpdatedChange with \a subjectId.
 */
QPropertyUpdatedChange::QPropertyUpdatedChange(QNodeId subjectId)
    : QStaticPropertyUpdatedChangeBase(*new QPropertyUpdatedChangePrivate, subjectId)
{
}

/*!
 * \internal
 */
QPropertyUpdatedChange::QPropertyUpdatedChange(QPropertyUpdatedChangePrivate &dd,
                                         QNodeId subjectId)
    : QStaticPropertyUpdatedChangeBase(dd, subjectId)
{
}

QPropertyUpdatedChange::~QPropertyUpdatedChange()
{
}

/*!
 * \return property value.
 */
QVariant QPropertyUpdatedChange::value() const
{
    Q_D(const QPropertyUpdatedChange);
    return d->m_value;
}

/*!
 * Set the property change \a value.
 */
void QPropertyUpdatedChange::setValue(const QVariant &value)
{
    Q_D(QPropertyUpdatedChange);
    d->m_value = value;
}

} // Qt3D

QT_END_NAMESPACE
