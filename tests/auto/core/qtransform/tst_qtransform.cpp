/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qcomponent.h>
#include <QtCore/qscopedpointer.h>
#include "testpostmanarbiter.h"

using namespace Qt3DCore;

class tst_QTransform : public QNode
{
    Q_OBJECT
public:
    ~tst_QTransform()
    {
        QNode::cleanup();
    }

private Q_SLOTS:
    void checkCloning_data()
    {
        QTest::addColumn<Qt3DCore::QTransform *>("transform");

        Qt3DCore::QTransform *defaultConstructed = new Qt3DCore::QTransform();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DCore::QTransform *matrixPropertySet = new Qt3DCore::QTransform();
        matrixPropertySet->setMatrix(Qt3DCore::QTransform::rotateAround(QVector3D(0.1877, 0.6868, 0.3884), 45.0, QVector3D(0, 0, 1)));
        QTest::newRow("matrixPropertySet") << matrixPropertySet;

        Qt3DCore::QTransform *translationSet = new Qt3DCore::QTransform();
        translationSet->setTranslation(QVector3D(0.1877, 0.6868, 0.3884));
        QTest::newRow("translationSet") << translationSet;

        Qt3DCore::QTransform *scaleSet = new Qt3DCore::QTransform();
        scaleSet->setScale3D(QVector3D(0.1, 0.6, 0.3));
        QTest::newRow("scaleSet") << scaleSet;

        Qt3DCore::QTransform *rotationSet = new Qt3DCore::QTransform();
        scaleSet->setRotation(Qt3DCore::QTransform::fromAxisAndAngle(0.0f, 0.0f, 1.0f, 30.0f));
        QTest::newRow("rotationSet") << rotationSet;

        Qt3DCore::QTransform *eulerRotationSet = new Qt3DCore::QTransform();
        eulerRotationSet->setRotationX(90.0f);
        eulerRotationSet->setRotationY(10.0f);
        eulerRotationSet->setRotationZ(1.0f);
        QTest::newRow("eulerRotationSet") << eulerRotationSet;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DCore::QTransform *, transform);

        // WHEN
        Qt3DCore::QTransform *clone = static_cast<Qt3DCore::QTransform *>(QNode::clone(transform));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(transform->id(), clone->id());
        QCOMPARE(transform->matrix(), clone->matrix());
        QCOMPARE(transform->translation(), clone->translation());
        QCOMPARE(transform->scale3D(), clone->scale3D());
        QCOMPARE(transform->scale(), clone->scale());
        QCOMPARE(transform->rotation(), clone->rotation());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QTransform> transform(new Qt3DCore::QTransform());
        TestArbiter arbiter(transform.data());

        // WHEN
        transform->setTranslation(QVector3D(454.0f, 427.0f, 383.0f));
        QCoreApplication::processEvents();

        // THEN
        Qt3DCore::QScenePropertyChangePtr change;
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "translation");
        QCOMPARE(change->value().value<QVector3D>(), QVector3D(454.0f, 427.0f, 383.0f));

        arbiter.events.clear();

        // WHEN
        QQuaternion q = Qt3DCore::QTransform::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), 90.0f);
        transform->setRotation(q);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "rotation");
        QCOMPARE(change->value().value<QQuaternion>(), q);

        arbiter.events.clear();

        // WHEN
        transform->setScale3D(QVector3D(883.0f, 1200.0f, 1340.0f));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "scale3D");
        QCOMPARE(change->value().value<QVector3D>(), QVector3D(883.0f, 1200.0f, 1340.0f));

        arbiter.events.clear();

        // WHEN
        // Force the transform to update its matrix
        (void)transform->matrix();

        transform->setMatrix(QMatrix4x4());
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 3);
        change = arbiter.events.takeFirst().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "scale3D");
        QCOMPARE(change->value().value<QVector3D>(), QVector3D(1.0f, 1.0f, 1.0f));
        change = arbiter.events.takeFirst().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "rotation");
        QCOMPARE(change->value().value<QQuaternion>(), QQuaternion());
        change = arbiter.events.takeFirst().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "translation");
        QCOMPARE(change->value().value<QVector3D>(), QVector3D());

        arbiter.events.clear();

        // WHEN
        transform->setRotationX(20.0f);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "rotation");
        QCOMPARE(change->value().value<QQuaternion>().toEulerAngles().x(), 20.0f);

        arbiter.events.clear();
    }

    void checkCompositionDecomposition()
    {
        // GIVEN
        Qt3DCore::QTransform t;
        Qt3DCore::QTransform t2;
        QMatrix4x4 m = Qt3DCore::QTransform::rotateAround(QVector3D(0.1877, 0.6868, 0.3884), 45.0, QVector3D(0, 0, 1));

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

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }
};

QTEST_MAIN(tst_QTransform)

#include "tst_qtransform.moc"
