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

#include "qstaticpropertyupdatedchangebase.h"

#include "qstaticpropertyupdatedchangebase_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QStaticPropertyUpdatedChangeBasePrivate::QStaticPropertyUpdatedChangeBasePrivate()
    : QPropertyUpdatedChangeBasePrivate(),
      m_propertyName(nullptr)
{
}

QStaticPropertyUpdatedChangeBasePrivate::~QStaticPropertyUpdatedChangeBasePrivate()
{
}


/*!
 * \class Qt3DCore::QStaticPropertyUpdatedChangeBase
 * \inheaderfile Qt3DCore/QStaticPropertyUpdatedChangeBase
 * \inmodule Qt3DCore
 * \since 5.7
 * \brief The QStaticPropertyUpdatedChangeBase class is the base class for all static PropertyUpdated QSceneChange events.
 *
 */

/*!
 * \typedef Qt3DCore::QStaticPropertyUpdatedChangeBasePtr
 * \relates Qt3DCore::QStaticPropertyUpdatedChangeBase
 *
 * A shared pointer for QStaticPropertyUpdatedChangeBase.
 */

/*!
 * Constructs a new QStaticPropertyUpdatedChangeBase with \a subjectId
 */
QStaticPropertyUpdatedChangeBase::QStaticPropertyUpdatedChangeBase(QNodeId subjectId)
    : QPropertyUpdatedChangeBase(*new QStaticPropertyUpdatedChangeBasePrivate, subjectId)
{
}

/*! \internal */
QStaticPropertyUpdatedChangeBase::QStaticPropertyUpdatedChangeBase(QStaticPropertyUpdatedChangeBasePrivate &dd,
                                                 QNodeId subjectId)
    : QPropertyUpdatedChangeBase(dd, subjectId)
{
}

QStaticPropertyUpdatedChangeBase::~QStaticPropertyUpdatedChangeBase()
{
}

/*!
 * \return name of the property.
 */
const char *QStaticPropertyUpdatedChangeBase::propertyName() const
{
    Q_D(const QStaticPropertyUpdatedChangeBase);
    return d->m_propertyName;
}

/*!
 * Sets the property change \a name.
 */
void QStaticPropertyUpdatedChangeBase::setPropertyName(const char *name)
{
    Q_D(QStaticPropertyUpdatedChangeBase);
    d->m_propertyName = name;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
