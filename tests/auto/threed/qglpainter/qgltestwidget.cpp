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
#include "qgltestwidget.h"
#include "compareimage.h"
#include "qglframebufferobjectsurface.h"

#include <QtCore/qeventloop.h>
#include <QtCore/qtimer.h>
#include <QtGui/qpainter.h>

QGLTestWidget::QGLTestWidget(QWindow *parent)
    : QGLView(parent)
{
    eventLoop = 0;
    paintDone = false;
    target = 0;
    method = 0;
}

QGLTestWidget::~QGLTestWidget()
{
    delete eventLoop;
}

bool QGLTestWidget::runTest(QObject *target, const char *method, int timeout)
{
    if (eventLoop)
        delete eventLoop;
    paintDone = false;
    capture = QImage();
    simulated = QImage();
    this->target = target;
    this->method = method;
    eventLoop = new QEventLoop(this);
    QTimer::singleShot(timeout, eventLoop, SLOT(quit()));
    if (!isVisible()) {
        show();
        QTest::qWaitForWindowExposed(this);
    } else {
        update();
    }
    if (!paintDone)
        eventLoop->exec();
    delete eventLoop;
    eventLoop = 0;
    this->target = 0;
    this->method = 0;
    if (!paintDone) {
        qWarning("timed out");
        return false;
    }
    if (!qFuzzyCompare(capture, simulated)) {
        qWarning("output is not as expected");
        return false;
    }
    return true;
}

void QGLTestWidget::paintGL()
{
    if (target && method)
        QMetaObject::invokeMethod(target, method, Qt::DirectConnection);
    QOpenGLFramebufferObject fbo(512, 512);
    QGLFramebufferObjectSurface surf(&fbo);

    capture = fbo.toImage();

    if (target && method) {
        QSize sz = size();
        simulated = QImage(sz, QImage::Format_RGB32);
        QPainter painter;
        painter.begin(&simulated);
        QByteArray meth(method);
        meth.append('Q');
        QMetaObject::invokeMethod
            (target, meth.constData(), Qt::DirectConnection,
             Q_ARG(QPainter *, &painter), Q_ARG(const QSize, sz));
        painter.end();
    }
    if (!paintDone) {
        paintDone = true;
        if (eventLoop)
            eventLoop->quit();
    }
}
