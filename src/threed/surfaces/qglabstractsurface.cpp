/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifdef QT_OPENGL_LIB
#include <QGLPixelBuffer>
#include "qglpixelbuffersurface.h"
#endif

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
    : m_context(0)
    , m_window(0)
    , m_fbo(0)
#ifdef QT_OPENGL_LIB
    , m_pb(0)
#endif
    , m_type(surfaceType)
{
}

/*!
    Destroys this OpenGL drawing surface.
*/
QGLAbstractSurface::~QGLAbstractSurface()
{
}

/*!
    \fn int QGLAbstractSurface::surfaceType() const

    Returns the type of this surface.
*/

/*!
    \fn QOpenGLContext *context() const

    Returns the OpenGL context which is associated with this surface, if any.
    When this surface is first activated, if this value is null then it will be
    set to the context current at that time.

    The default value is null.

    \sa setContext()
*/

/*!
    \fn void context(QOpenGLContext *context)

    Sets the OpenGL context which is to be associated with this surface.
    When this surface is first activated, if this value is null then it will be
    set to the context current at that time.

    \sa context()
*/

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
    is at the top-left instead of the bottom-right.

    \sa viewportGL(), device()
*/
QRect QGLAbstractSurface::viewportRect() const
{
    Q_ASSERT(isValid());

    QRect view = viewportGL();
    int height = 0;
    if (m_type == Window)
    {
        Q_ASSERT(m_window);
        height = m_window->height();
    }
    else if (m_type == FramebufferObject)
    {
        Q_ASSERT(m_fbo);
        height = m_fbo->size().height();
    }
#ifdef QT_OPENGL_LIB
    else if (m_type == PixelBuffer)
    {
        Q_ASSERT(m_pb);
        height = m_pb->size().height();
    }
#endif

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
qreal QGLAbstractSurface::aspectRatio() const
{
    Q_ASSERT(isValid());

    // Get the size of the current viewport.
    QSize size = viewportGL().size();
    if (size.width() == size.height())
        return 1.0f;

    // Return the final aspect ratio based on viewport.
    return (qreal)size.width() / (qreal)size.height();
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
    if (context->surface() && context->surface()->surfaceType() != QSurface::Window)
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
