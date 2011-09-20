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

#include <QtTest/QtTest>
#include "qglsharedresource_p.h"

class tst_QGLSharedResource : public QObject
{
Q_OBJECT

public:
    tst_QGLSharedResource() {}
    ~tst_QGLSharedResource() {}

private slots:
    void resourceSharing();
};

static int dummyDestroyCount = 0;

static void destroyDummyResource(GLuint id)
{
    QVERIFY(id != 0);
    QVERIFY(QGLContext::currentContext() != 0);
    ++dummyDestroyCount;
}

void tst_QGLSharedResource::resourceSharing()
{
    QSKIP("failing since refactor changes", SkipAll);

    dummyDestroyCount = 0;

    // Create a context.
    QGLWidget *glw1 = new QGLWidget();
    glw1->makeCurrent();

    // Nothing should be sharing with glw1's context yet.
    QVERIFY(!glw1->isSharing());

    // Create a guard for the first context.
    QGLSharedResource guard(destroyDummyResource);
    QVERIFY(guard.context() == 0);
    QVERIFY(guard.id() == 0);
    guard.attach(glw1->context(), 3);
    QVERIFY(guard.context() == glw1->context());
    QVERIFY(guard.id() == 3);

    // Create another context that shares with the first.
    QVERIFY(!glw1->isSharing());
    QGLWidget *glw2 = new QGLWidget(0, glw1);
    if (!glw2->isSharing()) {
        delete glw2;
        delete glw1;
        QSKIP("Context sharing is not supported", SkipSingle);
    }
    QVERIFY(glw1->isSharing());
    QVERIFY(glw1->context() != glw2->context());

    // Guard should still be the same.
    QVERIFY(guard.context() == glw1->context());
    QVERIFY(guard.id() == 3);

    // Create some guards and then destroy them while the context is active.
    {
        QGLSharedResource guard2(destroyDummyResource);
        guard2.attach(glw1->context(), 4);
        QVERIFY(guard2.context() == glw1->context());
        QVERIFY(guard2.id() == 4);
    }
    QCOMPARE(dummyDestroyCount, 1);
    QGLSharedResource guard3(destroyDummyResource);
    guard3.attach(glw1->context(), 6);
    QVERIFY(guard3.context() == glw1->context());
    QVERIFY(guard3.id() == 6);
    guard3.destroy();
    QVERIFY(guard3.context() == 0);
    QVERIFY(guard3.id() == 0);
    QCOMPARE(dummyDestroyCount, 2);

    // Create a third context, not sharing with the others.
    QGLWidget *glw3 = new QGLWidget();
    QVERIFY(!glw3->isSharing());

    // Create a guard on the standalone context.
    QGLSharedResource guard4(destroyDummyResource);
    guard4.attach(glw3->context(), 5);
    QVERIFY(guard4.context() == glw3->context());
    QVERIFY(guard4.id() == 5);

    // Shared guard should still be the same.
    QVERIFY(guard.context() == glw1->context());
    QVERIFY(guard.id() == 3);

    // Delete the first context.
    delete glw1;

    // The second context should no longer register as sharing.
    QVERIFY(!glw2->isSharing());

    // Shared guard should now be the second context, with the id the same.
    QVERIFY(guard.context() == glw2->context());
    QVERIFY(guard.id() == 3);
    QVERIFY(guard4.context() == glw3->context());
    QVERIFY(guard4.id() == 5);

    // Clean up the other contexts.
    delete glw2;
    delete glw3;

    // Guards should now be null and the id zero, but no extra calls to
    // the dummy destroy function.
    QVERIFY(guard.context() == 0);
    QVERIFY(guard.id() == 0);
    QVERIFY(guard4.context() == 0);
    QVERIFY(guard4.id() == 0);
    QCOMPARE(dummyDestroyCount, 2);
}

QTEST_MAIN(tst_QGLSharedResource)

#include "tst_qglsharedresource.moc"
