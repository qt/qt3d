// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#include "qabstractactioninput_p.h"

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
/*!
    \class Qt3DInput::QAbstractActionInput
    \inherits QNode

    \inmodule Qt3DInput
    \since 5.7

    \brief QAbstractActionInput is the base class for the Action Input and all Aggregate Action Inputs.
*/

/*!
    \qmltype AbstractActionInput
    \inqmlmodule Qt3D.Input
    \nativetype Qt3DInput::QAbstractActionInput
    \brief QML frontend for the abstract Qt3DInput::QAbstractActionInput C++ class.

    The base class for the Action Input and all Aggregate Action Inputs.
    \since 5.7
*/

/*!
    \internal
*/
QAbstractActionInput::QAbstractActionInput(QAbstractActionInputPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

/*!
    \internal
*/
QAbstractActionInput::~QAbstractActionInput()
{
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qabstractactioninput.cpp"
