/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
    \instantiates Qt3DInput::QAbstractActionInput
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
