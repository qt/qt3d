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

#include "qstaticpropertyvalueaddedchangebase.h"

#include "qstaticpropertyvalueaddedchangebase_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QStaticPropertyValueAddedChangeBasePrivate::QStaticPropertyValueAddedChangeBasePrivate()
    : QPropertyValueAddedChangeBasePrivate(),
      m_propertyName(nullptr)
{
}

/*!
 * \class Qt3DCore::QStaticPropertyValueAddedChangeBase
 * \inheaderfile Qt3DCore/QStaticPropertyValueAddedChangeBase
 * \inmodule Qt3DCore
 * \since 5.7
 * \brief The QStaticPropertyValueAddedChangeBase class is the base class for all static PropertyValueAdded QSceneChange events.
 *
 */

/*!
 * Constructs a new QStaticPropertyValueAddedChangeBase with \a subjectId
 */
QStaticPropertyValueAddedChangeBase::QStaticPropertyValueAddedChangeBase(QNodeId subjectId)
    : QPropertyValueAddedChangeBase(*new QStaticPropertyValueAddedChangeBasePrivate, subjectId)
{
}

QStaticPropertyValueAddedChangeBase::QStaticPropertyValueAddedChangeBase(QStaticPropertyValueAddedChangeBasePrivate &dd,
                                                                         QNodeId subjectId)
    : QPropertyValueAddedChangeBase(dd, subjectId)
{
}

QStaticPropertyValueAddedChangeBase::~QStaticPropertyValueAddedChangeBase()
{
}

/*!
 * \return name of the property.
 */
const char *QStaticPropertyValueAddedChangeBase::propertyName() const
{
    Q_D(const QStaticPropertyValueAddedChangeBase);
    return d->m_propertyName;
}

/*!
 * Sets the property change \a name.
 */
void QStaticPropertyValueAddedChangeBase::setPropertyName(const char *name)
{
    Q_D(QStaticPropertyValueAddedChangeBase);
    d->m_propertyName = name;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
