/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlogiccomponent.h"
#include "qlogiccomponent_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QLogicComponentPrivate
    \internal
*/
QLogicComponentPrivate::QLogicComponentPrivate()
    : QComponentPrivate()
{
}

/*!
    \class Qt3D::QLogicComponent
    \inmodule Qt3DLogic
    \since 5.5
    \brief Provides a way to have a synchronous function executed each frame.

    The QLogicComponent provides a way to perform tasks each frame in
    synchronized with the Qt3D backend. This is useful to implement some
    aspects of application logic and to prototype functionality that can later
    be folded into an additional Qt3D aspect.

    For example, the QLogicComponent can be used to animate a property in sync
    with the Qt3D engine where a Qt Quick animation element is not perfectly
    synchronized and may lead to stutters in some cases.

    To execute your own code each frame override the onFrameUpdate function.
*/

/*!
    \qmltype LogicComponent
    \inqmlmodule Qt3D.Logic
    \instantiates Qt3D::QLogicComponent
    \inherits Component3D
    \since 5.5
*/

/*!
    Constructs a new QLogicComponent instance with parent \a parent.
 */
QLogicComponent::QLogicComponent(QNode *parent)
    : QComponent(*new QLogicComponentPrivate, parent)
{
}

/*! \internal */
QLogicComponent::QLogicComponent(QLogicComponentPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

QLogicComponent::~QLogicComponent()
{
    QNode::cleanup();
}

/*!
    This virtual function will be called in a synchronous manner once each frame by
    the Logic aspect.
*/
void QLogicComponent::onFrameUpdate()
{
    // Emit signal so that QML instances get the onFrameUpdate() signal
    // handler called
    emit frameUpdate();
}

}

QT_END_NAMESPACE
