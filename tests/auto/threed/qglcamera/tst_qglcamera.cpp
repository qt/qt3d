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
#include "qglcamera.h"

class tst_QGLCamera : public QObject
{
    Q_OBJECT
public:
    tst_QGLCamera() {}
    ~tst_QGLCamera() {}

private slots:
    void create();
    void modify();
    void minViewSize();
    void translation_data();
    void translation();
    void rotate_data();
    void rotate();
};

void tst_QGLCamera::create()
{
    // Test that a newly created object has the correct defaults.
    QGLCamera camera;
    QVERIFY(camera.projectionType() == QGLCamera::Perspective);
    QCOMPARE(camera.fieldOfView(), 0.0f);
    QCOMPARE(camera.nearPlane(), 5.0f);
    QCOMPARE(camera.farPlane(), 1000.0f);
    QCOMPARE(camera.viewSize(), QSizeF(2.0f, 2.0f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.0001f, 0.0001f));
    QCOMPARE(camera.screenRotation(), 0);
    QVERIFY(camera.eye() == QVector3D(0, 0, 10));
    QVERIFY(camera.upVector() == QVector3D(0, 1, 0));
    QVERIFY(camera.center() == QVector3D(0, 0, 0));
    QCOMPARE(camera.eyeSeparation(), 0.0f);
    QVERIFY(camera.motionAdjustment() == QVector3D(0, 0, 1));
    QVERIFY(camera.adjustForAspectRatio());
}

