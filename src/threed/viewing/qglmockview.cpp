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

#include "qglmockview.h"
#include "qglmockview_p.h"

#include <QOpenGLContext>
#include <QDebug>

/*!
    \class QGLMockView
    \brief The QGLMockView class exists purely for unit testing.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::viewing

    The QGLMockView class exists purely for unit test writing and should
    never be used for creating actual applications.  QGLMockView is for
    advanced, mostly internal use and is not supported as part of the Qt3D
    functional API.
*/

QGLMockViewPrivate::QGLMockViewPrivate()
    : valid(false)
    , ctx(0)
{
    // nothing to do here
}

QGLMockViewPrivate::~QGLMockViewPrivate()
{
    delete ctx;
}

/*!
    Create a new QGLMockView object.  Internally this sets up the underlying
    QWindow with some typical values useful for unit-testing, and then tries
    to create the window.  If this fails the instance is invalid and it can
    be assumed the unit test environment does not support GL.
*/
QGLMockView::QGLMockView(QWindow *parent)
    : QWindow(parent)
    , dptr(new QGLMockViewPrivate)
{
    setGeometry(QRect(50, 50, 100, 100));
    setSurfaceType(QWindow::OpenGLSurface);
    create();
    dptr->valid = ensureContext();
    if (!dptr->valid)
        qWarning() << "Could not get valid GL in QGLMockView - test may fail";
}

/*!
    Destroys this QGLMockView object, recovering any resources.
*/
QGLMockView::~QGLMockView()
{
    // nothing to do here - dptr cleaned up automatically
}

/*!
    Returns true if this QGLMockView object can be used for testing.

    \code
    QGLMockView view;
    if (!view.isValid())
        QSKIP("GL not supported on this test platform");
    \endcode

    The isValid() function will be false typically if the underlying
    GL context could not be created for this window.
*/
bool QGLMockView::isValid() const
{
    return dptr->valid;
}

/*!
    Returns the QOpenGLContext for this mock view, if one exists.

    By default this value is null, but the constructor will attempt
    to set it to a non-null and valid context, and then make that
    context current on this window.

    \code
    QGLMockView view;
    QCOMPARE(view.context(), QOpenGLContext::currentContext());
    \endcode
*/
QOpenGLContext *QGLMockView::context()
{
    return dptr->ctx;
}

bool QGLMockView::ensureContext()
{
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    if (!dptr->ctx)
        dptr->ctx = new QOpenGLContext;
    dptr->ctx->setFormat(format);
#ifndef QT_NO_DEBUG_STREAM
    QSurfaceFormat oldFormat = format;
#endif
    dptr->ctx->create();
    if (!dptr->ctx->isValid())
        return false;
    // TODO: is it possible that the platform will downgrade the actual
    // format, or will it just fail if it can't deliver the actual format
    format = dptr->ctx->format();
#ifndef QT_NO_DEBUG_STREAM
    if (oldFormat.swapBehavior() != format.swapBehavior())
        qWarning() << "Could not specify swap behavior"
                   << oldFormat.swapBehavior();
#endif
    bool res = dptr->ctx->makeCurrent(this);
#ifndef QT_NO_DEBUG_STREAM
    if (!res)
        qWarning() << "Could not make context current!";
#endif
    return res;
}

