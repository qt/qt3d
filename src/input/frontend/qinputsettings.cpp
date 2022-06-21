// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qinputsettings.h"
#include "qinputsettings_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \class Qt3DInput::QInputSettings
    \inmodule Qt3DInput
    \inherits Qt3DCore::QComponent
    \brief QInputSettings class holds the pointer to an input event source object.
    \since 5.7

    The QInputSettings component must be set as a component of the scene root entity.
    It stores a pointer to the object that acts as the source of input events to be handled
    by various input classes. For example, a QWindow instance can be an event source.

    \sa QMouseDevice, QKeyboardDevice
*/

/*!
    \qmltype InputSettings
    \inqmlmodule Qt3D.Input
    \inherits Component3D
    \instantiates Qt3DInput::QInputSettings
    \brief InputSettings holds the pointer to an input event source object.
    \since 5.7

    The InputSettings component must be set as a component of the scene root entity.
    It stores a pointer to the object that acts as the source of input events to be handled
    by various input classes. For example, a Window instance can be an event source.

    \sa MouseDevice, KeyboardDevice
*/

QInputSettingsPrivate::QInputSettingsPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_eventSource(nullptr)
{
}

QInputSettings::QInputSettings(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QInputSettingsPrivate(), parent)
{
}

QInputSettings::~QInputSettings()
{
}

/*!
    \property QInputSettings::eventSource

    Holds the current event source. An event source is an object that is capable
    of receiving various input events, such as mouse or keyboard events.
    Typically it is a QWindow instance.
 */

/*!
    \qmlproperty QtObject InputSettings::eventSource

    Holds the current event source. An event source is an object that is capable
    of receiving various input events, such as mouse or keyboard events.
    Typically it is a Window instance.
*/
QObject *QInputSettings::eventSource() const
{
    Q_D(const QInputSettings);
    return d->m_eventSource;
}

void QInputSettings::setEventSource(QObject *eventSource)
{
    Q_D(QInputSettings);
    if (d->m_eventSource != eventSource) {
        if (d->m_eventSource)
            QObject::disconnect(d->m_connection);
        d->m_eventSource = eventSource;
        emit eventSourceChanged(eventSource);
        d->m_connection = QObject::connect(eventSource, &QObject::destroyed,
                                           this, &QInputSettings::eventSourceDestroyed);
    }
}

void QInputSettings::eventSourceDestroyed()
{
    Q_D(QInputSettings);
    QObject::disconnect(d->m_connection);
    d->m_eventSource = nullptr;
    emit eventSourceChanged(nullptr);
}

} // Qt3DInput


QT_END_NAMESPACE

#include "moc_qinputsettings.cpp"
