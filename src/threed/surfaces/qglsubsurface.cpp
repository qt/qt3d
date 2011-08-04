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

#include "qglsubsurface.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLSubsurface
    \brief The QGLSubsurface class represents a sub-surface of another OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

class QGLSubsurfacePrivate
{
public:
    QGLSubsurfacePrivate() : surface(0) {}
    QGLSubsurfacePrivate(QGLAbstractSurface *surf, const QRect &rgn)
        : surface(surf), region(rgn) {}

    QGLAbstractSurface *surface;
    QRect region;
};

/*!
    Constructs a new subsurface.  This constructor should be followed
    by calls to setSurface() and setRegion().
*/
QGLSubsurface::QGLSubsurface()
    : QGLAbstractSurface(QGLAbstractSurface::Subsurface)
    , d_ptr(new QGLSubsurfacePrivate)
{
}

/*!
    Constructs a new subsurface that occupies \a region within
    \a surface.  The \a region has its origin at the top-left
    of \a surface.
*/
QGLSubsurface::QGLSubsurface
        (QGLAbstractSurface *surface, const QRect &region)
    : QGLAbstractSurface(QGLAbstractSurface::Subsurface)
    , d_ptr(new QGLSubsurfacePrivate(surface, region))
{
}

/*!
    Destroys this subsurface.
*/
QGLSubsurface::~QGLSubsurface()
{
}

/*!
    Returns the surface behind this subsurface, or null if the
    surface has not been set.

    \sa setSurface(), region()
*/
QGLAbstractSurface *QGLSubsurface::surface() const
{
    Q_D(const QGLSubsurface);
    return d->surface;
}

/*!
    Sets the \a surface behind this subsurface.

    \sa surface(), setRegion()
*/
void QGLSubsurface::setSurface(QGLAbstractSurface *surface)
{
    Q_D(QGLSubsurface);
    d->surface = surface;
}

/*!
    Returns the region within surface() that is occupied by this
    subsurface, relative to the viewportRect() of surface().
    The origin is at the top-left of surface().

    \sa setRegion(), surface()
*/
QRect QGLSubsurface::region() const
{
    Q_D(const QGLSubsurface);
    return d->region;
}

/*!
    Sets the \a region within surface() that is occupied by this
    subsurface, relative to the viewportRect() of surface().
    The origin is at the top-left of surface().

    \sa region(), setSurface()
*/
void QGLSubsurface::setRegion(const QRect &region)
{
    Q_D(QGLSubsurface);
    d->region = region;
}

/*!
    \reimp
*/
QPaintDevice *QGLSubsurface::device() const
{
    Q_D(const QGLSubsurface);
    if (d->surface)
        return d->surface->device();
    else
        return 0;
}

/*!
    \reimp
*/
bool QGLSubsurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_D(QGLSubsurface);
    if (d->surface)
        return d->surface->activate(prevSurface);
    else
        return false;
}

/*!
    \reimp
*/
void QGLSubsurface::deactivate(QGLAbstractSurface *nextSurface)
{
    Q_D(QGLSubsurface);
    if (d->surface)
        d->surface->deactivate(nextSurface);
}

/*!
    \reimp
*/
QRect QGLSubsurface::viewportGL() const
{
    Q_D(const QGLSubsurface);
    if (d->surface) {
        // The underlying surface's viewportGL() has its origin
        // at the bottom-left, whereas d->region has its origin
        // at the top-left.  Flip the sub-region and adjust.
        QRect rect = d->surface->viewportGL();
        return QRect(rect.x() + d->region.x(),
                     rect.y() + rect.height() -
                        (d->region.y() + d->region.height()),
                     d->region.width(), d->region.height());
    } else {
        // Don't know the actual height of the surrounding surface,
        // so the best we can do is assume the region is bottom-aligned.
        return QRect(d->region.x(), 0, d->region.width(), d->region.height());
    }
}

QT_END_NAMESPACE
