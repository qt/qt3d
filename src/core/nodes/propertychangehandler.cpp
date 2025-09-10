// Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Milian Wolff <milian.wolff@kdab.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#include "moc_propertychangehandler_p.cpp"
