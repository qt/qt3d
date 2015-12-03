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

#include "qmousecontroller.h"
#include "qmousecontroller_p.h"

#include <Qt3DCore/qentity.h>


QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QMouseControllerPrivate::QMouseControllerPrivate()
    : QAbstractPhysicalDevicePrivate()
{
}

/*!
 * \class Qt3DInput::QMouseController
 * \inmodule Qt3DInput
 *
 * \brief QMouseController is in charge of dispatching mouse events to
 * attached QMouseInput objects.
 *
 * \since 5.5
 * \sa QMouseInput
 */
QMouseController::QMouseController(QNode *parent)
    : QAbstractPhysicalDevice(*new QMouseControllerPrivate, parent)
{
}

/*!
    \internal
*/
QMouseController::QMouseController(QMouseControllerPrivate &dd, QNode *parent)
    : QAbstractPhysicalDevice(dd, parent)
{
}

/*!
   Destroys this QMouseController object.
*/
QMouseController::~QMouseController()
{
    QNode::cleanup();
}

int QMouseController::axisCount() const
{
    // TO DO: we could have mouse wheel later on
    return 2;
}

int QMouseController::buttonCount() const
{
    return 3;
}

QStringList QMouseController::axisNames() const
{
    return QStringList()
            << QStringLiteral("X")
            << QStringLiteral("Y");
}

QStringList QMouseController::buttonNames() const
{
    return QStringList()
            << QStringLiteral("Left")
            << QStringLiteral("Right")
            << QStringLiteral("Center");
}

int QMouseController::axisIdentifier(const QString &name)
{
    if (name == QStringLiteral("X"))
        return X;
    else if (name == QStringLiteral("Y"))
        return Y;
    return -1;
}

int QMouseController::buttonIdentifier(const QString &name)
{
    if (name == QStringLiteral("Left"))
        return Left;
    else if (name == QStringLiteral("Right"))
        return Right;
    else if (name == QStringLiteral("Center"))
        return Center;
    return -1;
}

void QMouseController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_UNUSED(change);
    // TODO: To be completed as the mouse input aspect takes shape
}

} // namespace Qt3DInput

QT_END_NAMESPACE
