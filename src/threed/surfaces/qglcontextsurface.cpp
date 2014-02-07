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

#include "qglcontextsurface_p.h"

#include <QtCore/QtDebug>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLContext>

QT_BEGIN_NAMESPACE

bool QGLContextSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    Q_ASSERT_X(QOpenGLContext::currentContext() || context(),
               Q_FUNC_INFO,
               "Activating GL contex surface without GL context");
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
    // Once we have used this context with a window remember that window and
    // complain if it is used with another window, since that will affect the
    // viewport and other rendering assumptions.
    if (!window())
    {
#ifndef QT_NO_DEBUG_STREAM
        if (!context()->surface() || context()->surface()->surfaceClass() == QSurface::Window)
            qWarning() << "Attempt to access context without GL window";
#endif
        setWindow(static_cast<QWindow*>(context()->surface()));
    }
#ifndef QT_NO_DEBUG_STREAM
    else
    {
        if (context()->surface() != window())
            qWarning() << "Attempt to render in wrong window for context";
    }
#endif
    return isValid();
}

void QGLContextSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    Q_UNUSED(nextSurface);
}

QRect QGLContextSurface::viewportGL() const
{
    if (window())
    {
        QRect geom = window()->geometry();
        return QRect(0,0,geom.width(),geom.height());
    }
#ifndef QT_NO_DEBUG_STREAM
    else
    {
        qWarning() << "Attempt to get viewport rect with no window\n"
                      << "Call activate() first";
    }
#endif
    return QRect();
}

bool QGLContextSurface::isValid() const
{
    bool winOK = true;
    if (window())
        winOK = window()->surfaceType() == QWindow::OpenGLSurface;
    return isValid() && winOK;
}

QT_END_NAMESPACE
