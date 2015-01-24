/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsysteminformationservice.h"
#include "qsysteminformationservice_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QSystemInformationService
    \inmodule Qt3DCore
    \brief Interface for a Qt3D system information service

    This is an interface class that should be subclassesd by providers of the
    system information service.
*/

/*!
    Creates an instance of QSystemInformationService. This constructor is protected
    so only subclasses can instantiate a QSystemInformationService object.
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
