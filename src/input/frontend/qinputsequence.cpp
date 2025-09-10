// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#include "qinputsequence.h"
#include "qinputsequence_p.h"

#include <Qt3DInput/qabstractactioninput.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QInputSequencePrivate::QInputSequencePrivate()
    : QAbstractActionInputPrivate(),
      m_timeout(0),
      m_buttonInterval(0)
{
}

/*!
    \class Qt3DInput::QInputSequence
    \inmodule Qt3DInput
    \inherits QAbstractAggregateActionInput
    \brief QInputSequence represents a set of QAbstractActionInput's that must be triggerd one after the other.
    \since 5.7
*/

/*!
    \qmltype InputSequence
    \inqmlmodule Qt3D.Input
    \nativetype Qt3DInput::QInputSequence
    \brief QML frontend for the Qt3DInput::QInputSequence C++ class.

    Represents a set of QAbstractActionInput's that must be triggerd one after the other.

    The following example shows a chord that will be triggered by pressing the A and S keys together with a tolerance of 10 miliseconds between presses.
    \qml
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
    \endqml

    \since 5.7
*/

/*!
    Constructs a new QInputSequence with parent \a parent.
 */
QInputSequence::QInputSequence(Qt3DCore::QNode *parent)
    : Qt3DInput::QAbstractActionInput(*new QInputSequencePrivate(), parent)
{

}

/*! \internal */
QInputSequence::~QInputSequence()
{
}

/*!
    \qmlproperty list<AbstractActionInput> Qt3D.Input::InputSequence::sequences
*/


/*!
    \qmlproperty int Qt3D.Input::InputSequence::timeout

    The time in milliseconds in which all QAbstractActionInput's in the input sequence must triggered within.
*/

/*!
    \qmlsignal Qt3D.Input::InputSequence::timeoutChanged()

    This signal is emitted when the timeout of the input sequence is changed.

    The corresponding handler is \c onTimeoutChanged
*/

/*!
    Returns the time in which all QAbstractActionInput's in the input sequence must triggered within.
    The time is in milliseconds
 */
int QInputSequence::timeout() const
{
    Q_D(const QInputSequence);
    return d->m_timeout;
}

/*!
  \qmlproperty int Qt3D.Input::InputSequence::buttonInterval

  The maximum time in milliseconds in between consecutive QAbstractActionInput's in the input sequence.
*/

/*!
    \qmlsignal Qt3D.Input::InputSequence::buttonIntervalChanged()

    This signal is emitted when the buttonInterval of the input sequence is changed.

    The corresponding handler is \c onButtonIntervalChanged
*/

/*!
    Returns the maximum time in between consecutive QAbstractActionInput's in the input sequence.
    The time is in milliseconds
 */
int QInputSequence::buttonInterval() const
{
    Q_D(const QInputSequence);
    return d->m_buttonInterval;
}

/*!
    \property Qt3DInput::QInputSequence::timeout

    The time in which all QAbstractActionInput's in the input sequence must triggered within.
    The time is in milliseconds.
 */
void QInputSequence::setTimeout(int timeout)
{
    Q_D(QInputSequence);
    if (d->m_timeout != timeout) {
        d->m_timeout = timeout;
        emit timeoutChanged(timeout);
    }
}

/*!
    \property Qt3DInput::QInputSequence::buttonInterval

    The maximum time in between consecutive QAbstractActionInput's in the input sequence.
    The time is in milliseconds.
 */
void QInputSequence::setButtonInterval(int buttonInterval)
{
    Q_D(QInputSequence);
    if (d->m_buttonInterval != buttonInterval) {
        d->m_buttonInterval = buttonInterval;
        emit buttonIntervalChanged(buttonInterval);
    }
}

/*!
    Append the QAbstractActionInput \a input to the end of this QInputSequence's sequence vector.

    \sa removeSequence
 */
void QInputSequence::addSequence(QAbstractActionInput *input)
{
    Q_D(QInputSequence);
    if (!d->m_sequences.contains(input)) {
        d->m_sequences.push_back(input);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(input, &QInputSequence::removeSequence, d->m_sequences);

        if (!input->parent())
            input->setParent(this);

        d->update();
    }
}

/*!
    Remove the QAbstractActionInput \a input from this QInputSequence's sequence vector.

    \sa addSequence
 */
void QInputSequence::removeSequence(QAbstractActionInput *input)
{
    Q_D(QInputSequence);
    if (d->m_sequences.contains(input)) {
        d->update();

        d->m_sequences.removeOne(input);

        // Remove bookkeeping connection
        d->unregisterDestructionHelper(input);
    }
}

/*!
    Returns the QInputSequence's sequence vector.
 */
QList<QAbstractActionInput *> QInputSequence::sequences() const
{
    Q_D(const QInputSequence);
    return d->m_sequences;
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qinputsequence.cpp"
