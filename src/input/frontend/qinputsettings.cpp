/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qinputsettings.h"
#include "qinputsettings_p.h"
#include <Qt3DCore/qnodecreatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \class Qt3DInput::QInputSettings
    \inmodule Qt3DInput
    \inherits Qt3DCore::QComponant
    \brief QInputSettings stores settings for the specified eventSource.

    \since 5.7
*/

/*!
    \qmltype InputSettings
    \inqmlmodule Qt3D.Input
    \inherits Component3D
    \instantiates Qt3DInput::QInputSettings
    \brief QML frontend for the Qt3DInput::QInputSettings C++ class.

    Stores settings the specified eventSource
    \since 5.7
*/

QInputSettingsPrivate::QInputSettingsPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_eventSource(nullptr)
{
}

/*!
    Constructs a new QInputSettings instance with \a parent.
 */
QInputSettings::QInputSettings(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QInputSettingsPrivate(), parent)
{
}

/*! \internal */
QInputSettings::~QInputSettings()
{
}

/*!
    \property QInputSettings::eventSource

    Holds the source for the QInputSettings.
 */
/*!
  \fn QInputSettings::eventSourceChanged(QObject *)

  This signal is emitted when the even source associated with the input setting is changed.
*/

/*!
  \qmlproperty QObject Qt3D.Input::InputSettings::eventSource

  The current event source of the InputSettings object
*/

/*!
    \qmlsignal Qt3D.Input::InputSettings::ventSourceChanged(QObject *)

    This signal is emitted when the even source associated with the input setting is changed.

    The corresponding handler is \c onEventSourceChanged
*/

/*!
 * \brief QInputSettings::eventSource
 * \return the current event source of the QInputSettings instance.
 */
QObject *QInputSettings::eventSource() const
{
    Q_D(const QInputSettings);
    return d->m_eventSource;
}

/*!
   Set the current event source of the QInputSettings instance to \a eventSource.
 */
void QInputSettings::setEventSource(QObject *eventSource)
{
    Q_D(QInputSettings);
    if (d->m_eventSource != eventSource) {
        d->m_eventSource = eventSource;
        emit eventSourceChanged(eventSource);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QInputSettings::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QInputSettingsData>::create(this);
    auto &data = creationChange->data;

    Q_D(const QInputSettings);
    data.eventSource = d->m_eventSource;

    return creationChange;
}

} // Qt3DInput


QT_END_NAMESPACE
