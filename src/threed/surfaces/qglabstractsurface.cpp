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

#include "qglabstractsurface.h"
#include "qglcontextsurface_p.h"
#include "qglframebufferobjectsurface.h"
#include "qglpixelbuffersurface.h"
#include "qglsubsurface.h"
#include "qglwidgetsurface.h"
#include <QtGui/qpaintdevice.h>

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

    Activating a surface for OpenGL drawing, and deactivating it afterwards
    can be quite varied across surface types.  Sometimes it is enough
    to just make a QGLContext current and set the \c{glViewport()}.
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

    \sa QGLFramebufferObjectSurface, QGLWidgetSurface, QGLSubsurface
    \sa QGLPixelBufferSurface, QGLPainter::pushSurface()
*/

/*!
    \enum QGLAbstractSurface::SurfaceType
    This enum defines the type of a QGLAbstractSurface.

    \value Widget Instance of QGLWidgetSurface.
    \value FramebufferObject Instance of QGLFramebufferObjectSurface.
    \value PixelBuffer Instance of QGLPixelBufferSurface.
    \value Subsurface Instance of QGLSubsurface.
    \value User First user-defined surface type for use by applications.
*/

/*!
    \fn QGLAbstractSurface::QGLAbstractSurface(int surfaceType)

    Constructs an OpenGL drawing surface of the specified \a surfaceType.
*/

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
    \fn QPaintDevice *QGLAbstractSurface::device() const

    Returns the raw device that this surface will draw on.

    If the surface is an instance of QGLSubsurface, then this will
    return the device of the surface that underlies the subsurface.
    The viewportRect() defines the region to render into.

    \sa viewportRect()
*/

/*!
    \fn bool QGLAbstractSurface::activate(QGLAbstractSurface *prevSurface)

    Activate this surface by making its context current, and binding
    the associated framebuffer object, if any.

    If \a prevSurface is null, then that surface has just been deactivated
    in the process of switching to this surface.  This may allow activate()
    to optimize the transition to avoid unnecessary state changes.

    Returns true if the surface was activated; false otherwise.

    \sa deactivate(), switchTo()
*/

/*!
    \fn void QGLAbstractSurface::deactivate(QGLAbstractSurface *nextSurface)

    Deactivate this surface from the current context, but leave the
    context current.  Typically this will release the framebuffer
    object associated with the surface.

    If \a nextSurface is null, then that surface will be activated next
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
    QRect view = viewportGL();
    QPaintDevice *dev = device();
    int height;
    if (dev->devType() == QInternal::Widget)
        height = static_cast<QWidget *>(dev)->height();
    else
        height = dev->height();
    return QRect(view.x(), height - (view.y() + view.height()),
                 view.width(), view.height());
}

/*!
    \fn QRect QGLAbstractSurface::viewportGL() const

    Returns the rectangle of the surface device() that is occupied by
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

    \sa viewportRect(), device()
*/

/*!
    Returns the aspect ratio of viewportGL() after correcting for the
    DPI of device().

    The return value is used to correct perspective and orthographic
    projections for the aspect ratio of the drawing surface.  Subclasses
    may override this function to adjust the return value if the DPI of
    device() is not sufficient to determine the aspect ratio.
*/
qreal QGLAbstractSurface::aspectRatio() const
{
    // Get the size of the current viewport.
    QSize size = viewportGL().size();
    if (size.width() == 0 || size.height() == 0 ||
            size.width() == size.height())
        return 1.0f;

    // Use the device's DPI setting to determine the pixel aspect ratio.
    QPaintDevice *device = this->device();
    int dpiX = device->logicalDpiX();
    int dpiY = device->logicalDpiY();
    if (dpiX <= 0 || dpiY <= 0)
        dpiX = dpiY = 1;

    // Return the final aspect ratio based on viewport and pixel size.
    return ((qreal)(size.width() * dpiY)) / ((qreal)(size.height() * dpiX));
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
    Creates an OpenGL drawing surface for the specified paint \a device.
    Returns null if it is not possible to create a surface for \a device.

    \sa createSurfaceForContext()
*/
QGLAbstractSurface *QGLAbstractSurface::createSurfaceForDevice
    (QPaintDevice *device)
{
    Q_ASSERT(device);
    switch (device->devType()) {
    case QInternal::Widget: {
        QGLWidget *glw = qobject_cast<QGLWidget *>
                (static_cast<QWidget *>(device));
        if (glw)
            return new QGLWidgetSurface(glw);
        else
            return 0;
    }
    case QInternal::Pbuffer:
        return new QGLPixelBufferSurface(static_cast<QGLPixelBuffer *>(device));
    case QInternal::FramebufferObject:
        return new QGLFramebufferObjectSurface
            (static_cast<QGLFramebufferObject *>(device));
    default:
        return 0;
    }
}

/*!
    Creates an OpenGL drawing surface for the paint device
    underlying \a context.  If the paint device is not recognized,
    then a generic surface will be created that makes \a context
    current when the surface is activated.

    \sa createSurfaceForDevice()
*/
QGLAbstractSurface *QGLAbstractSurface::createSurfaceForContext
    (const QGLContext *context)
{
    Q_ASSERT(context);
    QGLAbstractSurface *surface = createSurfaceForDevice(context->device());
    if (!surface)
        surface = new QGLContextSurface(context);
    return surface;
}

QT_END_NAMESPACE
