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

#include "qglabstractsurface.h"
#include "qglcontextsurface_p.h"
#include "qglframebufferobjectsurface.h"
#include "qglsubsurface.h"
#include "qglwindowsurface.h"

#include <QDebug>
#include <QWindow>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>
#include <QSurface>
#include <qglobal.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractSurface
    \brief The QGLAbstractSurface class represents an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting

    OpenGL can be used to draw into a number of different surface types:
    windows, pixel buffers (pbuffers), framebuffer objects, and so on.
    It is also possible to use only part of a surface by setting
    the \c{glViewport()} to restrict drawing to that part.  An example
    of a subsurface may be the left or right eye image of a stereoscopic
    pair that is rendered into the two halves of a window.

    Note that pixelbuffers are only supported when Qt is built with the
    OpenGL library.

    Activating a surface for OpenGL drawing, and deactivating it afterwards
    can be quite varied across surface types.  Sometimes it is enough
    to just make a QOpenGLContext current and set the \c{glViewport()}.
    Other times a context must be made current, followed by binding a
    framebuffer object, and finally setting the \c{glViewport()}.

    QGLAbstractSurface and its subclasses simplify the activation and
    deactivation of surfaces by encapsulating the logic needed to
    use a particular kind of surface into activate() and deactivate()
    respectively.

    Normally surfaces are activated by calling QGLPainter::pushSurface()
    as in the following example of switching drawing to a framebuffer
    object:

    \code
    QGLPainter painter;
    painter.begin(widget);
    QGLFramebufferObjectSurface surface(fbo);
    painter.pushSurface(&surface);
    ... // draw into the fbo
    painter.popSurface();
    ... // draw into the widget
    \endcode

    QGLPainter maintains a stack of surfaces, starting with the paint
    device specified to QGLPainter::begin() (usually a widget).
    The QGLPainter::pushSurface() function calls deactivate() on the
    current surface, activate() on the new surface, and then adjusts the
    \c{glViewport()} to match the value of viewportGL() for the new surface.
    When QGLPainter::popSurface() is called, the previous surface
    is re-activated and the \c{glViewport()} changed accordingly.

    \sa QGLFramebufferObjectSurface, QGLWindowSurface, QGLSubsurface
    \sa QGLPixelBufferSurface, QGLPainter::pushSurface()
*/

class QGLAbstractSurfacePrivate
{
public:
    QGLAbstractSurfacePrivate(QGLAbstractSurface::SurfaceType surfaceType);
    ~QGLAbstractSurfacePrivate();

    QOpenGLContext *context;
    QWindow *window;
    QOpenGLFramebufferObject *fbo;
    int type;
};

QGLAbstractSurfacePrivate::QGLAbstractSurfacePrivate(QGLAbstractSurface::SurfaceType surfaceType)
    : context(0)
    , window(0)
    , fbo(0)
    , type(surfaceType)
{
    // nothing to do here
}

QGLAbstractSurfacePrivate::~QGLAbstractSurfacePrivate()
{
    // nothing to do here - objects are not owned by us
}

/*!
    \enum QGLAbstractSurface::SurfaceType
    This enum defines the type of a QGLAbstractSurface.

    \value Window Instance of QGLWindowSurface.
    \value FramebufferObject Instance of QGLFramebufferObjectSurface.
    \value PixelBuffer Instance of QGLPixelBufferSurface.
    \value Subsurface Instance of QGLSubsurface.
    \value User First user-defined surface type for use by applications.
*/

/*!
    \fn QGLAbstractSurface::QGLAbstractSurface(int surfaceType)

    Constructs an OpenGL drawing surface of the specified \a surfaceType.
*/
QGLAbstractSurface::QGLAbstractSurface(int surfaceType)
    : d_ptr(new QGLAbstractSurfacePrivate((QGLAbstractSurface::SurfaceType)surfaceType))
{
    Q_ASSERT(surfaceType >= Window);
}

/*!
    Destroys this OpenGL drawing surface.
*/
QGLAbstractSurface::~QGLAbstractSurface()
{
    // nothing to do - the scoped pointer will delete the d_ptr
}

/*!
    Returns the type of this surface.
*/
int QGLAbstractSurface::surfaceType() const
{
    Q_D(const QGLAbstractSurface);
    return d->type;
}

/*!
    Returns the OpenGL context which is associated with this surface, if any.
    When this surface is first activated, if this value is null then it will be
    set to the context current at that time.

    The default value is null.

    \sa setContext()
*/
QOpenGLContext *QGLAbstractSurface::context() const
{
    Q_D(const QGLAbstractSurface);
    return d->context;
}

/*!
    Sets the OpenGL context which is to be associated with this surface.
    When this surface is first activated, if this value is null then it will be
    set to the context current at that time.

    \sa context()
*/
void QGLAbstractSurface::setContext(QOpenGLContext *context)
{
    Q_D(QGLAbstractSurface);
    d->context = context;
}

/*!
    Returns the QWindow which is associated with this surface if any.
    The default value is null.

    \sa setWindow()
*/
QWindow *QGLAbstractSurface::window() const
{
    Q_D(const QGLAbstractSurface);
    return d->window;
}

/*!
    Sets the QWindow which is to be associated with this surface to be \a window.

    \sa window()
*/
void QGLAbstractSurface::setWindow(QWindow *window)
{
    Q_D(QGLAbstractSurface);
    d->window = window;
}

/*!
    Returns the QOpenGLFramebufferObject which is associated with this surface if any.
    The default value is null.

    \sa setFramebufferObject()
*/
QOpenGLFramebufferObject *QGLAbstractSurface::framebufferObject() const
{
    Q_D(const QGLAbstractSurface);
    return d->fbo;
}

