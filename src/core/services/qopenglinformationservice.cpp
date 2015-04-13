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

#include "qopenglinformationservice.h"
#include "qopenglinformationservice_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QOpenGLInformationServicePrivate
    \internal
*/

/*!
    \class Qt3D::QOpenGLInformationService
    \inmodule Qt3DCore
    \brief Interface for a Qt3D OpenGL information service

    This is an interface class that should be subclassesd by providers of the
    OpenGL information service. The service can be used to query various properties
    of the underlying OpenGL implementation without having to worry about
    having a valid OpenGL context on the current thread.
*/

/*!
    Creates an instance of QOpenGLInformationService, with a \a description for
    the new service. This constructor is protected so only subclasses can
    instantiate a QOpenGLInformationService object.
*/
QOpenGLInformationService::QOpenGLInformationService(const QString &description)
    : QAbstractServiceProvider(QServiceLocator::OpenGLInformation, description)
{
}

/*!
    \internal
*/
QOpenGLInformationService::QOpenGLInformationService(QOpenGLInformationServicePrivate &dd)
    : QAbstractServiceProvider(dd)
{
}

/*!
    \fn QSurfaceFormat Qt3D::QOpenGLInformationService::format() const

    Subclasses should override this function to return the QSurfaceFormat of the
    OpenGL context in use.
*/

} // namespace Qt3D

QT_END_NAMESPACE
