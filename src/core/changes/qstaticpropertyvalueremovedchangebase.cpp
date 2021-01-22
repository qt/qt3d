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

#include "qstaticpropertyvalueremovedchangebase.h"
#include "qstaticpropertyvalueremovedchangebase_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QStaticPropertyValueRemovedChangeBasePrivate::QStaticPropertyValueRemovedChangeBasePrivate()
    : QPropertyValueRemovedChangeBasePrivate(),
      m_propertyName(nullptr)
{
}

/*!
 * \class Qt3DCore::QStaticPropertyValueRemovedChangeBase
 * \inheaderfile Qt3DCore/QStaticPropertyValueRemovedChangeBase
 * \inmodule Qt3DCore
 * \since 5.7
 * \brief The QStaticPropertyValueRemovedChangeBase class is the base class for all static PropertyValueRemoved QSceneChange events.
 *
 */

/*!
 * Constructs a new QStaticPropertyValueRemovedChangeBase with \a subjectId
 */
QStaticPropertyValueRemovedChangeBase::QStaticPropertyValueRemovedChangeBase(QNodeId subjectId)
    : QPropertyValueRemovedChangeBase(*new QStaticPropertyValueRemovedChangeBasePrivate, subjectId)
{
}

QStaticPropertyValueRemovedChangeBase::QStaticPropertyValueRemovedChangeBase(QStaticPropertyValueRemovedChangeBasePrivate &dd,
                                                                         QNodeId subjectId)
    : QPropertyValueRemovedChangeBase(dd, subjectId)
{
}

QStaticPropertyValueRemovedChangeBase::~QStaticPropertyValueRemovedChangeBase()
{
}

/*!
 * \return name of the property.
 */
const char *QStaticPropertyValueRemovedChangeBase::propertyName() const
{
    Q_D(const QStaticPropertyValueRemovedChangeBase);
    return d->m_propertyName;
}

/*!
 * Sets the property change \a name.
 */
void QStaticPropertyValueRemovedChangeBase::setPropertyName(const char *name)
{
    Q_D(QStaticPropertyValueRemovedChangeBase);
    d->m_propertyName = name;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