/*!
    Sets the QOpenGLFramebufferObject which is to be associated with this surface to be \a framebufferObject.

    \sa framebufferObject()
*/
void QGLAbstractSurface::setFramebufferObject(QOpenGLFramebufferObject *framebufferObject)
{
    Q_D(QGLAbstractSurface);
    d->fbo = framebufferObject;
}

/*!
    \fn void QGLAbstractSurface::activate(QGLAbstractSurface *prevSurface)

    Activate this surface.

    If \a prevSurface is null, then that surface has just been deactivated
    in the process of switching to this surface.  This may allow activate()
    to optimize the transition to avoid unnecessary state changes.

    Typically implementations should assert if this fails in debug mode,
    since no rendering into the surface is possible.

    \sa deactivate(), switchTo()
*/

/*!
    \fn void QGLAbstractSurface::deactivate(QGLAbstractSurface *nextSurface)

    Deactivate this surface from the current context, but leave the
    context current.  Typically this will release the framebuffer
    object associated with the surface.

    If \a nextSurface is not-null, then that surface will be activated next
    in the process of switching away from this surface.  This may allow
    deactivate() to optimize the transition to avoid unnecessary state
    changes.

    \sa activate(), switchTo()
*/

/*!
    Returns the rectangle of the surface device() that is occupied by
    the viewport for this surface.  The origin is at the top-left.

    This function calls viewportGL() and then flips the rectangle
    upside down using the height of device() so that the origin
    is at the top-left instead of the bottom-left.

    \sa viewportGL(), device()
*/
QRect QGLAbstractSurface::viewportRect() const
{
    Q_ASSERT(isValid());

    QRect view = viewportGL();
    int height = 0;
    if (surfaceType() == Window)
    {
        Q_ASSERT(window());
        height = window()->height();
    }
    else if (surfaceType() == FramebufferObject)
    {
        Q_ASSERT(framebufferObject());
        height = framebufferObject()->size().height();
    }

    return QRect(view.x(), height - (view.y() + view.height()),
                 view.width(), view.height());
}

/*!
    \fn QRect QGLAbstractSurface::viewportGL() const

    Returns the rectangle of the surface that is occupied by
    the viewport for this surface.  The origin is at the bottom-left,
    which makes the value suitable for passing to \c{glViewport()}:

    \code
    QRect viewport = surface->viewportGL();
    glViewport(viewport.x(), viewport.y(), viewport.width(), viewport.height());
    \endcode

    Normally the viewport rectangle is the full extent of the device(),
    but it could be smaller if the application only wishes to draw
    into a subpart of the device().  An example would be rendering left
    and right stereo eye images into the two halves of a QGLWidget.
    The eye surfaces would typically be instances of QGLSubsurface.

    Note that the value returned from this function is not defined before
    the activate() function has been called at least once.

    \sa viewportRect(), device()
*/

/*!
    Returns the aspect ratio of viewportGL().

    The return value is used to correct perspective and orthographic
    projections for the aspect ratio of the drawing surface.

    No adjustments are made for DPI.

    Subclasses may override this function to further adjust the return value
    if the DPI in the horizontal vs vertical direction is not the same,
    that is, the pixels are not square.
*/
float QGLAbstractSurface::aspectRatio() const
{
    Q_ASSERT(isValid());

    // Get the size of the current viewport.
    QSize size = viewportGL().size();
    if (size.width() == size.height())
        return 1.0f;

    // Return the final aspect ratio based on viewport.
    return float(size.width()) / float(size.height());
}

/*!
    Switches from this surface to \a nextSurface by calling deactivate()
    on this surface and activate() on \a nextSurface.

    Returns true if \a nextSurface was activated, or false otherwise.
    If \a nextSurface could not be activated, then this surface will
    remain activated.

    \sa activate(), deactivate()
*/
bool QGLAbstractSurface::switchTo(QGLAbstractSurface *nextSurface)
{
    if (nextSurface) {
        deactivate(nextSurface);
        if (nextSurface->activate(this))
            return true;
        activate();
        return false;
    } else {
        deactivate();
        return true;
    }
}

/*!
    Returns true if this surface is valid and ready to be drawn into with OpenGL
    commands.  Typically it will be true if the surface has been associated with
    an opengl context and a supported painting context such as a window or fbo.

    Sub-class implementations can use this fall-back, which simply checks for
    a valid viewport rectangle.

    Note that the surface may only become valid during activate() calls.
*/
bool QGLAbstractSurface::isValid() const
{
    return viewportGL().isValid();
}

/*!
    Creates an OpenGL drawing surface using the given \a context.  This relies on
    the \a context having a valid surface which is a QWindow in which case the
    surface returned is a QGLWindowSurface.

    If not, then a generic surface based on the OpenGL context is returned.  In this
    case the generic surface will fail unless an underlying rendering surface becomes
    available prior to attempting to activate the surface.
*/
QGLAbstractSurface *QGLAbstractSurface::createSurfaceForContext(QOpenGLContext *context)
{
    Q_ASSERT(context);
#ifndef QT_NO_DEBUG_STREAM
    if (context->surface() && context->surface()->surfaceClass() != QSurface::Window)
        qWarning() << "Attempt to cast non-window surface";
#endif
    QWindow *win = static_cast<QWindow*>(context->surface());
    if (win)
    {
        return new QGLWindowSurface(win);
    }
    else
    {
        return new QGLContextSurface(context);
    }
    return 0;
}

QT_END_NAMESPACE
