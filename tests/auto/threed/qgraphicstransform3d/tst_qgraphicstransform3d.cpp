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
#include "qgraphicsrotation3d.h"
#include "qgraphicsscale3d.h"
#include "qgraphicstranslation3d.h"
#include "qgraphicsbillboardtransform.h"

class tst_QGraphicsTransform3D : public QObject
{
    Q_OBJECT
public:
    tst_QGraphicsTransform3D() {}
    ~tst_QGraphicsTransform3D() {}

private slots:
    void rotation3D();
    void scale3D();
    void translation3D();
    void billboard();
};

static bool isSameMatrix(const QMatrix4x4 &m1, const QMatrix4x4 &m2)
{
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (!qFuzzyIsNull(float(m1(row, col) - m2(row, col))))
                return false;
        }
    }
    return true;
}

void tst_QGraphicsTransform3D::rotation3D()
{
    QGraphicsRotation3D rot1;
    QVERIFY(rot1.origin() == QVector3D(0, 0, 0));
    QVERIFY(rot1.axis() == QVector3D(0, 0, 1));
    QCOMPARE(rot1.angle(), 0.0f);

    QSignalSpy spy1(&rot1, SIGNAL(originChanged()));
    QSignalSpy spy2(&rot1, SIGNAL(axisChanged()));
    QSignalSpy spy3(&rot1, SIGNAL(angleChanged()));
    QSignalSpy spy4(&rot1, SIGNAL(transformChanged()));

    rot1.setOrigin(QVector3D(1, 2, 3));
    rot1.setAxis(QVector3D(4, -5, 6));
    rot1.setAngle(45.0f);
    QVERIFY(rot1.origin() == QVector3D(1, 2, 3));
    QVERIFY(rot1.axis() == QVector3D(4, -5, 6));
    QCOMPARE(rot1.angle(), 45.0f);

    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);
    QCOMPARE(spy3.size(), 1);
    QCOMPARE(spy4.size(), 3);

    // Change to same values should not result in any extra signals.
    rot1.setOrigin(QVector3D(1, 2, 3));
    rot1.setAxis(QVector3D(4, -5, 6));
    rot1.setAngle(45.0f);
    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);
    QCOMPARE(spy3.size(), 1);
    QCOMPARE(spy4.size(), 3);

    QMatrix4x4 m1;
    rot1.applyTo(&m1);

    QMatrix4x4 m2;
    m2.translate(1, 2, 3);
    m2.rotate(45.0f, QVector3D(4, -5, 6));
    m2.translate(-1, -2, -3);
    QVERIFY(isSameMatrix(m1, m2));

    QQuickQGraphicsTransform3D *transform2 = rot1.clone(this);
    QGraphicsRotation3D *rot2 = qobject_cast<QGraphicsRotation3D *>(transform2);
    QVERIFY(rot2 != 0);
    QVERIFY(rot2 != &rot1);
    QVERIFY(rot2->parent() == this);
    QVERIFY(rot2->origin() == rot1.origin());
    QVERIFY(rot2->axis() == rot1.axis());
    QVERIFY(rot2->angle() == rot1.angle());
    delete transform2;
}

void tst_QGraphicsTransform3D::scale3D()
{
    QGraphicsScale3D scale1;
    QVERIFY(scale1.origin() == QVector3D(0, 0, 0));
    QVERIFY(scale1.scale() == QVector3D(1, 1, 1));

    QSignalSpy spy1(&scale1, SIGNAL(originChanged()));
    QSignalSpy spy2(&scale1, SIGNAL(scaleChanged()));
    QSignalSpy spy3(&scale1, SIGNAL(transformChanged()));

    scale1.setOrigin(QVector3D(1, 2, 3));
    scale1.setScale(QVector3D(4, -6, 0.5f));
    QVERIFY(scale1.origin() == QVector3D(1, 2, 3));
    QVERIFY(scale1.scale() == QVector3D(4, -6, 0.5f));

    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);
    QCOMPARE(spy3.size(), 2);

    // Change to same values should not result in any extra signals.
    scale1.setOrigin(QVector3D(1, 2, 3));
    scale1.setScale(QVector3D(4, -6, 0.5f));
    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);
    QCOMPARE(spy3.size(), 2);

    QMatrix4x4 m1;
    scale1.applyTo(&m1);

    QMatrix4x4 m2;
    m2.translate(1, 2, 3);
    m2.scale(QVector3D(4, -6, 0.5f));
    m2.translate(-1, -2, -3);
    QVERIFY(isSameMatrix(m1, m2));

    scale1.setScale(QVector3D(20, -4, 42));
    QCOMPARE(spy2.size(), 2);
    QCOMPARE(spy3.size(), 3);

    QVERIFY(scale1.scale() == QVector3D(20, -4, 42));

    QVERIFY(scale1.origin() == QVector3D(1, 2, 3));
    QCOMPARE(spy1.size(), 1);

    QQuickQGraphicsTransform3D *transform2 = scale1.clone(this);
    QGraphicsScale3D *scale2 = qobject_cast<QGraphicsScale3D *>(transform2);
    QVERIFY(scale2 != 0);
    QVERIFY(scale2 != &scale1);
    QVERIFY(scale2->parent() == this);
    QVERIFY(scale2->origin() == scale1.origin());
    QVERIFY(scale2->scale() == scale1.scale());
    delete transform2;
}

