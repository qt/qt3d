// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qlogicaldevice.h"
#include "qlogicaldevice_p.h"

#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxis.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QLogicalDevicePrivate::QLogicalDevicePrivate()
    : Qt3DCore::QComponentPrivate()
{
}

QLogicalDevicePrivate::~QLogicalDevicePrivate()
{
}

/*!
    \class Qt3DInput::QLogicalDevice
    \inmodule Qt3DInput
    \inherits Qt3DCore::QNode
    \brief QLogicalDevice allows the user to define a set of actions that they wish to use within an application.

    \since 5.6
*/

/*!
    \qmltype LogicalDevice
    \inqmlmodule Qt3D.Input
    \nativetype Qt3DInput::QLogicalDevice
    \brief QML frontend for the Qt3DInput::QLogicalDevice C++ class.

    Allows the user to define a set of actions that they wish to use within an application.

    \qml
    LogicalDevice {
        id: keyboardLogicalDevice

        actions: [
            Action {
                name: "fire"
                inputs: [
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_Space]
                    },
                    InputChord {
                        tolerance: 10
                        inputs: [
                            ActionInput {
                                sourceDevice: keyboardSourceDevice
                                keys: [Qt.Key_A]
                            },
                            ActionInput {
                                sourceDevice: keyboardSourceDevice
                                keys: [Qt.Key_S]
                            }
                        ]
                    }
                ]
            },
            Action {
                name: "reload"
                inputs: [
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        keys: [Qt.Key_Alt]
                    }
                ]
            },
            Action {
                name: "combo"
                inputs: [
                    InputSequence {
                        interval: 1000
                        timeout: 10000
                        inputs: [
                            ActionInput {
                                sourceDevice: keyboardSourceDevice
                                keys: [Qt.Key_G]
                            },
                            ActionInput {
                                sourceDevice: keyboardSourceDevice
                                keys: [Qt.Key_D]
                            },
                            ActionInput {
                                sourceDevice: keyboardSourceDevice
                                keys: [Qt.Key_J]
                            }
                        ]
                    }
                ]
            }
        ]
    }
    \endqml

    \since 5.6
*/

/*!
    Constructs a new QLogicalDevice instance with parent \a parent.
 */
QLogicalDevice::QLogicalDevice(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QLogicalDevicePrivate(), parent)
{
}

QLogicalDevice::~QLogicalDevice()
{
}

/*!
  \qmlproperty list<Action> Qt3D.Input::LogicalDevice::actions

  The actions used by this Logical Device
*/

/*!
    Add an \a action to the list of actions.
 */
void QLogicalDevice::addAction(QAction *action)
{
    Q_D(QLogicalDevice);
    if (!d->m_actions.contains(action)) {
        d->m_actions.push_back(action);
        // Force creation in backend by setting parent
        if (!action->parent())
            action->setParent(this);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(action, &QLogicalDevice::removeAction, d->m_actions);

        d->update();
    }
}

/*!
    Remove an \a action from the list of actions.
 */
void QLogicalDevice::removeAction(QAction *action)
{
    Q_D(QLogicalDevice);
    if (d->m_actions.contains(action)) {
        d->update();

        d->m_actions.removeOne(action);

        // Remove bookkeeping connection
        d->unregisterDestructionHelper(action);
    }
}

/*!
    Returns the list of actions.
 */
QList<QAction *> QLogicalDevice::actions() const
{
    Q_D(const QLogicalDevice);
    return d->m_actions;
}

/*!
  \qmlproperty list<Axis> Qt3D.Input::LogicalDevice::axis

  The axis used by this Logical Device
*/

/*!
    Add an \a axis to the list of axis.
 */
void QLogicalDevice::addAxis(QAxis *axis)
{
    Q_D(QLogicalDevice);
    if (!d->m_axes.contains(axis)) {
        d->m_axes.push_back(axis);

        // Force creation in backend by setting parent
        if (!axis->parent())
            axis->setParent(this);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(axis, &QLogicalDevice::removeAxis, d->m_axes);

        d->update();
    }
}

/*!
    Remove an \a axis drom the list of axis.
 */
void QLogicalDevice::removeAxis(QAxis *axis)
{
    Q_D(QLogicalDevice);
    if (d->m_axes.contains(axis)) {
        d->update();

        d->m_axes.removeOne(axis);

        // Remove bookkeeping connection
        d->unregisterDestructionHelper(axis);
    }
}

/*!
    Returns the list of axis.
 */
QList<QAxis *> QLogicalDevice::axes() const
{
    Q_D(const QLogicalDevice);
    return d->m_axes;
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qlogicaldevice.cpp"
