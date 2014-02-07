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

#include "qglmaskedsurface_p.h"

#include <QOpenGLBuffer>

QT_BEGIN_NAMESPACE

/*!
    \class QGLMaskedSurface
    \brief The QGLMaskedSurface class represents a masked copy of another OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
    \internal

    Masked surfaces are typically used to render red-cyan anaglyph images
    into an underlying surface().  For the left eye image, the mask()
    is set to RedMask | AlphaMask.  Then for the right eye image, the mask()
    is set to GreenMask | BlueMask.
*/

/*!
    \enum QGLMaskedSurface::BufferMaskBit
    This enum defines the channels to mask with QGLMaskedSurface.

    \value RedMask Allow the red channel to be written to the color buffer.
    \value GreenMask Allow the green channel to be written to the color buffer.
    \value BlueMask Allow the blue channel to be written to the color buffer.
    \value AlphaMask Allow the alpha channel to be written to the color buffer.
*/

class QGLMaskedSurfacePrivate
{
public:
    QGLMaskedSurfacePrivate
        (QGLAbstractSurface *surf = 0,
         QGLMaskedSurface::BufferMask msk = QGLMaskedSurface::RedMask |
                                            QGLMaskedSurface::GreenMask |
                                            QGLMaskedSurface::BlueMask |
                                            QGLMaskedSurface::AlphaMask)
            : surface(surf), mask(msk) {}

    QGLAbstractSurface *surface;
    QGLMaskedSurface::BufferMask mask;
};

#define MaskedSurfaceType       501

/*!
    Constructs a masked OpenGL drawing surface with surface() initially
    set to null and mask() initially set to allow all channels to be
    written to the color buffer.
*/
QGLMaskedSurface::QGLMaskedSurface()
    : QGLAbstractSurface(MaskedSurfaceType)
    , d_ptr(new QGLMaskedSurfacePrivate)
{
}

/*!
    Constructs a masked OpenGL drawing surface that applies \a mask
    to \a surface when activate() is called.
*/
QGLMaskedSurface::QGLMaskedSurface
        (QGLAbstractSurface *surface, QGLMaskedSurface::BufferMask mask)
    : QGLAbstractSurface(MaskedSurfaceType)
    , d_ptr(new QGLMaskedSurfacePrivate(surface, mask))
{
}

/*!
    Destroys this masked OpenGL drawing surface.
*/
QGLMaskedSurface::~QGLMaskedSurface()
{
}

/*!
    Returns the underlying surface that mask() will be applied to
    when activate() is called.

    \sa setSurface(), mask()
*/
QGLAbstractSurface *QGLMaskedSurface::surface() const
{
    Q_D(const QGLMaskedSurface);
    return d->surface;
}

/*!
    Sets the underlying \a surface that mask() will be applied to
    when activate() is called.

    \sa surface(), setMask()
*/
void QGLMaskedSurface::setSurface(QGLAbstractSurface *surface)
{
    Q_D(QGLMaskedSurface);
    d->surface = surface;
}

/*!
    Returns the color mask to apply to surface() when activate()
    is called.

    \sa setMask(), surface()
*/
QGLMaskedSurface::BufferMask QGLMaskedSurface::mask() const
{
    Q_D(const QGLMaskedSurface);
    return d->mask;
}

/*!
    Sets the color \a mask to apply to surface() when activate()
    is called.

    \sa mask(), setSurface()
*/
void QGLMaskedSurface::setMask(QGLMaskedSurface::BufferMask mask)
{
    Q_D(QGLMaskedSurface);
    d->mask = mask;
}

/*!
    \reimp
*/
bool QGLMaskedSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_D(const QGLMaskedSurface);
    if (!d->surface || !d->surface->activate(prevSurface))
        return false;
    glColorMask((d->mask & RedMask) != 0, (d->mask & GreenMask) != 0,
                (d->mask & BlueMask) != 0, (d->mask & AlphaMask) != 0);
    return true;
}

/*!
    \reimp
*/
void QGLMaskedSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    Q_D(QGLMaskedSurface);
    if (d->surface)
        d->surface->deactivate(nextSurface);
    if (nextSurface && nextSurface->surfaceType() == MaskedSurfaceType) {
        // If we are about to switch to another masked surface for
        // the same underlying surface, then don't bother calling
        // glColorMask() for this one.
        QGLMaskedSurface *next = static_cast<QGLMaskedSurface *>(nextSurface);
        if (d->surface == next->surface())
            return;
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

/*!
    \reimp
*/
QRect QGLMaskedSurface::viewportGL() const
{
    Q_D(const QGLMaskedSurface);
    return d->surface ? d->surface->viewportGL() : QRect();
}

bool QGLMaskedSurface::isValid() const
{
    return QGLAbstractSurface::isValid();
}

QT_END_NAMESPACE