void tst_QGraphicsTransform3D::translation3D()
{
    QGraphicsTranslation3D translate1;
    QVERIFY(translate1.translate() == QVector3D(0, 0, 0));
    QCOMPARE(translate1.progress(), 1.0f);

    QSignalSpy spy1(&translate1, SIGNAL(translateChanged()));
    QSignalSpy spy2(&translate1, SIGNAL(progressChanged()));
    QSignalSpy spy3(&translate1, SIGNAL(transformChanged()));

    translate1.setTranslate(QVector3D(4, -6, 0.5f));
    translate1.setProgress(2.0f);
    QVERIFY(translate1.translate() == QVector3D(4, -6, 0.5f));
    QCOMPARE(translate1.progress(), 2.0f);

    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);
    QCOMPARE(spy3.size(), 2);

    // Change to same values should not result in any extra signals.
    translate1.setTranslate(QVector3D(4, -6, 0.5f));
    translate1.setProgress(2.0f);
    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);
    QCOMPARE(spy3.size(), 2);

    QMatrix4x4 m1;
    translate1.applyTo(&m1);

    QMatrix4x4 m2;
    m2.translate(QVector3D(8, -12, 1));
    QVERIFY(isSameMatrix(m1, m2));

    QQuickQGraphicsTransform3D *transform2 = translate1.clone(this);
    QGraphicsTranslation3D *translate2 = qobject_cast<QGraphicsTranslation3D *>(transform2);
    QVERIFY(translate2 != 0);
    QVERIFY(translate2 != &translate1);
    QVERIFY(translate2->parent() == this);
    QVERIFY(translate2->translate() == translate1.translate());
    QVERIFY(translate2->progress() == translate1.progress());
    delete transform2;
}

void tst_QGraphicsTransform3D::billboard()
{
    QGraphicsBillboardTransform billboard1;
    QVERIFY(!billboard1.preserveUpVector());

    QSignalSpy spy1(&billboard1, SIGNAL(preserveUpVectorChanged()));
    QSignalSpy spy2(&billboard1, SIGNAL(transformChanged()));

    billboard1.setPreserveUpVector(true);
    QVERIFY(billboard1.preserveUpVector());

    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);

    // Change to same value should not result in any extra signals.
    billboard1.setPreserveUpVector(true);
    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);

    QMatrix4x4 m1;
    billboard1.applyTo(&m1);
    QVERIFY(m1.isIdentity());

    QMatrix4x4 m2(2.0f, 3.0f, 4.0f, 5.0f,
                  6.0f, 7.0f, 8.0f, 9.0f,
                  10.0f, 11.0f, 12.0f, 13.0f,
                  14.0f, 15.0f, 16.0f, 17.0f);
    QMatrix4x4 m3(1.0f, 0.0f, 0.0f, 5.0f,
                  0.0f, 1.0f, 0.0f, 9.0f,
                  0.0f, 0.0f, 1.0f, 13.0f,
                  14.0f, 15.0f, 16.0f, 17.0f);
    QMatrix4x4 m4(1.0f, 3.0f, 0.0f, 5.0f,
                  0.0f, 7.0f, 0.0f, 9.0f,
                  0.0f, 11.0f, 1.0f, 13.0f,
                  14.0f, 15.0f, 16.0f, 17.0f);

    QMatrix4x4 m5(m2);
    billboard1.setPreserveUpVector(false);
    billboard1.applyTo(&m5);
    QVERIFY(m5 == m3);

    QMatrix4x4 m6(m2);
    billboard1.setPreserveUpVector(true);
    billboard1.applyTo(&m6);
    QVERIFY(m6 == m4);

    QQuickQGraphicsTransform3D *transform2 = billboard1.clone(this);
    QGraphicsBillboardTransform *billboard2 = qobject_cast<QGraphicsBillboardTransform *>(transform2);
    QVERIFY(billboard2 != 0);
    QVERIFY(billboard2 != &billboard1);
    QVERIFY(billboard2->parent() == this);
    QVERIFY(billboard2->preserveUpVector() == billboard1.preserveUpVector());
    delete transform2;
}

QTEST_APPLESS_MAIN(tst_QGraphicsTransform3D)

#include "tst_qgraphicstransform3d.moc"
