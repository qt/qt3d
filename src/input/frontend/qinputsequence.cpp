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
****************************************************************************/

#include "qinputsequence.h"
#include "qinputsequence_p.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qabstractactioninput.h>
#include <Qt3DCore/qnodepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

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
    \inherits QAbstractAggregateActionInput
    \instantiates Qt3DInput::QInputSequence
    \brief QML frontend for the Qt3DInput::QInputSequence C++ class.

    Represents a set of QAbstractActionInput's that must be triggerd one after the other.

    The following example shows a chord that will be triggered by pressing the A and S keys together with a tolerence of 10 miliseconds between presses.
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

/*!
    Deletes the QInputSequence instance.
 */
QInputSequence::~QInputSequence()
{
    QNode::cleanup();
}

/*!
  \fn QInputSequence::timeoutChanged()

  This signal is emitted when the timeout of the input sequence is changed.
*/

/*!
  \qmlproperty int Qt3D.Input::InputSequence::timeout

  The time in milliseconds in which all QAbstractActionInput's in the input sequence must triggered within.
*/

/*!
    \qmlsignal Qt3D.Input::InputSequence::timeoutChanged()

    This signal is emitted when the timeout of the input sequence is changed.

    The corresponding handeler is \c onTimeoutChanged
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
  \fn QInputSequence::buttonIntervalChanged()

  This signal is emitted when the buttonInterval of the input sequence is changed.
*/

/*!
  \qmlproperty int Qt3D.Input::InputSequence::buttonInterval

  The maximum time in milliseconds in between consecutive QAbstractActionInput's in the input sequence.
*/

/*!
    \qmlsignal Qt3D.Input::InputSequence::buttonIntervalChanged()

    This signal is emitted when the buttonInterval of the input sequence is changed.

    The corresponding handeler is \c onButtonIntervalChanged
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
    Set the time in which all QAbstractActionInput's in the input sequence must triggered within.
    The time is in milliseconds
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
    Set the maximum time in between consecutive QAbstractActionInput's in the input sequence.
    The time is in milliseconds
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

    \sa removeInput
 */
void QInputSequence::addSequence(QAbstractActionInput *input)
{
    Q_D(QInputSequence);
    if (!d->m_sequences.contains(input)) {
        d->m_sequences.push_back(input);

        if (!input->parent())
            input->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            Qt3DCore::QNodePropertyChangePtr change(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeAdded, Qt3DCore::QSceneChange::Node, id()));
            change->setPropertyName("sequence");
            change->setValue(QVariant::fromValue(input->id()));
            d->notifyObservers(change);
        }
    }
}

/*!
    Remove the QAbstractActionInput \a input from this QInputSequence's sequence vector.

    \sa addInput
 */
void QInputSequence::removeSequence(QAbstractActionInput *input)
{
    Q_D(QInputSequence);
    if (d->m_sequences.contains(input)) {

        if (d->m_changeArbiter != Q_NULLPTR) {
            Qt3DCore::QNodePropertyChangePtr change(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeRemoved, Qt3DCore::QSceneChange::Node, id()));
            change->setPropertyName("sequence");
            change->setValue(QVariant::fromValue(input->id()));
            d->notifyObservers(change);
        }

        d->m_sequences.removeOne(input);
    }
}

/*!
    Returns the QInputSequence's sequence vector.
 */
QVector<QAbstractActionInput *> QInputSequence::sequences() const
{
    Q_D(const QInputSequence);
    return d->m_sequences;
}

void QInputSequence::copy(const Qt3DCore::QNode *ref)
{
    const QInputSequence *input = static_cast<const QInputSequence *>(ref);
    d_func()->m_timeout = input->d_func()->m_timeout;
    d_func()->m_buttonInterval = input->d_func()->m_buttonInterval;
    d_func()->m_sequences = input->sequences();
}

Qt3DCore::QNodeCreatedChangeBasePtr QInputSequence::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QInputSequenceData>::create(this);
    QInputSequenceData &data = creationChange->data;

    Q_D(const QInputSequence);
    data.sequenceIds = qIdsForNodes(sequences());
    data.timeout = d->m_timeout;
    data.buttonInterval = d->m_buttonInterval;

    return creationChange;
}

} // Qt3DInput

QT_END_NAMESPACE
