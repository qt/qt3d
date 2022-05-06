/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
**
******************************************************************************/

#include "qopenglinformationservice_p.h"
#include "qopenglinformationservice_p_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {


/* !\internal
    \class Qt3DCore::QOpenGLInformationService
    \inmodule Qt3DCore
    \brief Interface for a Qt3D OpenGL information service

    This is an interface class that should be subclassesd by providers of the
    OpenGL information service. The service can be used to query various properties
    of the underlying OpenGL implementation without having to worry about
    having a valid OpenGL context on the current thread.
*/

/*
    Creates an instance of QOpenGLInformationService, with a \a description for
    the new service. This constructor is protected so only subclasses can
    instantiate a QOpenGLInformationService object.
*/
QOpenGLInformationService::QOpenGLInformationService(const QString &description)
    : QAbstractServiceProvider(*new QOpenGLInformationServicePrivate(description))
{
}

/*
    \internal
*/
QOpenGLInformationService::QOpenGLInformationService(QOpenGLInformationServicePrivate &dd)
    : QAbstractServiceProvider(dd)
{
}

/*
    \fn QSurfaceFormat Qt3DCore::QOpenGLInformationService::format() const

    Subclasses should override this function to return the QSurfaceFormat of the
    OpenGL context in use.
*/

} // namespace Qt3DCore

QT_END_NAMESPACE
