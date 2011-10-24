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

#include "qglwindowsurface.h"
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtGui/QOpenGLContext>
#include <QtGui/QWindow>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QGLWindowSurface
    \brief The QGLWindowSurface class represents a QGLwindow that is begin used as an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    Constructs a window surface, with a null window.  This constructor should be
    followed by a call to setWindow().
*/
QGLWindowSurface::QGLWindowSurface()
    : QGLAbstractSurface(QGLAbstractSurface::Window)
{
}

/*!
    Constructs a window surface for \a window.  If \a window is null,
    then this surface is invalid, that is isValid() will return null.
*/
QGLWindowSurface::QGLWindowSurface(QWindow *window)
    : QGLAbstractSurface(QGLAbstractSurface::Window)
{
    setWindow(window);
}

/*!
    Destroys this window surface.
*/
QGLWindowSurface::~QGLWindowSurface()
{
    // we don't own the window - don't destroy it here
}

/*!
    Returns the window that is underlying this surface.  By default this
    value is null.

    \sa setWindow()
*/
QWindow *QGLWindowSurface::window() const
{
    return m_window;
}

/*!
    Sets the \a window that is underlying this surface. If \a window is null,
    then this surface is invalid, that is isValid() will return null.

    The window should have a surfaceType() of QWindow::OpenGLSurface.
    \sa window()
*/
void QGLWindowSurface::setWindow(QWindow *window)
{
    m_window = window;
}

/*!
    \reimp
*/
bool QGLWindowSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    Q_ASSERT_X(QOpenGLContext::currentContext() || m_context, Q_FUNC_INFO,
               "Activating GL window surface without GL context");
    if (m_context)
    {
        if (m_context != QOpenGLContext::currentContext())
        {
            m_context->makeCurrent(m_window);
        }
    }
    else
    {
        m_context = QOpenGLContext::currentContext();
    }
    if (m_window)
    {
#ifndef QT_NO_DEBUG_STREAM
        if (m_context->surface() != m_window)
            qWarning() << "Attempt to activate GL window surface on wrong context";
#endif
    }
    else
    {
        m_window = static_cast<QWindow*>(m_context->surface());
    }
#ifndef QT_NO_DEBUG_STREAM
    if (!m_context->surface() || m_context->surface()->surfaceType() != QSurface::Window)
        qWarning() << "Attempt to activate GL window surface on bad context";
    if (!isValid())
        qWarning() << "Attempt to activate invalid window surface";
#endif
    return isValid();
}

/*!
    \reimp
*/
void QGLWindowSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    // Nothing to do here - leave the context current.
    Q_UNUSED(nextSurface);
}

/*!
    \reimp
*/
QRect QGLWindowSurface::viewportGL() const
{
    if (m_window)
        return m_window->geometry();    // Origin assumed to be (0, 0).
    else
        return QRect();
}

/*!
    \reimp
*/
bool QGLWindowSurface::isValid() const
{
    return QGLAbstractSurface::isValid() && m_window;
}

QT_END_NAMESPACE
