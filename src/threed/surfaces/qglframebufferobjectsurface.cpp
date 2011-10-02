/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglframebufferobjectsurface.h"

#include <QtGui/QOpenGLContext>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QGLFramebufferObjectSurface
    \brief The QGLFramebufferObjectSurface class represents a framebuffer object that is being used as an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

// This exists solely for future expansion
class QGLFramebufferObjectSurfacePrivate
{
public:
    QGLFramebufferObjectSurfacePrivate()
        : forExpansion(0) {}

    int forExpansion;
};

/*!
    Constructs a default framebuffer object surface.  This constructor
    should be followed by a call to setFramebufferObject().
*/
QGLFramebufferObjectSurface::QGLFramebufferObjectSurface()
    : QGLAbstractSurface(QGLAbstractSurface::FramebufferObject)
    , d_ptr(new QGLFramebufferObjectSurfacePrivate)
{
}

/*!
    Constructs a framebuffer object surface for \a fbo and \a context.
    If \a context is null, then the framebuffer will be bound to the
    current context when activate() is called.
*/
QGLFramebufferObjectSurface::QGLFramebufferObjectSurface
        (QOpenGLFramebufferObject *fbo, QOpenGLContext *context)
    : QGLAbstractSurface(QGLAbstractSurface::FramebufferObject)
    , d_ptr(new QGLFramebufferObjectSurfacePrivate)
{
    m_fbo = fbo;
    m_context = context;
}

/*!
    Destroys this framebuffer object surface.
*/
QGLFramebufferObjectSurface::~QGLFramebufferObjectSurface()
{
}

/*!
    Returns the framebuffer object for this surface, or null if
    it has not been set yet.

    \sa setFramebufferObject()
*/
QOpenGLFramebufferObject *QGLFramebufferObjectSurface::framebufferObject() const
{
    return m_fbo;
}

/*!
    Sets the framebuffer object for this surface to \a fbo.

    \sa framebufferObject()
*/
void QGLFramebufferObjectSurface::setFramebufferObject
        (QOpenGLFramebufferObject *fbo)
{
    m_fbo = fbo;
}

/*!
    \reimp
*/
bool QGLFramebufferObjectSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    bool success = false;
    if (m_context) {
        if (!QOpenGLContext::areSharing(QOpenGLContext::currentContext(), m_context))
        {
            m_context->makeCurrent(m_context->surface());
        }
    } else {
        m_context = QOpenGLContext::currentContext();
    }

    if (isValid())
    {
        success = m_fbo->bind();
    }
#ifndef QT_NO_DEBUG_STREAM
    else
    {
        qWarning() << "Attempt to activate invalid fbo surface";
    }
#endif
    return success;
}

/*!
    \reimp
*/
void QGLFramebufferObjectSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    if (m_fbo) {
        if (nextSurface && nextSurface->surfaceType() == FramebufferObject) {
            // If we are about to switch to another fbo that is on the
            // same context, then don't bother doing the release().
            // This saves an unnecessary glBindFramebuffer(0) call.
            if (static_cast<QGLFramebufferObjectSurface *>(nextSurface)
                    ->context() == m_context)
                return;
        }
        m_fbo->release();
    }
}

/*!
    \reimp
*/
QRect QGLFramebufferObjectSurface::viewportGL() const
{
    if (m_fbo)
        return QRect(QPoint(0, 0), m_fbo->size());
    else
        return QRect();
}

/*!
    \reimp
*/
bool QGLFramebufferObjectSurface::isValid() const
{
    return QGLAbstractSurface::isValid() && m_fbo && m_context;
}

QT_END_NAMESPACE
