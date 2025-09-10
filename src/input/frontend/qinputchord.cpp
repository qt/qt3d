// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#include "qinputchord.h"
#include "qinputchord_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \class Qt3DInput::QInputChord
    \inmodule Qt3DInput
    \inherits QAbstractActionInput
    \brief QInputChord represents a set of QAbstractActionInput's that must be triggerd at once.

    \since 5.7
*/

/*!
    \qmltype InputChord
    \inqmlmodule Qt3D.Input
    \inherits AbstractActionInput
    \nativetype Qt3DInput::QInputChord
    \brief QML frontend for the Qt3DInput::QInputChord C++ class.

    Represents a set of QAbstractActionInput's that must be triggerd at once.

    The following example shows an sequence that will be triggered by pressing the G, D, and J keys in that order with a maximum time between key presses of 1 second  and overall maximum input time of 3 seconds.
    \qml
    InputChord {
        interval: 1000
        timeout: 3000
        chords: [
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
    \endqml
    \since 5.7
*/

/*!
  \qmlproperty list<AbstractActionInput> Qt3D.Input::InputChord::chords

  The list of AbstractActionInput that must be triggered to trigger this aggregate input.
*/

/*!
  \qmlproperty int Qt3D.Input::InputChord::timeout
*/

/*!
    Constructs a new QInputChord with parent \a parent.
 */
QInputChord::QInputChord(Qt3DCore::QNode *parent)
    : Qt3DInput::QAbstractActionInput(*new QInputChordPrivate(), parent)
{

}

/*! \internal */
QInputChord::~QInputChord()
{
}

/*!
    \property Qt3DInput::QInputChord::timeout

    The time in which all QAbstractActionInput's in the input chord must triggered within.
    The time is in milliseconds.
 */
int QInputChord::timeout() const
{
    Q_D(const QInputChord);
    return d->m_timeout;
}

/*!
    Sets the time in which all QAbstractActionInput's in the input chord must triggered within to \a timeout.
    The time is in milliseconds
 */
void QInputChord::setTimeout(int timeout)
{
    Q_D(QInputChord);
    if (d->m_timeout != timeout) {
        d->m_timeout = timeout;
        emit timeoutChanged(timeout);
    }
}

/*!
    Append the QAbstractActionInput \a input to the end of this QInputChord's chord vector.

    \sa removeChord
 */
void QInputChord::addChord(QAbstractActionInput *input)
{
    Q_D(QInputChord);
    if (!d->m_chords.contains(input)) {
        d->m_chords.push_back(input);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(input, &QInputChord::removeChord, d->m_chords);

        if (!input->parent())
            input->setParent(this);

        d->update();
    }
}

/*!
    Remove the QAbstractActionInput \a input from this QInputChord's chord vector.

    \sa addChord
 */
void QInputChord::removeChord(QAbstractActionInput *input)
{
    Q_D(QInputChord);
    if (d->m_chords.contains(input)) {
        d->update();

        d->m_chords.removeOne(input);

        // Remove bookkeeping connection
        d->unregisterDestructionHelper(input);
    }
}

/*!
    Returns the QInputChord's chord vector.
 */
QList<QAbstractActionInput *> QInputChord::chords() const
{
    Q_D(const QInputChord);
    return d->m_chords;
}

QInputChordPrivate::QInputChordPrivate()
    : QAbstractActionInputPrivate(),
      m_timeout(0)
{
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qinputchord.cpp"
