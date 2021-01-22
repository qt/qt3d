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

#include "qpropertyupdatedchangebase.h"
#include "qpropertyupdatedchangebase_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QPropertyUpdatedChangeBasePrivate::QPropertyUpdatedChangeBasePrivate()
    : QSceneChangePrivate()
    , m_isIntermediate(false)
{
}

QPropertyUpdatedChangeBasePrivate::~QPropertyUpdatedChangeBasePrivate()
{
}

QPropertyUpdatedChangeBasePrivate *QPropertyUpdatedChangeBasePrivate::get(QPropertyUpdatedChangeBase *q)
{
    return q->d_func();
}

/*!
 * \class Qt3DCore::QPropertyUpdatedChangeBase
 * \inheaderfile Qt3DCore/QPropertyUpdatedChangeBase
 * \inmodule Qt3DCore
 * \brief The QPropertyUpdatedChangeBase class is the base class for all PropertyUpdated QSceneChange events.
 *
 * The QPropertyUpdatedChangeBase class is the base class for all QSceneChange events that
 * have the changeType() PropertyUpdated. You should not need to instantiate this class.
 * Usually you should be using one of its subclasses such as QPropertyUpdatedChange or
 * QTypedPropertyUpdatedChange.
 *
 * You can subclass this to create your own node update types for communication between
 * your QNode and QBackendNode subclasses when writing your own aspects.
 */

/*!
 * \typedef Qt3DCore::QPropertyUpdatedChangeBasePtr
 * \relates Qt3DCore::QPropertyUpdatedChangeBase
 *
 * A shared pointer for QPropertyUpdatedChangeBase.
 */

/*!
 * Constructs a new QPropertyUpdatedChangeBase with \a subjectId
 */
QPropertyUpdatedChangeBase::QPropertyUpdatedChangeBase(QNodeId subjectId)
    : QSceneChange(*new QPropertyUpdatedChangeBasePrivate, PropertyUpdated, subjectId)
{
}

QPropertyUpdatedChangeBase::QPropertyUpdatedChangeBase(QPropertyUpdatedChangeBasePrivate &dd, QNodeId subjectId)
    : QSceneChange(dd, PropertyUpdated, subjectId)
{
}

QPropertyUpdatedChangeBase::~QPropertyUpdatedChangeBase()
{
}

} // namespace Qt3DCore

QT_END_NAMESPACE
