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
    \reimp
*/
bool QGLWindowSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    Q_ASSERT_X(QOpenGLContext::currentContext() || context(), Q_FUNC_INFO,
               "Activating GL window surface without GL context");
    if (context())
    {
        if (context() != QOpenGLContext::currentContext())
        {
            context()->makeCurrent(window());
        }
    }
    else
    {
        setContext(QOpenGLContext::currentContext());
    }
    if (window())
    {
#ifndef QT_NO_DEBUG_STREAM
        if (context()->surface() != window())
            qWarning() << "Attempt to activate GL window surface on wrong context";
#endif
    }
    else
    {
        setWindow(static_cast<QWindow*>(context()->surface()));
    }
#ifndef QT_NO_DEBUG_STREAM
    if (!context()->surface() || context()->surface()->surfaceClass() != QSurface::Window)
        qWarning() << "Attempt to activate GL window surface on bad context";
    if (!isValid())
    {
        qWarning() << "Attempt to activate invalid window surface";
        if (window() && !window()->geometry().isValid())
        {
            qWarning() << "Maybe set the window size, eg view.resize(800, 600)..?";
        }
    }
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
    if (window()) {
        QRect geom = window()->geometry();
        return QRect(0,0,geom.width(),geom.height());
    } else
        return QRect();
}

/*!
    \reimp
*/
bool QGLWindowSurface::isValid() const
{
    return QGLAbstractSurface::isValid() && window();
}

QT_END_NAMESPACE
