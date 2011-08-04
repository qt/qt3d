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

QT_BEGIN_NAMESPACE

/*!
    \class QGLFramebufferObjectSurface
    \brief The QGLFramebufferObjectSurface class represents a framebuffer object that is being used as an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

class QGLFramebufferObjectSurfacePrivate
{
public:
    QGLFramebufferObjectSurfacePrivate
            (QGLFramebufferObject *fbo, const QGLContext *ctx)
        : framebufferObject(fbo), context(ctx) {}

    QGLFramebufferObject *framebufferObject;
    const QGLContext *context;
};

/*!
    Constructs a default framebuffer object surface.  This constructor
    should be followed by a call to setFramebufferObject().
*/
QGLFramebufferObjectSurface::QGLFramebufferObjectSurface()
    : QGLAbstractSurface(QGLAbstractSurface::FramebufferObject)
    , d_ptr(new QGLFramebufferObjectSurfacePrivate(0, 0))
{
}

/*!
    Constructs a framebuffer object surface for \a fbo and \a context.
    If \a context is null, then the framebuffer will be bound to the
    current context when activate() is called.
*/
QGLFramebufferObjectSurface::QGLFramebufferObjectSurface
        (QGLFramebufferObject *fbo, const QGLContext *context)
    : QGLAbstractSurface(QGLAbstractSurface::FramebufferObject)
    , d_ptr(new QGLFramebufferObjectSurfacePrivate(fbo, context))
{
}

/*!
    Destroys this framebuffer object surface.
*/
QGLFramebufferObjectSurface::~QGLFramebufferObjectSurface()
{
}

/*!
    Returns the context that owns framebufferObject(); or null
    if the framebufferObject() should be assumed to be owned by
    the current context when activate() is called.

    \sa setContext(), framebufferObject()
*/
const QGLContext *QGLFramebufferObjectSurface::context() const
{
    Q_D(const QGLFramebufferObjectSurface);
    return d->context;
}

/*!
    Sets the \a context that owns framebufferObject().

    When activate() is called, it checks to see if \a context is sharing
    with the current context.  If it is, then the framebufferObject()
    is directly bound to the current context.  Otherwise, \a context
    is made current and then framebufferObject() is bound.

    If \a context is null, then framebufferObject() will be bound
    to whatever the current context is when activate() is called.

    \sa context()
*/
void QGLFramebufferObjectSurface::setContext(const QGLContext *context)
{
    Q_D(QGLFramebufferObjectSurface);
    d->context = context;
}

/*!
    Returns the framebuffer object for this surface, or null if
    it has not been set yet.

    \sa setFramebufferObject(), context()
*/
QGLFramebufferObject *QGLFramebufferObjectSurface::framebufferObject() const
{
    Q_D(const QGLFramebufferObjectSurface);
    return d->framebufferObject;
}

/*!
    Sets the framebuffer object for this surface to \a fbo.

    \sa framebufferObject()
*/
void QGLFramebufferObjectSurface::setFramebufferObject
        (QGLFramebufferObject *fbo)
{
    Q_D(QGLFramebufferObjectSurface);
    d->framebufferObject = fbo;
}

/*!
    \reimp
*/
QPaintDevice *QGLFramebufferObjectSurface::device() const
{
    Q_D(const QGLFramebufferObjectSurface);
    return d->framebufferObject;
}

/*!
    \reimp
*/
bool QGLFramebufferObjectSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    Q_D(QGLFramebufferObjectSurface);
    if (d->context) {
        if (!QGLContext::areSharing(QGLContext::currentContext(), d->context))
            const_cast<QGLContext *>(d->context)->makeCurrent();
    } else {
        // If we don't have a current context, then something is wrong.
        Q_ASSERT(QGLContext::currentContext());
    }
    if (d->framebufferObject)
        return d->framebufferObject->bind();
    return false;
}

/*!
    \reimp
*/
void QGLFramebufferObjectSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    Q_D(QGLFramebufferObjectSurface);
    if (d->framebufferObject) {
        if (nextSurface && nextSurface->surfaceType() == FramebufferObject) {
            // If we are about to switch to another fbo that is on the
            // same context, then don't bother doing the release().
            // This saves an unnecessary glBindFramebuffer(0) call.
            if (static_cast<QGLFramebufferObjectSurface *>(nextSurface)
                    ->context() == d->context)
                return;
        }
        d->framebufferObject->release();
    }
}

/*!
    \reimp
*/
QRect QGLFramebufferObjectSurface::viewportGL() const
{
    Q_D(const QGLFramebufferObjectSurface);
    if (d->framebufferObject)
        return QRect(QPoint(0, 0), d->framebufferObject->size());
    else
        return QRect();
}

QT_END_NAMESPACE
