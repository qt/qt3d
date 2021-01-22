/****************************************************************************
**
** Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Milian Wolff <milian.wolff@kdab.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebChannel module of the Qt Toolkit.
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

#include "propertychangehandler_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*
    \internal
    \class Qt3DCore::PropertyChangeHandlerBase
    \inmodule Qt3DCore
    \brief Base class for the property change handler.
*/
PropertyChangeHandlerBase::PropertyChangeHandlerBase(QObject *parent) :
    QObject(parent)
{
}

void PropertyChangeHandlerBase::connectToPropertyChange(const QObject *object, int propertyIndex)
{
    const QMetaObject *metaObject = object->metaObject();
    const QMetaProperty property = metaObject->property(propertyIndex);
    if (!property.hasNotifySignal())
        return;

    static const int memberOffset = staticMetaObject.methodCount();
    QMetaObject::Connection connection = QMetaObject::connect(object, property.notifySignalIndex(),
                                                              this, memberOffset + propertyIndex,
                                                              Qt::DirectConnection, 0);
    Q_ASSERT(connection);
    Q_UNUSED(connection);
}

void PropertyChangeHandlerBase::disconnectFromPropertyChange(const QObject *object, int propertyIndex)
{
    const QMetaObject *metaObject = object->metaObject();
    const QMetaProperty property = metaObject->property(propertyIndex);
    if (!property.hasNotifySignal())
        return;

    static const int memberOffset = staticMetaObject.methodCount();
    QMetaObject::disconnect(object, property.notifySignalIndex(), this, memberOffset + propertyIndex);
}


}

QT_END_NAMESPACE
