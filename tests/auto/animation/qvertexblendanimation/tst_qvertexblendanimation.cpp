// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/qtest.h>
#include <Qt3DAnimation/qvertexblendanimation.h>
#include <qobject.h>
#include <qsignalspy.h>

class tst_QVertexBlendAnimation : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QVertexBlendAnimation vertexBlendAnimation;

        // THEN
        QCOMPARE(vertexBlendAnimation.interpolator(), 0.0f);
        QCOMPARE(vertexBlendAnimation.target(), nullptr);
        QCOMPARE(vertexBlendAnimation.targetName(), QString());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QVertexBlendAnimation vertexBlendAnimation;

        {
            // WHEN
            QScopedPointer<Qt3DRender::QGeometryRenderer> gm(new Qt3DRender::QGeometryRenderer);
            QSignalSpy spy(&vertexBlendAnimation,
                           SIGNAL(targetChanged(Qt3DRender::QGeometryRenderer*)));
            Qt3DRender::QGeometryRenderer *newValue = gm.data();
            vertexBlendAnimation.setTarget(newValue);

            // THEN
            QCOMPARE(vertexBlendAnimation.target(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            vertexBlendAnimation.setTarget(newValue);

            // THEN
            QCOMPARE(vertexBlendAnimation.target(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&vertexBlendAnimation, SIGNAL(targetNameChanged(QString)));
            const QString newValue = QString("target");
            vertexBlendAnimation.setTargetName(newValue);

            // THEN
            QCOMPARE(vertexBlendAnimation.targetName(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            vertexBlendAnimation.setTargetName(newValue);

            // THEN
            QCOMPARE(vertexBlendAnimation.targetName(), newValue);
            QCOMPARE(spy.size(), 0);

        }
    }

};

QTEST_APPLESS_MAIN(tst_QVertexBlendAnimation)

#include "tst_qvertexblendanimation.moc"
