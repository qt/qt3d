// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#include "qactioninput.h"
#include "qactioninput_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qabstractactioninput.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QActionInputPrivate::QActionInputPrivate()
    : QAbstractActionInputPrivate(),
      m_sourceDevice(nullptr)
{
}

/*!
    \class Qt3DInput::QActionInput
    \inmodule Qt3DInput
    \inherits Qt3DInput::QAbstractActionInput
    \brief QActionInput stores Device and Buttons used to trigger an input event.

    \since 5.7
*/

/*!
    \qmltype ActionInput
    \inqmlmodule Qt3D.Input
    \inherits AbstractActionInput
    \nativetype Qt3DInput::QActionInput
    \brief QML frontend for the Qt3DInput::QActionInput C++ class.

    Links a physical device and selected buttons on it which can trigger this action.

    Each Action input can be triggered by one or many buttons on a source device
    \qml
    ActionInput {
      sourceDevice: keyboardSourceDevice
      buttons: [Qt.Key_A]
    }
    \endqml
   \qml
    ActionInput {
      sourceDevice: keyboardSourceDevice
      buttons: [Qt.Key_A,Qt.Key_B]
    }
    \endqml
    \since 5.7
*/

/*!
    Constructs a new QActionInput instance with \a parent.
 */
QActionInput::QActionInput(Qt3DCore::QNode *parent)
    : Qt3DInput::QAbstractActionInput(*new QActionInputPrivate(), parent)
{

}

/*! \internal */
QActionInput::~QActionInput()
{
}

/*!
    \property Qt3DInput::QActionInput::buttons

    Holds the buttons that can trigger this Action.
 */
QList<int> QActionInput::buttons() const
{
    Q_D(const QActionInput);
    return d->m_buttons;
}

/*!
    \property Qt3DInput::QActionInput::sourceDevice

    The current source device of the QActionInput.
*/
/*!
  \fn Qt3DInput::QActionInput::sourceDeviceChanged(QAbstractPhysicalDevice *sourceDevice)

  This signal is emitted when the source device associated with the action input is changed to \a sourceDevice.
*/

/*!
  \qmlproperty AbstractPhysicalDevice Qt3D.Input::ActionInput::sourceDevice

  The current source device of the ActionInput
*/

/*!
    \qmlsignal Qt3D.Input::ActionInput::sourceDeviceChanged()

    This signal is emitted when the source device associated with the action input is changed.

    The corresponding handler is \c onSourceDeviceChanged
*/
void QActionInput::setSourceDevice(QAbstractPhysicalDevice *sourceDevice)
{
    Q_D(QActionInput);
    if (d->m_sourceDevice != sourceDevice) {

        if (d->m_sourceDevice)
            d->unregisterDestructionHelper(d->m_sourceDevice);

        // Check and set parent if needed
        // to force creation in the backend
        if (sourceDevice && !sourceDevice->parent())
            sourceDevice->setParent(this);

        d->m_sourceDevice = sourceDevice;

        // Ensures proper bookkeeping
        if (d->m_sourceDevice)
            d->registerDestructionHelper(sourceDevice, &QActionInput::setSourceDevice, d->m_sourceDevice);

        emit sourceDeviceChanged(sourceDevice);
    }
}

QAbstractPhysicalDevice *QActionInput::sourceDevice() const
{
    Q_D(const QActionInput);
    return d->m_sourceDevice;
}

/*!
  \fn Qt3DInput::QActionInput::buttonsChanged(const QList<int> &buttons)

  This signal is emitted when the buttons associated with the action input is changed.
  The buttons changed are \a buttons
*/

/*!
    \qmlproperty list<int> Qt3D.Input::ActionInput::buttons

    The Buttons that can trigger this Action.
*/

/*!
    \qmlsignal Qt3D.Input::ActionInput::buttonsChanged(const QList<int> &buttons)

    This signal is emitted when the \a buttons associated with the action input is changed.

    The corresponding handler is \c onbuttonsChanged
*/

/*!
    Set the buttons to trigger the QActionInput instance to \a buttons.
 */
void QActionInput::setButtons(const QList<int> &buttons)
{
    Q_D(QActionInput);
    if (buttons != d->m_buttons) {
        d->m_buttons = buttons;
        emit buttonsChanged(buttons);
    }
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qactioninput.cpp"
