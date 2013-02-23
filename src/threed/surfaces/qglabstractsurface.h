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

#ifndef QGLABSTRACTSURFACE_H
#define QGLABSTRACTSURFACE_H

#include <Qt3D/qt3dglobal.h>

#include <QRect>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

class QWindow;
class QOpenGLFramebufferObject;
class QOpenGLContext;
class QGLAbstractSurfacePrivate;

class Q_QT3D_EXPORT QGLAbstractSurface
{
public:
    virtual ~QGLAbstractSurface();

    enum SurfaceType
    {
        Window,
        FramebufferObject,
        Subsurface,
        User = 1000
    };

    int surfaceType() const;

    QOpenGLContext *context() const;
    void setContext(QOpenGLContext *context);
    QWindow *window() const;
    void setWindow(QWindow *window);
    QOpenGLFramebufferObject *framebufferObject() const;
    void setFramebufferObject(QOpenGLFramebufferObject *framebufferObject);

    virtual bool activate(QGLAbstractSurface *prevSurface = 0) = 0;
    virtual void deactivate(QGLAbstractSurface *nextSurface = 0) = 0;
    virtual QRect viewportGL() const = 0;
    QRect viewportRect() const;
    virtual float aspectRatio() const;

    bool switchTo(QGLAbstractSurface *nextSurface);
    virtual bool isValid() const;

    static QGLAbstractSurface *createSurfaceForContext(QOpenGLContext *context);

protected:
    QGLAbstractSurface(int surfaceType);

private:
    QScopedPointer<QGLAbstractSurfacePrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLAbstractSurface)
    Q_DISABLE_COPY(QGLAbstractSurface)
};

QT_END_NAMESPACE

#endif
