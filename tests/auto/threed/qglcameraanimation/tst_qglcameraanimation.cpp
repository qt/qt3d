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
#include "qglcameraanimation.h"
#include "qglcamera.h"

class tst_QGLCameraAnimation : public QObject
{
    Q_OBJECT
public:
    tst_QGLCameraAnimation() {}
    ~tst_QGLCameraAnimation() {}

private slots:
    void properties();
    void animationStartEnd();
};

void tst_QGLCameraAnimation::properties()
{
    // Check the default property values.
    QGLCameraAnimation anim;
    QVERIFY(anim.camera() == 0);
    QVERIFY(anim.startEye() == QVector3D(0.0f, 0.0f, 10.0f));
    QVERIFY(anim.startUpVector() == QVector3D(0.0f, 1.0f, 0.0f));
    QVERIFY(anim.startCenter() == QVector3D(0.0f, 0.0f, 0.0f));
    QVERIFY(anim.endEye() == QVector3D(0.0f, 0.0f, 10.0f));
    QVERIFY(anim.endUpVector() == QVector3D(0.0f, 1.0f, 0.0f));
    QVERIFY(anim.endCenter() == QVector3D(0.0f, 0.0f, 0.0f));
    QCOMPARE(anim.duration(), 250);
    QVERIFY(anim.easingCurve() == QEasingCurve());

    // Change the properties.
    QGLCamera camera;
    anim.setCamera(&camera);
    anim.setStartEye(QVector3D(1, -2, 3));
    anim.setStartUpVector(QVector3D(-4, 5, -6));
    anim.setStartCenter(QVector3D(7, -8, 9));
    anim.setEndEye(QVector3D(-1, 2, -3));
    anim.setEndUpVector(QVector3D(4, -5, 6));
    anim.setEndCenter(QVector3D(-7, 8, -9));
    anim.setDuration(500);
    anim.setEasingCurve(QEasingCurve::OutQuad);

    // Check that the properties have their new values.
    QVERIFY(anim.camera() == &camera);
    QVERIFY(anim.startEye() == QVector3D(1, -2, 3));
    QVERIFY(anim.startUpVector() == QVector3D(-4, 5, -6));
    QVERIFY(anim.startCenter() == QVector3D(7, -8, 9));
    QVERIFY(anim.endEye() == QVector3D(-1, 2, -3));
    QVERIFY(anim.endUpVector() == QVector3D(4, -5, 6));
    QVERIFY(anim.endCenter() == QVector3D(-7, 8, -9));
    QCOMPARE(anim.duration(), 500);
    QVERIFY(anim.easingCurve() == QEasingCurve(QEasingCurve::OutQuad));
}

void tst_QGLCameraAnimation::animationStartEnd()
{
    QGLCameraAnimation anim;
    QGLCamera camera;
    QGLCamera camera2;

    anim.setCamera(&camera);
    anim.setStartEye(QVector3D(1, -2, 3));
    anim.setStartUpVector(QVector3D(-4, 5, -6));
    anim.setStartCenter(QVector3D(7, -8, 9));
    anim.setEndEye(QVector3D(-1, 2, -3));
    anim.setEndUpVector(QVector3D(4, -5, 6));
    anim.setEndCenter(QVector3D(-7, 8, -9));
    anim.setDuration(1);

    // Verify the start state.
    anim.start();
    QVERIFY(camera.eye() == anim.startEye());
    QVERIFY(camera.upVector() == anim.startUpVector());
    QVERIFY(camera.center() == anim.startCenter());

    // Wait for the animation to end
    QTRY_COMPARE(anim.state(), QAbstractAnimation::Stopped);
    QVERIFY(camera.eye() == anim.endEye());
    QVERIFY(camera.upVector() == anim.endUpVector());
    QVERIFY(camera.center() == anim.endCenter());

    // Zero duration animation instantly transitions to the end state.
    anim.setCamera(&camera2);
    anim.setDuration(0);
    anim.start();
    QVERIFY(camera2.eye() == anim.endEye());
    QVERIFY(camera2.upVector() == anim.endUpVector());
    QVERIFY(camera2.center() == anim.endCenter());
}

QTEST_MAIN(tst_QGLCameraAnimation)

#include "tst_qglcameraanimation.moc"
