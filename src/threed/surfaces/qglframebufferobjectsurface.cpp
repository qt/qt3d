/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
    setFramebufferObject(fbo);
    setContext(context);
}

/*!
    Destroys this framebuffer object surface.
*/
QGLFramebufferObjectSurface::~QGLFramebufferObjectSurface()
{
}

/*!
    \reimp
*/
bool QGLFramebufferObjectSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    bool success = false;
    if (context()) {
        if (!QOpenGLContext::areSharing(QOpenGLContext::currentContext(), context()))
        {
            context()->makeCurrent(context()->surface());
        }
    } else {
        setContext(QOpenGLContext::currentContext());
    }

    if (isValid())
    {
        success = framebufferObject()->bind();
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
    if (framebufferObject()) {
        if (nextSurface && nextSurface->surfaceType() == FramebufferObject) {
            // If we are about to switch to another fbo that is on the
            // same context, then don't bother doing the release().
            // This saves an unnecessary glBindFramebuffer(0) call.
            if (static_cast<QGLFramebufferObjectSurface *>(nextSurface)
                    ->context() == context())
                return;
        }
        framebufferObject()->release();
    }
}

/*!
    \reimp
*/
QRect QGLFramebufferObjectSurface::viewportGL() const
{
    if (framebufferObject())
        return QRect(QPoint(0, 0), framebufferObject()->size());
    else
        return QRect();
}

/*!
    \reimp
*/
bool QGLFramebufferObjectSurface::isValid() const
{
    return QGLAbstractSurface::isValid() && framebufferObject() && context();
}

QT_END_NAMESPACE
