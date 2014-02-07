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

#include <QtTest/QtTest>
#include "qglabstractsurface.h"
#include "qglwindowsurface.h"
#include "qglframebufferobjectsurface.h"
#include "qglsubsurface.h"
#include "qopenglfunctions.h"
#include <qglmockview.h>
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

void tst_QGLAbstractSurface::glWindowSurface()
{
    QGLMockView glw;
    if (!glw.isValid())
        QSKIP("GL is not supported on this platform");

    QOpenGLContext *ctx = glw.context();
    QVERIFY(ctx != 0);

    QGLWindowSurface surface1;
    QVERIFY(surface1.surfaceType() == QGLAbstractSurface::Window);
    QVERIFY(surface1.window() == 0);
    surface1.setWindow(&glw);
    QCOMPARE(surface1.window(), &glw);

    QVERIFY(surface1.activate());
    QVERIFY(QOpenGLContext::currentContext() == ctx);

#ifndef Q_OS_WIN
    // TODO: these geometry tests are buggy on Windows QPA due to frame
    // geometry issues - FIXME: http://bugreports.qt-project.org/browse/QTBUG-24539
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));
#endif

    surface1.deactivate();
    QVERIFY(QOpenGLContext::currentContext() == ctx); // Left current.

    QGLMockView glw2;
    QOpenGLContext *ctx2 = glw2.context();

    // This should have been made current by the QGLMockView constructor
    QVERIFY(ctx2 != 0);
    QVERIFY(ctx2 == QOpenGLContext::currentContext());

    QGLWindowSurface surface2(&glw2);
    QVERIFY(surface2.activate());
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::Window);
    QVERIFY(surface2.window() == &glw2);

    QVERIFY(surface1.activate());
    QVERIFY(surface1.switchTo(&surface2));

    QVERIFY(QOpenGLContext::currentContext() == ctx2);

    QVERIFY(surface2.switchTo(&surface1));
    QVERIFY(QOpenGLContext::currentContext() == ctx);
}

void tst_QGLAbstractSurface::fboSurface()
{
    QGLMockView glw;
    if (!glw.isValid())
        QSKIP("GL is not supported on this platform");

    QOpenGLContext *ctx = glw.context();
    QVERIFY(ctx != 0);

    if (!QOpenGLFramebufferObject::hasOpenGLFramebufferObjects())
        QSKIP("fbo's are not supported");

    QGLWindowSurface surface1(&glw);
    QVERIFY(surface1.activate());

#ifdef Q_OS_MAC
    // The geometry tests all thru this function will fail til this is fixed
    QSKIP("Mac issue with this bug - is Y inverted on Mac?");
#endif

#ifdef Q_OS_WIN
    // The geometry tests all thru this function will fail til this is fixed
    QSKIP("Windows QPA bug http://bugreports.qt-project.org/browse/QTBUG-24539");
#endif

    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));

    QOpenGLFramebufferObject fbo1(128, 128);
    QVERIFY(fbo1.handle() != 0);

    QGLFramebufferObjectSurface surface2(&fbo1);
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::FramebufferObject);
    QVERIFY(surface2.framebufferObject() == &fbo1);

    QVERIFY(isFboCurrent(0));

    QVERIFY(surface2.activate());
    QVERIFY(QOpenGLContext::currentContext() == ctx);
    QVERIFY(isFboCurrent(&fbo1));

    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    surface2.deactivate();
    QVERIFY(isFboCurrent(0));
    QVERIFY(QOpenGLContext::currentContext() == ctx);

    surface2.setContext(ctx);

    QGLMockView glw2;
    QOpenGLContext *ctx2 = glw2.context();

    QGLWindowSurface surface3(&glw2);
    QVERIFY(surface3.activate());
    QVERIFY(QOpenGLContext::currentContext() == ctx2);
    QCOMPARE(surface3.viewportGL(), QRect(0, 0, glw2.width(), glw2.height()));

    QVERIFY(surface3.switchTo(&surface2));
    QVERIFY(isFboCurrent(&fbo1));
    QVERIFY(QOpenGLContext::currentContext() == ctx);
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    QVERIFY(surface2.switchTo(&surface3));
    QVERIFY(isFboCurrent(0));
    QVERIFY(QOpenGLContext::currentContext() == ctx2);
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
    surface4.setContext(ctx);

    QVERIFY(surface1.switchTo(&surface2));
    QCOMPARE(QOpenGLContext::currentContext(), ctx);
    QVERIFY(isFboCurrent(&fbo1));
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    QVERIFY(surface2.switchTo(&surface4));
    QVERIFY(QOpenGLContext::currentContext() == ctx);
    QVERIFY(isFboCurrent(&fbo2));
    QCOMPARE(surface4.viewportGL(), QRect(QPoint(0, 0), fbo2.size()));

    QVERIFY(surface3.switchTo(&surface2));
    QVERIFY(QOpenGLContext::currentContext() == ctx);
    QVERIFY(isFboCurrent(&fbo1));
    QCOMPARE(surface2.viewportGL(), QRect(QPoint(0, 0), fbo1.size()));

    QVERIFY(surface2.switchTo(&surface1));
    QVERIFY(QOpenGLContext::currentContext() == ctx);
    QVERIFY(isFboCurrent(0));
    QCOMPARE(surface1.viewportGL(), QRect(0, 0, glw.width(), glw.height()));
}

void tst_QGLAbstractSurface::subSurface()
{
    QGLMockView glw;
    if (!glw.isValid())
        QSKIP("GL is not supported on this platform");

    QOpenGLContext *ctx = glw.context();
    QVERIFY(ctx != 0);

    QGLWindowSurface surface1(&glw);

    QGLSubsurface surface2(&surface1, QRect(0, 0, 32, 16));
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface2.surface() == &surface1);
    QCOMPARE(surface2.region(), QRect(0, 0, 32, 16));

    QVERIFY(surface2.activate());
    QVERIFY(QOpenGLContext::currentContext() == ctx);

#ifdef Q_OS_MAC
    // The geometry tests all thru this function will fail til this is fixed
    QSKIP("Mac issue with this bug - is Y inverted on Mac?");
#endif

#ifdef Q_OS_WIN
    // The geometry tests all thru this function will fail til this is fixed
    QSKIP("Windows QPA bug http://bugreports.qt-project.org/browse/QTBUG-24539");
#endif

    QCOMPARE(surface2.viewportGL(), QRect(0, glw.height() - 16, 32, 16));

    QGLSubsurface surface3(&surface1, QRect(16, 8, 32, 16));
    QVERIFY(surface3.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface3.surface() == &surface1);
    QCOMPARE(surface3.region(), QRect(16, 8, 32, 16));

    QVERIFY(surface2.switchTo(&surface3));
    QVERIFY(QOpenGLContext::currentContext() == ctx);
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
    QVERIFY(QOpenGLContext::currentContext() == ctx);
    QCOMPARE(surface4.viewportGL(), QRect(16 + 4, glw.height() - (6 + 8 + 8), 12, 8));
}

QTEST_MAIN(tst_QGLAbstractSurface)

#include "tst_qglabstractsurface.moc"
