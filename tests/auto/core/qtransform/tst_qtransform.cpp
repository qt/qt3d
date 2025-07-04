// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/private/qtransform_p.h>

#include <QtGui/qquaternion.h>

#include <QtCore/qscopedpointer.h>

#include "testarbiter.h"

using namespace Qt3DCore;

class FakeTransform : public Qt3DCore::QTransform
{
public:
};

class tst_QTransform : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstruction()
    {
        // GIVEN
        Qt3DCore::QTransform transform;

        // THEN
        QCOMPARE(transform.isShareable(), false);
        QCOMPARE(transform.matrix(), QMatrix4x4());
        QCOMPARE(transform.worldMatrix(), QMatrix4x4());
        QCOMPARE(transform.scale(), 1.0f);
        QCOMPARE(transform.scale3D(), QVector3D(1.0f, 1.0f, 1.0f));
        QCOMPARE(transform.rotation(), QQuaternion());
        QCOMPARE(transform.rotationX(), 0.0f);
        QCOMPARE(transform.rotationY(), 0.0f);
        QCOMPARE(transform.rotationZ(), 0.0f);
        QCOMPARE(transform.translation(), QVector3D(0.0f, 0.0f, 0.0f));
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DCore::QTransform> transform(new Qt3DCore::QTransform());
        arbiter.setArbiterOnNode(transform.data());

        // WHEN
        transform->setTranslation(QVector3D(454.0f, 427.0f, 383.0f));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), transform.data());

        arbiter.clear();

        // WHEN
        QQuaternion q = Qt3DCore::QTransform::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), 90.0f);
        transform->setRotation(q);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), transform.data());

        arbiter.clear();

        // WHEN
        transform->setScale3D(QVector3D(883.0f, 1200.0f, 1340.0f));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), transform.data());

        arbiter.clear();

        // WHEN
        // Force the transform to update its matrix
        (void)transform->matrix();

        transform->setMatrix(QMatrix4x4());

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), transform.data());

        arbiter.clear();

        // WHEN
        transform->setRotationX(20.0f);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), transform.data());

        arbiter.clear();
    }

    void checkSignalEmittion()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QTransform> transform(new Qt3DCore::QTransform());

        int rotationXChangedCount = 0;
        int rotationYChangedCount = 0;
        int rotationZChangedCount = 0;
        int rotationChangedCount = 0;
        int matrixChangedCount = 0;
        int scaleChangedCount = 0;
        int scale3DChangedCount = 0;
        int translationChangedCount = 0;

        QObject::connect(transform.data(), &Qt3DCore::QTransform::rotationChanged, this, [&] { ++rotationChangedCount; });
        QObject::connect(transform.data(), &Qt3DCore::QTransform::rotationXChanged, this, [&] { ++rotationXChangedCount; });
        QObject::connect(transform.data(), &Qt3DCore::QTransform::rotationYChanged, this, [&] { ++rotationYChangedCount; });
        QObject::connect(transform.data(), &Qt3DCore::QTransform::rotationZChanged, this, [&] { ++rotationZChangedCount; });
        QObject::connect(transform.data(), &Qt3DCore::QTransform::matrixChanged, this, [&] { ++matrixChangedCount; });
        QObject::connect(transform.data(), &Qt3DCore::QTransform::scale3DChanged, this, [&] { ++scale3DChangedCount; });
        QObject::connect(transform.data(), &Qt3DCore::QTransform::scaleChanged, this, [&] { ++scaleChangedCount; });
        QObject::connect(transform.data(), &Qt3DCore::QTransform::translationChanged, this, [&] { ++translationChangedCount; });

        // WHEN
        transform->setRotationX(180.0f);

        // THEN
        QCOMPARE(rotationXChangedCount, 1);
        QCOMPARE(rotationYChangedCount, 0);
        QCOMPARE(rotationZChangedCount, 0);
        QCOMPARE(rotationChangedCount, 1);
        QCOMPARE(matrixChangedCount, 1);
        QCOMPARE(scaleChangedCount, 0);
        QCOMPARE(scale3DChangedCount, 0);
        QCOMPARE(translationChangedCount, 0);

        // WHEN
        transform->setRotationY(180.0f);

        // THEN
        QCOMPARE(rotationXChangedCount, 1);
        QCOMPARE(rotationYChangedCount, 1);
        QCOMPARE(rotationZChangedCount, 0);
        QCOMPARE(rotationChangedCount, 2);
        QCOMPARE(matrixChangedCount, 2);
        QCOMPARE(scaleChangedCount, 0);
        QCOMPARE(scale3DChangedCount, 0);
        QCOMPARE(translationChangedCount, 0);

        // WHEN
        transform->setRotationZ(180.0f);

        // THEN
        QCOMPARE(rotationXChangedCount, 1);
        QCOMPARE(rotationYChangedCount, 1);
        QCOMPARE(rotationZChangedCount, 1);
        QCOMPARE(rotationChangedCount, 3);
        QCOMPARE(matrixChangedCount, 3);
        QCOMPARE(scaleChangedCount, 0);
        QCOMPARE(scale3DChangedCount, 0);
        QCOMPARE(translationChangedCount, 0);

        // WHEN
        transform->setRotation(Qt3DCore::QTransform::fromEulerAngles(15.0f, 25.0f, 84.0f));

        // THEN
        QCOMPARE(rotationXChangedCount, 2);
        QCOMPARE(rotationYChangedCount, 2);
        QCOMPARE(rotationZChangedCount, 2);
        QCOMPARE(rotationChangedCount, 4);
        QCOMPARE(matrixChangedCount, 4);
        QCOMPARE(scaleChangedCount, 0);
        QCOMPARE(scale3DChangedCount, 0);
        QCOMPARE(translationChangedCount, 0);

        // WHEN
        transform->setMatrix(QMatrix4x4());

        // THEN
        QCOMPARE(rotationXChangedCount, 3);
        QCOMPARE(rotationYChangedCount, 3);
        QCOMPARE(rotationZChangedCount, 3);
        QCOMPARE(rotationChangedCount, 5);
        QCOMPARE(matrixChangedCount, 5);
        QCOMPARE(scaleChangedCount, 1);
        QCOMPARE(scale3DChangedCount, 1);
        QCOMPARE(translationChangedCount, 1);

        // WHEN
        transform->setScale(18.0f);

        // THEN
        QCOMPARE(rotationXChangedCount, 3);
        QCOMPARE(rotationYChangedCount, 3);
        QCOMPARE(rotationZChangedCount, 3);
        QCOMPARE(rotationChangedCount, 5);
        QCOMPARE(matrixChangedCount, 6);
        QCOMPARE(scaleChangedCount, 2);
        QCOMPARE(scale3DChangedCount, 2);
        QCOMPARE(translationChangedCount, 1);

        // WHEN
        transform->setScale3D(QVector3D(15.0f, 18.0f, 15.0f));

        // THEN
        QCOMPARE(rotationXChangedCount, 3);
        QCOMPARE(rotationYChangedCount, 3);
        QCOMPARE(rotationZChangedCount, 3);
        QCOMPARE(rotationChangedCount, 5);
        QCOMPARE(matrixChangedCount, 7);
        QCOMPARE(scaleChangedCount, 2);
        QCOMPARE(scale3DChangedCount, 3);
        QCOMPARE(translationChangedCount, 1);

        // WHEN
        transform->setTranslation(QVector3D(350.0f, 383.0f, 454.0f));

        // THEN
        QCOMPARE(rotationXChangedCount, 3);
        QCOMPARE(rotationYChangedCount, 3);
        QCOMPARE(rotationZChangedCount, 3);
        QCOMPARE(rotationChangedCount, 5);
        QCOMPARE(matrixChangedCount, 8);
        QCOMPARE(scaleChangedCount, 2);
        QCOMPARE(scale3DChangedCount, 3);
        QCOMPARE(translationChangedCount, 2);
    }

    void checkCompositionDecomposition()
    {
        // GIVEN
        Qt3DCore::QTransform t;
        Qt3DCore::QTransform t2;
        QMatrix4x4 m = Qt3DCore::QTransform::rotateAround(QVector3D(0.1877f, 0.6868f, 0.3884f), 45.0f, QVector3D(0.0f, 0.0f, 1.0f));

        // WHEN
        t.setMatrix(m);
        t2.setScale3D(t.scale3D());
        t2.setRotation(t.rotation());
        t2.setTranslation(t.translation());

        // THEN
        QCOMPARE(t.scale3D(), t2.scale3D());
        QCOMPARE(t.rotation(), t2.rotation());
        QCOMPARE(t.translation(), t2.translation());

        // Note: t.matrix() != t2.matrix() since different matrices
        // can result in the same scale, rotation, translation
    }

    void checkUpdateWorldTransform()
    {
        // GIVEN
        TestArbiter arbiter;
        FakeTransform t;
        arbiter.setArbiterOnNode(&t);

        // WHEN
        QSignalSpy spy(&t, SIGNAL(worldMatrixChanged(QMatrix4x4)));

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        QTransformPrivate *dT = static_cast<QTransformPrivate *>(QNodePrivate::get(&t));
        const QMatrix4x4 newValue(1.0f, 2.0f, 3.0f, 4.0f,
                                  5.0f, 6.0f, 7.0f, 8.0f,
                                  9.0f, 10.0f, 11.0f, 12.0f,
                                  13.0f, 14.0f, 15.0f, 16.0f);
        dT->setWorldMatrix(newValue);


        // THEN
        QCOMPARE(spy.size(), 1);
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
        QCOMPARE(t.worldMatrix(), newValue);

        // WHEN
        spy.clear();
        dT->setWorldMatrix(newValue);

        // THEN
        QCOMPARE(spy.size(), 0);
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
        QCOMPARE(t.worldMatrix(), newValue);
    }
};

QTEST_MAIN(tst_QTransform)

#include "tst_qtransform.moc"
