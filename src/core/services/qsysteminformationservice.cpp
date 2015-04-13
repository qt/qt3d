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

#include "qsysteminformationservice.h"
#include "qsysteminformationservice_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractServiceProviderPrivate
    \internal
*/

/*!
    \class Qt3D::QSystemInformationServicePrivate
    \internal
*/

/*!
    \class Qt3D::QSystemInformationService
    \inmodule Qt3DCore
    \brief Interface for a Qt3D system information service

    This is an interface class that should be subclassesd by providers of the
    system information service.
*/

/*!
    Creates an instance of QSystemInformationService, with a \a description for
    the new service. This constructor is protected so only subclasses can
    instantiate a QSystemInformationService object.
*/
QSystemInformationService::QSystemInformationService(const QString &description)
    : QAbstractServiceProvider(*new QSystemInformationServicePrivate(description))
{
}

/*!
    \internal
*/
QSystemInformationService::QSystemInformationService(QSystemInformationServicePrivate &dd)
    : QAbstractServiceProvider(dd)
{
}

/*!
    \fn QStringList Qt3D::QSystemInformationService::aspectNames()

    Subclasses should override this function and return a string list containing the
    names of all registered aspects.
*/

/*!
    \fn int threadPoolThreadCount() const

    Subclasses should override this function and return the number of threads in the
    Qt3D task manager's threadpool.
*/

}

QT_END_NAMESPACE
