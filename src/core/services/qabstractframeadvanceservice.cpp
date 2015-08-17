/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include "qabstractframeadvanceservice.h"
#include "qabstractframeadvanceservice_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractFrameAdvanceServicePrivate
    \internal
*/

/*!
    \class Qt3D::QAbstractFrameAdvanceService
    \inmodule Qt3DCore
    \brief Interface for a Qt3D frame advance service

    This is an interface class that should be subclassed by providers of the
    frame advance service. When used with the Renderer aspect, the aspect needs to
    be the one providing the ticks depending on the vertical refresh rate. When
    used with no Renderer aspect, a default tick clock implementation can be
    used.
*/


QAbstractFrameAdvanceService::QAbstractFrameAdvanceService(const QString &description)
    : QAbstractServiceProvider(QServiceLocator::FrameAdvanceService, description)
{

}

QAbstractFrameAdvanceService::QAbstractFrameAdvanceService(QAbstractFrameAdvanceServicePrivate &dd)
    : QAbstractServiceProvider(dd)
{

}

/*!
    \fn qint64 Qt3D::QAbstractFrameAdvanceService::waitForNextTick()

    Returns the current time, the call may be blocking if waiting for a tick.
*/

/*!
    \fn void Qt3D::QAbstractFrameAdvanceService::start()

    Starts the service.
*/

/*!
    \fn void Qt3D::QAbstractFrameAdvanceService::stop()

    Stops the service, performing any cleanup deemed necessary.
*/

} // Qt3D

QT_END_NAMESPACE
