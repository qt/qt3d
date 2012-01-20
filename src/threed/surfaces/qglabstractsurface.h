/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
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

#ifndef QGLABSTRACTSURFACE_H
#define QGLABSTRACTSURFACE_H

#include "qt3dglobal.h"

#include <QRect>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QWindow;
class QOpenGLFramebufferObject;
#ifdef QT_OPENGL_LIB
class QGLPixelBuffer;
#endif
class QOpenGLContext;

class Q_QT3D_EXPORT QGLAbstractSurface
{
public:
    virtual ~QGLAbstractSurface();

    enum SurfaceType
    {
        Window,
        FramebufferObject,
#ifdef QT_OPENGL_LIB
        PixelBuffer,
#endif
        Subsurface,
        User = 1000
    };

    int surfaceType() const { return m_type; }

    QOpenGLContext *context() const
    {
        return m_context;
    }

    void setContext(QOpenGLContext *context)
    {
        m_context = context;
    }

    virtual bool activate(QGLAbstractSurface *prevSurface = 0) = 0;
    virtual void deactivate(QGLAbstractSurface *nextSurface = 0) = 0;
    virtual QRect viewportGL() const = 0;
    QRect viewportRect() const;
    virtual qreal aspectRatio() const;

    bool switchTo(QGLAbstractSurface *nextSurface);
    virtual bool isValid() const;

    static QGLAbstractSurface *createSurfaceForContext(QOpenGLContext *context);

protected:
    QGLAbstractSurface(int surfaceType);

    QOpenGLContext *m_context;
    QWindow *m_window;
    QOpenGLFramebufferObject *m_fbo;
#ifdef QT_OPENGL_LIB
    QGLPixelBuffer *m_pb;
#endif

private:
    int m_type;

    Q_DISABLE_COPY(QGLAbstractSurface)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
