// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#include "moc_qopenglinformationservice_p.cpp"
