/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativeview3d.h"
#include <QtOpenGL/QGLWidget>

/*!
    \qmlclass QDeclarativeView3D
    \brief The QDeclarativeView3D replaces the QDeclarativeView for applications
    using OpenGL.
    \since 4.8
    \ingroup qt3d::qml3d

    \section1 Usage

    This class is substituted for the QDeclarativeView class when creating
    Qt3D applications.  It replaces the standard raster viewport with an OpenGL
    capable viewport suitable for rendering 3D content.
*/

QT_BEGIN_NAMESPACE

/*!
    \internal
*/
QDeclarativeView3D::QDeclarativeView3D(QWidget *parent) : QDeclarativeView(parent)
{
    //Set up the GL viewport widget format in the same manner that
    //qmlviewer does when openGL is specified.
    QGLFormat format = QGLFormat::defaultFormat();
#ifdef Q_WS_MAC
    format.setSampleBuffers(true);
#else
    format.setSampleBuffers(false);
#endif

    //Assign a GLWidget as the viewport.
    defaultViewportWidget = new QGLWidget(format);
    setViewport(defaultViewportWidget);
}

/*!
    \internal
*/
QDeclarativeView3D::~QDeclarativeView3D()
{
    if (defaultViewportWidget) delete defaultViewportWidget;
}

QT_END_NAMESPACE
