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

#include "qpropertyvalueremovedchangebase.h"

#include "qpropertyvalueremovedchangebase_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QPropertyValueRemovedChangeBasePrivate::QPropertyValueRemovedChangeBasePrivate()
    : QSceneChangePrivate()
{
}

QPropertyValueRemovedChangeBasePrivate::~QPropertyValueRemovedChangeBasePrivate()
{
}

/*!
 * \class Qt3DCore::QPropertyValueRemovedChangeBase
 * \inheaderfile Qt3DCore/QPropertyValueRemovedChangeBase
 * \inmodule Qt3DCore
 * \brief The QPropertyValueRemovedChangeBase class is the base class for all PropertyValueRemoved QSceneChange events.
 *
 * The QPropertyValueRemovedChangeBase class is the base class for all QSceneChange events that
 * have the changeType() PropertyValueRemoved. You should not need to instantiate this class.
 * Usually you should be using one of its subclasses such as QPropertyNodeRemovedChange.
 *
 * You can subclass this to create your own node Removed types for communication between
 * your QNode and QBackendNode subclasses when writing your own aspects.
 */

/*!
 * \typedef Qt3DCore::QPropertyValueRemovedChangeBasePtr
 * \relates Qt3DCore::QPropertyValueRemovedChangeBase
 *
 * A shared pointer for QPropertyValueRemovedChangeBase.
 */

/*!
 * Constructs a new QPropertyValueRemovedChangeBase with \a subjectId
 */
QPropertyValueRemovedChangeBase::QPropertyValueRemovedChangeBase(QNodeId subjectId)
    : QSceneChange(*new QPropertyValueRemovedChangeBasePrivate, PropertyValueRemoved, subjectId)
{
}

/*! \internal */
QPropertyValueRemovedChangeBase::QPropertyValueRemovedChangeBase(QPropertyValueRemovedChangeBasePrivate &dd,
                                                                 QNodeId subjectId)
    : QSceneChange(dd, PropertyValueRemoved, subjectId)
{
}

QPropertyValueRemovedChangeBase::~QPropertyValueRemovedChangeBase()
{
}

} // namespace Qt3DCore

QT_END_NAMESPACE
