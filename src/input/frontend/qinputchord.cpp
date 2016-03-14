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

#include "qinputchord.h"
#include "qinputchord_p.h"
#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qabstractaggregateactioninput.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \class Qt3DInput::QInputChord
    \inmodule Qt3DInput
    \inherits QAbstractAggregateActionInput
    \brief QInputChord represents a set of QAbstractActionInput's that must be triggerd at once.

    \since 5.7
*/

/*!
    \qmltype InputChord
    \inqmlmodule Qt3D.Input
    \inherits QAbstractAggregateActionInput
    \instantiates Qt3DInput::QInputChord
    \brief QML frontend for the Qt3DInput::QInputChord C++ class.

    Represents a set of QAbstractActionInput's that must be triggerd at once.

    The following example shows an sequence that will be triggered by pressing the G, D, and J keys in that order with a maximum time between key presses of 1 second  and overall maximum input time of 3 seconds.
    \qml
    InputSequence {
        interval: 1000
        timeout: 3000
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
    \end
    \since 5.7
*/

/*!
    Constructs a new QInputChord with parent \a parent.
 */
QInputChord::QInputChord(Qt3DCore::QNode *parent)
    : Qt3DInput::QAbstractAggregateActionInput(*new QInputChordPrivate(), parent)
{

}

/*!
    Deletes the QInputChord instance.
 */
QInputChord::~QInputChord()
{
    QNode::cleanup();
}

/*!
  \fn QInputChord::timeoutChanged()

  This signal is emitted when the timeout of the input chord is changed.
*/

/*!
  \qmlproperty int Qt3D.Input::InputChord::tollerance

  The time in milliseconds in which all QAbstractActionInput's in the input chord must triggered within.
*/

/*!
    \qmlsignal Qt3D.Input::InputChord::tolleranceChanged()

    This signal is emitted when the timeout of the input chord is changed.

    The corresponding handeler is \c onTolleranceChanged
*/

/*!
    Returns the time in which all QAbstractActionInput's in the input chord must triggered within.
    The time is in milliseconds
 */
int QInputChord::timeout() const
{
    Q_D(const QInputChord);
    return d->m_timeout;
}

/*!
    Sets the time in which all QAbstractActionInput's in the input chord must triggered within.
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

void QInputChord::copy(const Qt3DCore::QNode *ref)
{
    QAbstractAggregateActionInput::copy(ref);
    const QInputChord *input = static_cast<const QInputChord *>(ref);
    d_func()->m_timeout = input->d_func()->m_timeout;
}

Qt3DCore::QNodeCreatedChangeBasePtr QInputChord::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QInputChordData>::create(this);
    auto &data = creationChange->data;

    Q_D(const QInputChord);
    data.inputIds = qIdsForNodes(inputs());
    data.timeout = d->m_timeout;

    return creationChange;
}

} // Qt3DInput

QT_END_NAMESPACE
