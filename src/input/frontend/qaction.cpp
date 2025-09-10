// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaction.h"
#include "qaction_p.h"

#include <Qt3DInput/qabstractactioninput.h>

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \class Qt3DInput::QAction
    \inmodule Qt3DInput
    \inherits Qt3DCore::QNode
    \brief Links a set of QAbstractActionInput that trigger the same event.
    \since 5.7
*/

/*!
    \qmltype Action
    \inqmlmodule Qt3D.Input
    \nativetype Qt3DInput::QAction
    \brief QML frontend for the Qt3DInput::QAction C++ class.

    Links a set of AbstractActionInput that trigger the same event.
    \since 5.7
*/

/*!
    Constructs a new QAction instance with parent \a parent.
 */
QAction::QAction(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QActionPrivate(), parent)
{
}

/*! \internal */
QAction::~QAction()
{
}

/*!
    \qmlproperty bool Qt3D.Input::Action::action
*/

/*!
    \property Qt3DInput::QAction::active

    Holds \c true if the action is active.

    Note this property is not updated when the action is disabled.
 */
bool QAction::isActive() const
{
    Q_D(const QAction);
    return d->m_active;
}

/*!
    \qmlproperty list<AbstractActionInput> Qt3D.Input::Action::inputs

    The list of AbstractActionInput that must be triggered to trigger this Action.
*/

/*!
    Append QAbstractActionInput \a input to the list of inputs that can trigger this action.
 */
void QAction::addInput(QAbstractActionInput *input)
{
    Q_D(QAction);
    if (!d->m_inputs.contains(input)) {
        d->m_inputs.push_back(input);

        if (!input->parent())
            input->setParent(this);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(input, &QAction::removeInput, d->m_inputs);

        d->update();
    }
}

/*!
    Remove QAbstractActionInput \a input to the list of inputs that can trigger this action.
 */
void QAction::removeInput(QAbstractActionInput *input)
{
    Q_D(QAction);
    if (d->m_inputs.contains(input)) {

        d->update();

        d->m_inputs.removeOne(input);

        // Remove bookkeeping connection
        d->unregisterDestructionHelper(input);
    }
}

/*!
    Returns the list of inputs that can trigger this action.
 */
QList<QAbstractActionInput *> QAction::inputs() const
{
    Q_D(const QAction);
    return d->m_inputs;
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qaction.cpp"
