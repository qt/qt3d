// Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Milian Wolff <milian.wolff@kdab.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QMetaMethod>
#include <QtCore/QObject>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORESHARED_EXPORT PropertyChangeHandlerBase : public QObject
{
    Q_OBJECT
public:
    PropertyChangeHandlerBase(QObject *parent = nullptr);

    /**
     * Connect to the change signal of @p property in @p object.
     */
    void connectToPropertyChange(const QObject *object, int propertyIndex);

    /**
     * Disconnect from the change signal of @p property in @p object.
     */
    void disconnectFromPropertyChange(const QObject *object, int propertyIndex);
};

/**
 * The property change handler is similar to QSignalSpy, but geared towards the usecase of Qt3D.
 *
 * It allows connecting to any number of property change signals of the receiver object and forwards
 * the signal invocations to the Receiver by calling its propertyChanged function.
 */
template<class Receiver>
class PropertyChangeHandler : public PropertyChangeHandlerBase
{
public:
    PropertyChangeHandler(Receiver *receiver, QObject *parent = nullptr);

    /**
     * @internal
     *
     * Custom implementation of qt_metacall which calls propertyChanged() on the receiver for
     * connected signals.
     */
    int qt_metacall(QMetaObject::Call call, int methodId, void **args) override;

private:
    Receiver *m_receiver;
};

template<class Receiver>
PropertyChangeHandler<Receiver>::PropertyChangeHandler(Receiver *receiver, QObject *parent)
    : PropertyChangeHandlerBase(parent)
    , m_receiver(receiver)
{
}

template<class Receiver>
int PropertyChangeHandler<Receiver>::qt_metacall(QMetaObject::Call call, int methodId, void **args)
{
    methodId = QObject::qt_metacall(call, methodId, args);
    if (methodId < 0)
        return methodId;

    if (call == QMetaObject::InvokeMetaMethod) {
        m_receiver->propertyChanged(methodId);
        return -1;
    }
    return methodId;
}

}

QT_END_NAMESPACE

#endif // SIGNALHANDLER_H
