/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qaxisactionhandler.h"
#include "qaxisactionhandler_p.h"

#include <Qt3DInput/qlogicaldevice.h>
#include <Qt3DInput/private/axisactionpayload_p.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*! \internal */
QAxisActionHandlerPrivate::QAxisActionHandlerPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_logicalDevice(Q_NULLPTR)
{
}

/*!
    \class Qt3DInput::QAxisActionHandler
    \inmodule Qt3DInput
    \inherits Qt3DCore::QComponent
    \brief QAxisActionHandler is the main link between Qt3d Actions and the rest of the program.

    It is responsible for linking to the actions on the connected logical device and performing
    some meaninful work within the application.

    \since 5.7
*/

/*!
    \qmltype AxisActionHandler
    \inqmlmodule Qt3D.Input
    \instantiates Qt3DInput::QAxisActionHandler
    \brief QML frontend for the Qt3DInput::QAxisActionHandler C++ class.

    QAxisActionHandler is the main link between Qt3d Actions and the rest of the program.

    It is responsible for linking to the actions on the connected logical device and performing
    some meaninful work within the application.

    \qml
    AxisActionHandler {
        id: axisActionHandler
        logicalDevice: keyboardLogicalDevice

        onActionStarted: {
            if (name === "fire")
                console.log("Firing")
        }

        onActionFinished: {
            if (name === "fire")
                console.log("Stopped Firing")
        }

    }
    \endqml
    \since 5.7
*/

/*!
    Constructs a new QAxisActionHandler instance with parent \a parent.
 */
QAxisActionHandler::QAxisActionHandler(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QAxisActionHandlerPrivate, parent)
{
}

QAxisActionHandler::QAxisActionHandler(QAxisActionHandlerPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{

}

/*!
    Deletes the QAxisActionHandler instance.
 */
QAxisActionHandler::~QAxisActionHandler()
{
    QNode::cleanup();
}

/*!
  \fn QAxisActionHandler::logicalDeviceChanged()

  This signal is emitted when the logical device ascociated with the handeler is changed.
*/

/*!
  \qmlproperty LogicalDevice Qt3D.Input::AxisActionHandler::logicalDevice

  The current logical device of the assocciated with this handeler
*/

/*!
    \qmlsignal Qt3D.Input::AxisActionHandler::logicalDeviceChanged()

    This signal is emitted when the logical device ascociated with the handeler is changed.

    The corresponding handeler is \c onLogicalDeviceChanged
*/
/*!
    Return the logical device whos actions and actions this object handles.
 */
Qt3DInput::QLogicalDevice *QAxisActionHandler::logicalDevice() const
{
    Q_D(const QAxisActionHandler);
    return d->m_logicalDevice;
}

/*!
    Set the logical device whos actions and actions this object handles.
 */
void QAxisActionHandler::setLogicalDevice(Qt3DInput::QLogicalDevice *logicalDevice)
{
    Q_D(QAxisActionHandler);
    if (d->m_logicalDevice == logicalDevice)
        return;

    // Need to set the parent of the logical device if it has none
    // so as to trigger the backend node created with a NodeCreated notification
    if (logicalDevice && !logicalDevice->parent())
        logicalDevice->setParent(this);

    d->m_logicalDevice = logicalDevice;
    emit logicalDeviceChanged(logicalDevice);
}

void QAxisActionHandler::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QAxisActionHandler *component = static_cast<const QAxisActionHandler *>(ref);
    d_func()->m_logicalDevice = qobject_cast<QLogicalDevice *>(QNode::clone(component->d_func()->m_logicalDevice));
}

void QAxisActionHandler::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Qt3DCore::QBackendScenePropertyChangePtr e = qSharedPointerCast<Qt3DCore::QBackendScenePropertyChange>(change);
    if (e->type() == Qt3DCore::NodeUpdated) {
        if (e->propertyName() == QByteArrayLiteral("axisPayload")) {
            Qt3DInput::Input::AxisPayload payload = e->value().value<Qt3DInput::Input::AxisPayload>();
            Q_FOREACH (const Qt3DInput::Input::AxisUpdate &axisUpdate, payload.axes)
                axisValueChanged(axisUpdate.name, axisUpdate.value);

        } else  if (e->propertyName() == QByteArrayLiteral("actionPayload")) {
            Qt3DInput::Input::ActionPayload payload = e->value().value<Qt3DInput::Input::ActionPayload>();
            Q_FOREACH (const Qt3DInput::Input::ActionUpdate &actionUpdate, payload.actions) {
                if (actionUpdate.triggered)
                    actionStarted(actionUpdate.name);
                else
                    actionFinished(actionUpdate.name);
            }
        }
    }
}

QT_END_NAMESPACE

} // namespace Qt3DInput

