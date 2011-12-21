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

#include <QtTest/QtTest>
#include "qglabstractsurface.h"
#include "qglwindowsurface.h"
#include "qglframebufferobjectsurface.h"

#ifdef QT_OPENGL_LIB
#include "qglpixelbuffersurface.h"
#endif

#include "qglsubsurface.h"
#include "qopenglfunctions.h"

#include <QtGui/QOpenGLContext>

class tst_QGLAbstractSurface : public QObject
{
    Q_OBJECT
public:
    tst_QGLAbstractSurface() {}
    ~tst_QGLAbstractSurface() {}

private slots:
    void glWindowSurface();
    void fboSurface();
    //  TODO: void pbufferSurface();
    void subSurface();

private:
    bool isFboCurrent(QOpenGLFramebufferObject *fbo);
};

bool tst_QGLAbstractSurface::isFboCurrent(QOpenGLFramebufferObject *fbo)
{
    GLint currentFbo = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo);
    if (currentFbo == -1)
        return true;    // XXX: Mesa 7.0.3 bug?
    if (fbo)
        return fbo->handle() == GLuint(currentFbo);
    else
        return currentFbo == 0;
}

static void ensureContext(QWindow &win, QOpenGLContext &ctx)
{
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    ctx.setFormat(format);
#ifndef QT_NO_DEBUG_STREAM
    QSurfaceFormat oldFormat = format;
#endif
    ctx.create();
    // TODO: is it possible that the platform will downgrade the actual
    // format, or will it just fail if it can't deliver the actual format
    format = ctx.format();
#ifndef QT_NO_DEBUG_STREAM
    if (oldFormat.swapBehavior() != format.swapBehavior())
        qWarning() << "Could not create context for swap behavior"
                   << oldFormat.swapBehavior();
#endif
    ctx.makeCurrent(&win);
}

void tst_QGLAbstractSurface::glWindowSurface()
{
    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid())
    {
        QSKIP("GL Implementation not valid");
    }

    QGLWindowSurface surface1;
    QVERIFY(surface1.surfaceType() == QGLAbstractSurface::Window);
    QVERIFY(surface1.window() == 0);
    surface1.setWindow(&glw);
    QVERIFY(surface1.window() == &glw);
    QCOMPARE(surface1.viewportRect(), glw.geometry());

    QVERIFY(surface1.activate());
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));

    surface1.deactivate();
    QVERIFY(QOpenGLContext::currentContext() == &ctx); // Left current.

    QWindow glw2;
    glw2.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx2;
    ensureContext(glw2, ctx2);

    QGLWindowSurface surface2(&glw2);
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::Window);
    QVERIFY(surface2.window() == &glw2);

    QVERIFY(surface1.activate());
    QVERIFY(surface1.switchTo(&surface2));

    QVERIFY(QOpenGLContext::currentContext() == &ctx2);

    QVERIFY(surface2.switchTo(&surface1));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
}

void tst_QGLAbstractSurface::fboSurface()
{
    if (!QOpenGLFramebufferObject::hasOpenGLFramebufferObjects())
        QSKIP("fbo's are not supported");

    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid())
        QSKIP("GL Implementation not valid");

    QGLWindowSurface surface1(&glw);
    QVERIFY(surface1.activate());
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));

    QOpenGLFramebufferObject fbo1(128, 128);
    QVERIFY(fbo1.handle() != 0);

    QGLFramebufferObjectSurface surface2(&fbo1);
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::FramebufferObject);
    QVERIFY(surface2.framebufferObject() == &fbo1);

    QVERIFY(isFboCurrent(0));

    QVERIFY(surface2.activate());
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QVERIFY(isFboCurrent(&fbo1));
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    surface2.deactivate();
    QVERIFY(isFboCurrent(0));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);

    surface2.setContext(&ctx);

    QWindow glw2;
    glw2.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx2;
    ensureContext(glw2, ctx2);

    QGLWindowSurface surface3(&glw2);
    QVERIFY(surface3.activate());
    QVERIFY(QOpenGLContext::currentContext() == &ctx2);
    QCOMPARE(surface3.viewportGL(), QRect(0, 0, glw2.width(), glw2.height()));

    QVERIFY(surface3.switchTo(&surface2));
    QVERIFY(isFboCurrent(&fbo1));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    QVERIFY(surface2.switchTo(&surface3));
    QVERIFY(isFboCurrent(0));
    QVERIFY(QOpenGLContext::currentContext() == &ctx2);
    QCOMPARE(surface3.viewportGL(), QRect(0, 0, glw2.width(), glw2.height()));

    surface3.deactivate();

    surface1.activate();
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));

    QOpenGLFramebufferObject fbo2(256, 256);
    QVERIFY(fbo2.handle() != 0);

    QGLFramebufferObjectSurface surface4;
    QVERIFY(surface4.surfaceType() == QGLAbstractSurface::FramebufferObject);
    QVERIFY(surface4.framebufferObject() == 0);
    QVERIFY(!surface4.activate(0));
    QVERIFY(surface4.viewportGL().isNull());
    surface4.setFramebufferObject(&fbo2);
    QVERIFY(surface4.framebufferObject() == &fbo2);
    surface4.setContext(&ctx);

    QVERIFY(surface1.switchTo(&surface2));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QVERIFY(isFboCurrent(&fbo1));
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    QVERIFY(surface2.switchTo(&surface4));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QVERIFY(isFboCurrent(&fbo2));
    QCOMPARE(surface4.viewportGL(), QRect(QPoint(0, 0), fbo2.size()));

    QVERIFY(surface3.switchTo(&surface2));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QVERIFY(isFboCurrent(&fbo1));
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    QVERIFY(surface2.switchTo(&surface1));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QVERIFY(isFboCurrent(0));
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));
}

