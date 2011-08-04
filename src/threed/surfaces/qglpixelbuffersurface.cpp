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

#include "qglpixelbuffersurface.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLPixelBufferSurface
    \brief The QGLPixelBufferSurface class represents a pixel buffer that is being used as an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    Constructs a default pixel buffer surface.  This constructor
    should be followed by a call to setPixelBuffer().
*/
QGLPixelBufferSurface::QGLPixelBufferSurface()
    : QGLAbstractSurface(QGLAbstractSurface::PixelBuffer)
    , m_pbuffer(0)
{
}

/*!
    Constructs a pixel buffer surface for \a pbuffer.
*/
QGLPixelBufferSurface::QGLPixelBufferSurface(QGLPixelBuffer *pbuffer)
    : QGLAbstractSurface(QGLAbstractSurface::PixelBuffer)
    , m_pbuffer(pbuffer)
{
}

/*!
    Destroys this pixel buffer surface.
*/
QGLPixelBufferSurface::~QGLPixelBufferSurface()
{
}

/*!
    Returns the pixel buffer for this surface, or null if
    it has not been set yet.

    \sa setPixelBuffer()
*/
QGLPixelBuffer *QGLPixelBufferSurface::pixelBuffer() const
{
    return m_pbuffer;
}

/*!
    Sets the framebuffer object for this surface to \a pbuffer.

    \sa pixelBuffer()
*/
void QGLPixelBufferSurface::setPixelBuffer
        (QGLPixelBuffer *pbuffer)
{
    m_pbuffer = pbuffer;
}

/*!
    \reimp
*/
QPaintDevice *QGLPixelBufferSurface::device() const
{
    return m_pbuffer;
}

/*!
    \reimp
*/
bool QGLPixelBufferSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    if (m_pbuffer)
        return m_pbuffer->makeCurrent();
    else
        return false;
}

/*!
    \reimp
*/
void QGLPixelBufferSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    // Nothing to do here - leave the context current.
    Q_UNUSED(nextSurface);
}

/*!
    \reimp
*/
QRect QGLPixelBufferSurface::viewportGL() const
{
    if (m_pbuffer)
        return QRect(0, 0, m_pbuffer->width(), m_pbuffer->height());
    else
        return QRect();
}

QT_END_NAMESPACE
