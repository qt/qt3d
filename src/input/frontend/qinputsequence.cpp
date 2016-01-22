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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/QAbstractAggregateActionInput>
#include <Qt3DInput/private/qabstractaggregateactioninput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
/*!
    \class Qt3DInput::QInputChordSequence
    \internal
*/
class QInputSequencePrivate : public Qt3DInput::QAbstractAggregateActionInputPrivate
{
public:
    QInputSequencePrivate()
        : Qt3DInput::QAbstractAggregateActionInputPrivate()
        , m_timeout(0)
        , m_interval(0)
        , m_sequential(true)
    {}

    int m_timeout;
    int m_interval;
    bool m_sequential;
};

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
    : Qt3DInput::QAbstractAggregateActionInput(*new QInputSequencePrivate(), parent)
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
  \fn QInputSequence::intervalChanged()

  This signal is emitted when the interval of the input sequence is changed.
*/

/*!
  \qmlproperty int Qt3D.Input::InputSequence::interval

  The maximum time in milliseconds in between consecutive QAbstractActionInput's in the input sequence.
*/

/*!
    \qmlsignal Qt3D.Input::InputSequence::intervalChanged()

    This signal is emitted when the interval of the input sequence is changed.

    The corresponding handeler is \c onIntervalChanged
*/

/*!
    Returns the maximum time in between consecutive QAbstractActionInput's in the input sequence.
    The time is in milliseconds
 */
int QInputSequence::interval() const
{
    Q_D(const QInputSequence);
    return d->m_interval;
}

/*!
  \fn QInputSequence::sequentialChanged()

  This signal is emitted when the sequential property of the input sequence is changed.
*/

/*!
  \qmlproperty bool Qt3D.Input::InputSequence::sequential

  If the QAbstractActionInput's in the input sequence must triggered in order.
*/

/*!
    \qmlsignal Qt3D.Input::InputSequence::sequentialChanged()

    This signal is emitted when the sequential property of the input sequence is changed.

    The corresponding handeler is \c onSequentialChanged
*/

/*!
    Returns true if the QAbstractActionInput's in the input sequence must triggered in order.
 */
bool QInputSequence::sequential() const
{
    Q_D(const QInputSequence);
    return d->m_sequential;
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
void QInputSequence::setInterval(int interval)
{
    Q_D(QInputSequence);
    if (d->m_interval != interval) {
        d->m_interval = interval;
        emit intervalChanged(interval);
    }
}

/*!
    Set if the QAbstractActionInput's in the input sequence must triggered in order.
 */
void QInputSequence::setSequential(bool sequential)
{
    Q_D(QInputSequence);
    if (d->m_sequential != sequential) {
        d->m_sequential = sequential;
        emit sequentialChanged(sequential);
    }
}

void QInputSequence::copy(const Qt3DCore::QNode *ref)
{
    QAbstractAggregateActionInput::copy(ref);
    const QInputSequence *input = static_cast<const QInputSequence *>(ref);
    d_func()->m_timeout = input->d_func()->m_timeout;
    d_func()->m_interval = input->d_func()->m_interval;
    d_func()->m_sequential = input->d_func()->m_sequential;
}

} // Qt3DInput

QT_END_NAMESPACE
