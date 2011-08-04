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

#include "qglwidgetsurface.h"
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLWidgetSurface
    \brief The QGLWidgetSurface class represents a QGLWidget that is begin used as an OpenGL drawing surface.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    Constructs a widget surface.  This constructor should be followed
    by a call to setWidget().
*/
QGLWidgetSurface::QGLWidgetSurface()
    : QGLAbstractSurface(QGLAbstractSurface::Widget)
    , m_widget(0)
{
}

/*!
    Constructs a widget surface for \a widget.
*/
QGLWidgetSurface::QGLWidgetSurface(QGLWidget *widget)
    : QGLAbstractSurface(QGLAbstractSurface::Widget)
    , m_widget(widget)
{
}

/*!
    Destroys this widget surface.
*/
QGLWidgetSurface::~QGLWidgetSurface()
{
}

/*!
    Returns the widget that is underlying this surface.

    \sa setWidget()
*/
QGLWidget *QGLWidgetSurface::widget() const
{
    return m_widget;
}

/*!
    Sets the \a widget that is underlying this surface.

    \sa widget()
*/
void QGLWidgetSurface::setWidget(QGLWidget *widget)
{
    m_widget = widget;
}

/*!
    \reimp
*/
QPaintDevice *QGLWidgetSurface::device() const
{
    return m_widget;
}

/*!
    \reimp
*/
bool QGLWidgetSurface::activate(QGLAbstractSurface *prevSurface)
{
    Q_UNUSED(prevSurface);
    if (m_widget) {
        const QGLContext *context = m_widget->context();
        if (QGLContext::currentContext() != context)
            const_cast<QGLContext *>(context)->makeCurrent();
        return true;
    } else {
        return false;
    }
}

/*!
    \reimp
*/
void QGLWidgetSurface::deactivate(QGLAbstractSurface *nextSurface)
{
    // Nothing to do here - leave the context current.
    Q_UNUSED(nextSurface);
}

/*!
    \reimp
*/
QRect QGLWidgetSurface::viewportGL() const
{
    if (m_widget)
        return m_widget->rect();    // Origin assumed to be (0, 0).
    else
        return QRect();
}

QT_END_NAMESPACE
