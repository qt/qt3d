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

#include "qglcontextsurface_p.h"

#include <QtCore/QtDebug>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLContext>

QT_BEGIN_NAMESPACE

bool QGLContextSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    Q_ASSERT_X(QOpenGLContext::currentContext() || m_context,
               Q_FUNC_INFO,
               "Activating GL contex surface without GL context");
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
    // Once we have used this context with a window remember that window and
    // complain if it is used with another window, since that will affect the
    // viewport and other rendering assumptions.
    if (!m_window)
    {
#ifndef QT_NO_DEBUG_STREAM
        if (!m_context->surface() || m_context->surface()->surfaceType() == QSurface::Window)
            qWarning() << "Attempt to access context without GL window";
#endif
        m_window = static_cast<QWindow*>(m_context->surface());
    }
#ifndef QT_NO_DEBUG_STREAM
    else
    {
        if (m_context->surface() != m_window)
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
    QRect r;
    if (m_window)
    {
        r = m_window->geometry();
    }
#ifndef QT_NO_DEBUG_STREAM
    else
    {
        qWarning() << "Attempt to get viewport rect with no window\n"
                      << "Call activate() first";
    }
#endif
    return r;
}

bool QGLContextSurface::isValid() const
{
    bool winOK = true;
    if (m_window)
        winOK = m_window->surfaceType() == QWindow::OpenGLSurface;
    return isValid() && winOK;
}

QT_END_NAMESPACE