void tst_QGLCamera::modify()
{
    // Test modifying each field individually.
    QGLCamera camera;
    QSignalSpy spy1(&camera, SIGNAL(projectionChanged()));
    QSignalSpy spy2(&camera, SIGNAL(viewChanged()));
    camera.setProjectionType(QGLCamera::Orthographic);
    QVERIFY(camera.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(spy1.size(), 1);
    camera.setFieldOfView(60.0f);
    QCOMPARE(camera.fieldOfView(), 60.0f);
    QCOMPARE(spy1.size(), 2);
    camera.setNearPlane(-3.0f);
    QCOMPARE(camera.nearPlane(), -3.0f);
    QCOMPARE(spy1.size(), 3);
    camera.setFarPlane(3000.0f);
    QCOMPARE(camera.farPlane(), 3000.0f);
    QCOMPARE(spy1.size(), 4);
    camera.setViewSize(QSizeF(45.0f, 25.5f));
    QCOMPARE(camera.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(spy1.size(), 5);
    camera.setMinViewSize(QSizeF(0.05f, 0.025f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.05f, 0.025f));
    QCOMPARE(spy1.size(), 6);
    camera.setScreenRotation(270);
    QCOMPARE(camera.screenRotation(), 270);
    QCOMPARE(spy1.size(), 7);
    camera.setEye(QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QCOMPARE(spy2.size(), 1);
    camera.setUpVector(QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QCOMPARE(spy2.size(), 2);
    camera.setCenter(QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(camera.center() == QVector3D(7.0f, 8.0f, 9.0f));
    QCOMPARE(spy2.size(), 3);
    camera.setEyeSeparation(3.0f);
    QCOMPARE(camera.eyeSeparation(), 3.0f);
    QCOMPARE(spy2.size(), 4);
    camera.setMotionAdjustment(QVector3D(10.0f, 11.0f, 12.0f));
    QVERIFY(camera.motionAdjustment() == QVector3D(10.0f, 11.0f, 12.0f));
    QCOMPARE(spy2.size(), 5);
    camera.setAdjustForAspectRatio(false);
    QVERIFY(!camera.adjustForAspectRatio());
    QCOMPARE(spy2.size(), 6);

    // Test that we don't get any side effects between properties.
    QVERIFY(camera.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(camera.fieldOfView(), 60.0f);
    QCOMPARE(camera.nearPlane(), -3.0f);
    QCOMPARE(camera.farPlane(), 3000.0f);
    QCOMPARE(camera.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.05f, 0.025f));
    QCOMPARE(camera.screenRotation(), 270);
    QVERIFY(camera.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera.center() == QVector3D(7.0f, 8.0f, 9.0f));
    QCOMPARE(camera.eyeSeparation(), 3.0f);
    QVERIFY(camera.motionAdjustment() == QVector3D(10.0f, 11.0f, 12.0f));
    QVERIFY(!camera.adjustForAspectRatio());

    // Test that changing to the same values does not emit any signals.
    camera.setProjectionType(QGLCamera::Orthographic);
    camera.setFieldOfView(60.0f);
    camera.setNearPlane(-3.0f);
    camera.setFarPlane(3000.0f);
    camera.setViewSize(QSizeF(45.0f, 25.5f));
    camera.setMinViewSize(QSizeF(0.05f, 0.025f));
    camera.setScreenRotation(270);
    camera.setEye(QVector3D(1.0f, 2.0f, 3.0f));
    camera.setUpVector(QVector3D(4.0f, 5.0f, 6.0f));
    camera.setCenter(QVector3D(7.0f, 8.0f, 9.0f));
    camera.setEyeSeparation(3.0f);
    camera.setMotionAdjustment(QVector3D(10.0f, 11.0f, 12.0f));
    camera.setAdjustForAspectRatio(false);
    QCOMPARE(spy1.size(), 7);
    QCOMPARE(spy2.size(), 6);
}

// Check that the minimum view size works correctly, including when
// the view size goes negative (e.g. for flipped y co-ordinates).
void tst_QGLCamera::minViewSize()
{
    QGLCamera camera;
    camera.setMinViewSize(QSizeF(0.05f, 0.05f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.05f, 0.05f));
    camera.setViewSize(QSizeF(-20.0f, -30.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-20.0f, -30.0f));
    camera.setViewSize(QSizeF(0.0f, 1.0f));
    QCOMPARE(camera.viewSize(), QSizeF(0.05f, 1.0f));
    camera.setViewSize(QSizeF(-0.01f, 1.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-0.05f, 1.0f));
    camera.setViewSize(QSizeF(-1.0f, 0.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-1.0f, 0.05f));
    camera.setViewSize(QSizeF(-1.0f, -0.01f));
    QCOMPARE(camera.viewSize(), QSizeF(-1.0f, -0.05f));
}

static bool fuzzyCompare(const QVector3D &v1, const QVector3D &v2)
{
    if (qAbs(v1.x() - v2.x()) <= 0.00001 &&
            qAbs(v1.y() - v2.y()) <= 0.00001 &&
            qAbs(v1.z() - v2.z()) <= 0.00001) {
        return true;
    }
    qWarning() << "actual:" << v1 << "expected:" << v2;
    return false;
}

static bool fuzzyCompare(const QQuaternion &q1, const QQuaternion &q2)
{
    if (qAbs(q1.x() - q2.x()) <= 0.00001 &&
            qAbs(q1.y() - q2.y()) <= 0.00001 &&
            qAbs(q1.z() - q2.z()) <= 0.00001 &&
            qAbs(q1.scalar() - q2.scalar()) <= 0.00001) {
        return true;
    }
    qWarning() << "actual:" << q1 << "expected:" << q2;
    return false;
}

void tst_QGLCamera::translation_data()
{
    QTest::addColumn<QVector3D>("eye");
    QTest::addColumn<QVector3D>("upVector");
    QTest::addColumn<QVector3D>("center");

    QTest::newRow("default")
        << QVector3D(0, 0, 10) << QVector3D(0, 1, 0) << QVector3D(0, 0, 0);
    QTest::newRow("random")
        << QVector3D(2, -3, 5) << QVector3D(1, -1, 1) << QVector3D(-1, 6, -2);
}

void tst_QGLCamera::translation()
{
    QFETCH(QVector3D, eye);
    QFETCH(QVector3D, upVector);
    QFETCH(QVector3D, center);

    QGLCamera camera;
    camera.setEye(eye);
    camera.setUpVector(upVector);
    camera.setCenter(center);

    QVector3D viewVector = center - eye;
    QVector3D sideVector = QVector3D::crossProduct(viewVector, upVector);

    QVector3D nup = upVector.normalized();
    QVector3D nview = viewVector.normalized();
    QVector3D nside = sideVector.normalized();

    QVERIFY(fuzzyCompare(camera.translation(0, 0, 0), QVector3D(0, 0, 0)));

    QVERIFY(fuzzyCompare(camera.translation(2.5f, 0, 0), 2.5f * nside));
    QVERIFY(fuzzyCompare(camera.translation(0, -1.5f, 0), -1.5f * nup));
    QVERIFY(fuzzyCompare(camera.translation(0, 0, 2.0f), 2.0f * nview));

    QVector3D t = camera.translation(2.5f, -1.5f, 2.0f);
    QVERIFY(fuzzyCompare(t, 2.5f * nside - 1.5f * nup + 2.0f * nview));

    camera.translateEye(2.5f, -1.5f, 2.0f);
    QVERIFY(fuzzyCompare(camera.eye(), eye + t));
    QVERIFY(fuzzyCompare(camera.center(), center));
    QVERIFY(fuzzyCompare(camera.upVector(), upVector));

    camera.setEye(eye);

    camera.translateCenter(2.5f, -1.5f, 2.0f);
    QVERIFY(fuzzyCompare(camera.eye(), eye));
    QVERIFY(fuzzyCompare(camera.center(), center + t));
    QVERIFY(fuzzyCompare(camera.upVector(), upVector));
}

void tst_QGLCamera::rotate_data()
{
    translation_data();
}

void tst_QGLCamera::rotate()
{
    QFETCH(QVector3D, eye);
    QFETCH(QVector3D, upVector);
    QFETCH(QVector3D, center);

    QGLCamera camera;
    camera.setEye(eye);
    camera.setUpVector(upVector);
    camera.setCenter(center);

    QVector3D viewVector = center - eye;
    QVector3D sideVector = QVector3D::crossProduct(viewVector, upVector);

    QQuaternion tilt = camera.tilt(-30.0f);
    QQuaternion pan = camera.pan(125.0f);
    QQuaternion roll = camera.roll(45.0f);

    QVERIFY(fuzzyCompare(tilt, QQuaternion::fromAxisAndAngle(sideVector, -30.0f)));
    QVERIFY(fuzzyCompare(pan, QQuaternion::fromAxisAndAngle(upVector, 125.0f)));
    QVERIFY(fuzzyCompare(roll, QQuaternion::fromAxisAndAngle(viewVector, 45.0f)));

    QQuaternion combined = tilt * pan * roll;
    camera.rotateEye(combined);

    QVERIFY(fuzzyCompare(camera.eye(), eye));
    QVERIFY(fuzzyCompare(camera.upVector(), combined.rotatedVector(upVector)));
    QVERIFY(fuzzyCompare(camera.center(), eye  + combined.rotatedVector(viewVector)));

    camera.setEye(eye);
    camera.setUpVector(upVector);
    camera.setCenter(center);

    camera.rotateCenter(combined);
    QVERIFY(fuzzyCompare(camera.eye(), center - combined.rotatedVector(viewVector)));
    QVERIFY(fuzzyCompare(camera.upVector(), combined.rotatedVector(upVector)));
    QVERIFY(fuzzyCompare(camera.center(), center));

#define TestTiltPanRoll(order, combine) \
    do { \
        combined = combine; \
        camera.setEye(eye); \
        camera.setUpVector(upVector); \
        camera.setCenter(center); \
        camera.tiltPanRollEye(-30.0f, 125.0f, 45.0f, QGLCamera::order); \
        QVERIFY(fuzzyCompare(camera.eye(), eye)); \
        QVERIFY(fuzzyCompare(camera.upVector(), combined.rotatedVector(upVector))); \
        QVERIFY(fuzzyCompare(camera.center(), eye  + combined.rotatedVector(viewVector))); \
        camera.setEye(eye); \
        camera.setUpVector(upVector); \
        camera.setCenter(center); \
        camera.tiltPanRollCenter(-30.0f, 125.0f, 45.0f, QGLCamera::order); \
        QVERIFY(fuzzyCompare(camera.eye(), center - combined.rotatedVector(viewVector))); \
        QVERIFY(fuzzyCompare(camera.upVector(), combined.rotatedVector(upVector))); \
        QVERIFY(fuzzyCompare(camera.center(), center)); \
    } while (0)

    // Quaternions are multiplied in the reverse order of applying them.
    TestTiltPanRoll(TiltPanRoll, roll * pan * tilt);
    TestTiltPanRoll(TiltRollPan, pan * roll * tilt);
    TestTiltPanRoll(PanTiltRoll, roll * tilt * pan);
    TestTiltPanRoll(PanRollTilt, tilt * roll * pan);
    TestTiltPanRoll(RollTiltPan, pan * tilt * roll);
    TestTiltPanRoll(RollPanTilt, tilt * pan * roll);
}

QTEST_APPLESS_MAIN(tst_QGLCamera)

#include "tst_qglcamera.moc"