/*
void tst_QGLAbstractSurface::pbufferSurface()
{
    if (!QGLPixelBuffer::hasOpenGLPbuffers())
        QSKIP("pbuffer's are not supported");

    QGLWidget glw;
    if (!glw.isValid())
        QSKIP("GL Implementation not valid");

    QGLWindowSurface surface1(&glw);
    QVERIFY(surface1.activate());
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));

    QGLPixelBuffer pbuffer1(128, 128);
    QVERIFY(pbuffer1.handle() != 0);

    QGLPixelBufferSurface surface2(&pbuffer1);
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::PixelBuffer);
    QVERIFY(surface2.pixelBuffer() == &pbuffer1);
    QVERIFY(surface2.device() == &pbuffer1);

    QVERIFY(surface2.activate());
    QVERIFY(QOpenGLContext::currentContext() != glw.context());
    const QOpenGLContext *surface2ctx = QOpenGLContext::currentContext();
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), pbuffer1.size()));

    surface2.deactivate();
    QVERIFY(QOpenGLContext::currentContext() == surface2ctx); // Left current.

    QGLWidget glw2;
    QGLWindowSurface surface3(&glw2);
    QVERIFY(surface3.activate());
    QVERIFY(QOpenGLContext::currentContext() == glw2.context());
    QCOMPARE(surface3.viewportGL(), QRect(0, 0, glw2.width(), glw2.height()));

    QVERIFY(surface3.switchTo(&surface2));
    QVERIFY(QOpenGLContext::currentContext() == surface2ctx);
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), pbuffer1.size()));

    QVERIFY(surface2.switchTo(&surface3));
    QVERIFY(QOpenGLContext::currentContext() == glw2.context());
    QCOMPARE(surface3.viewportGL(), QRect(0, 0, glw2.width(), glw2.height()));

    surface3.deactivate();

    surface1.activate();
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));

    QGLPixelBuffer pbuffer2(256, 256);
    QVERIFY(pbuffer2.handle() != 0);

    QGLPixelBufferSurface surface4;
    QVERIFY(surface4.surfaceType() == QGLAbstractSurface::PixelBuffer);
    QVERIFY(surface4.pixelBuffer() == 0);
    QVERIFY(surface4.device() == 0);
    QVERIFY(!surface4.activate(0));
    QVERIFY(surface4.viewportGL().isNull());
    surface4.setPixelBuffer(&pbuffer2);
    QVERIFY(surface4.pixelBuffer() == &pbuffer2);
    QVERIFY(surface4.device() == &pbuffer2);

    QVERIFY(surface1.switchTo(&surface2));
    QVERIFY(QOpenGLContext::currentContext() == surface2ctx);
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), pbuffer1.size()));

    QVERIFY(surface2.switchTo(&surface4));
    const QOpenGLContext *surface4ctx = QOpenGLContext::currentContext();
    QVERIFY(surface4ctx != glw.context());
    QVERIFY(surface4ctx != surface2ctx);
    QCOMPARE(surface4.viewportGL(), QRect(QPoint(0, 0), pbuffer2.size()));

    QVERIFY(surface4.switchTo(&surface2));
    QVERIFY(QOpenGLContext::currentContext() == surface2ctx);
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), pbuffer1.size()));

    QVERIFY(surface2.switchTo(&surface1));
    QVERIFY(QOpenGLContext::currentContext() == glw.context());
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));
}
*/
void tst_QGLAbstractSurface::subSurface()
{
    QWindow glw;
    glw.setSurfaceType(QWindow::OpenGLSurface);
    QOpenGLContext ctx;
    ensureContext(glw, ctx);
    if (!ctx.isValid())
        QSKIP("GL Implementation not valid");

    QGLWindowSurface surface1(&glw);

    QGLSubsurface surface2(&surface1, QRect(0, 0, 32, 16));
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface2.surface() == &surface1);
    QCOMPARE(surface2.region(), QRect(0, 0, 32, 16));

    QVERIFY(surface2.activate());
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QCOMPARE(surface2.viewportGL(), QRect(0, glw.height() - 16, 32, 16));

    QGLSubsurface surface3(&surface1, QRect(16, 8, 32, 16));
    QVERIFY(surface3.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface3.surface() == &surface1);
    QCOMPARE(surface3.region(), QRect(16, 8, 32, 16));

    QVERIFY(surface2.switchTo(&surface3));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QCOMPARE(surface3.viewportGL(), QRect(16, glw.height() - (8 + 16), 32, 16));

    QGLSubsurface surface4;
    QVERIFY(surface4.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface4.surface() == 0);
    QVERIFY(surface4.region().isNull());
    QVERIFY(!surface4.activate(0));
    surface4.setRegion(QRect(4, 6, 12, 8));
    QCOMPARE(surface4.viewportGL(), QRect(4, 0, 12, 8));
    surface4.setSurface(&surface3);
    QVERIFY(surface4.surface() == &surface3);
    QCOMPARE(surface4.region(), QRect(4, 6, 12, 8));

    QVERIFY(surface2.switchTo(&surface4));
    QVERIFY(QOpenGLContext::currentContext() == &ctx);
    QCOMPARE(surface4.viewportGL(), QRect(16 + 4, glw.height() - (6 + 8 + 8), 12, 8));
}

QTEST_MAIN(tst_QGLAbstractSurface)

#include "tst_qglabstractsurface.moc"
