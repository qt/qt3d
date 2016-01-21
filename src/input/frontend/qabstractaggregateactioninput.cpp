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

#include "qabstractaggregateactioninput.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DInput/private/qabstractaggregateactioninput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \internal
*/
QAbstractAggregateActionInputPrivate::QAbstractAggregateActionInputPrivate()
    : m_inputs()
{
}

/*!
    \class Qt3DInput::QAbstractAggregateActionInput
    \inherits QAbstractActionInput

    \inmodule Qt3DInput
    \since 5.7

    \brief QAbstractAggregateActionInput is the base class for the all Aggregate Action Inputs.

    The QAbstractAggregateActionInput class provides the storage for all the aggregate actions child actions.
*/

/*!
    \qmltype QAbstractAggregateActionInput
    \inqmlmodule Qt3D.Input
    \inherits QAbstractActionInput
    \instantiates Qt3DInput::QAbstractAggregateActionInput
    \brief QML frontend for the abstract Qt3DInput::QAbstractAggregateActionInput C++ class.

    The base class for all Aggregate Action Inputs.
    \since 5.7
*/

/*!
    \internal
*/
QAbstractAggregateActionInput::QAbstractAggregateActionInput(Qt3DInput::QAbstractAggregateActionInputPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DInput::QAbstractActionInput(dd, parent)
{
}

/*!
    \internal
*/
QAbstractAggregateActionInput::~QAbstractAggregateActionInput()
{
}

/*!
  \qmlproperty QQmlListProperty<Qt3DInput::QAbstractActionInput> Qt3D.Input::QAbstractAggregateActionInput::inputs

  The list of QAbstractActionInput that must be triggered to trigger this aggregate input.
*/

/*!
    Append the QAbstractActionInput \a input to the end of this Agggregate Actions input vector.

    \sa removeInput
 */
void QAbstractAggregateActionInput::addInput(QAbstractActionInput *input)
{
    Q_D(QAbstractAggregateActionInput);
    if (!d->m_inputs.contains(input)) {
        d->m_inputs.push_back(input);

        if (!input->parent())
            input->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            Qt3DCore::QScenePropertyChangePtr change(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeAdded, Qt3DCore::QSceneChange::Node, id()));
            change->setPropertyName("input");
            change->setValue(QVariant::fromValue(input->id()));
            d->notifyObservers(change);
        }
    }
}

/*!
    Remove the QAbstractActionInput \a input from this Agggregate Actions input vector.

    \sa addInput
 */
void QAbstractAggregateActionInput::removeInput(QAbstractActionInput *input)
{
    Q_D(QAbstractAggregateActionInput);
    if (d->m_inputs.contains(input)) {

        if (d->m_changeArbiter != Q_NULLPTR) {
            Qt3DCore::QScenePropertyChangePtr change(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeRemoved, Qt3DCore::QSceneChange::Node, id()));
            change->setPropertyName("input");
            change->setValue(QVariant::fromValue(input->id()));
            d->notifyObservers(change);
        }

        d->m_inputs.removeOne(input);
    }
}

/*!
    Return vector of QAbstractActionInput's in this Agggregate Actions input vector.
 */
QVector<QAbstractActionInput *> QAbstractAggregateActionInput::inputs() const
{
    Q_D(const QAbstractAggregateActionInput);
    return d->m_inputs;
}

void QAbstractAggregateActionInput::copy(const Qt3DCore::QNode *ref)
{
    QNode::copy(ref);
    const QAbstractAggregateActionInput *aggregateInput = static_cast<const QAbstractAggregateActionInput *>(ref);
    d_func()->m_inputs = aggregateInput->inputs();
}
} // Qt3DInput

QT_END_NAMESPACE
