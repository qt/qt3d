// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DRender/qproximityfilter.h>
#include <Qt3DRender/private/qproximityfilter_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/qentity.h>
#include "testarbiter.h"

class tst_QProximityFilter : public QObject
{
    Q_OBJECT

public:
    tst_QProximityFilter() : QObject()
    {
        qRegisterMetaType<Qt3DCore::QEntity*>();
    }

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QProximityFilter proximityFilter;

        // THEN
        QVERIFY(proximityFilter.entity() == nullptr);
        QCOMPARE(proximityFilter.distanceThreshold(), 0.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QProximityFilter proximityFilter;

        {
            // WHEN
            Qt3DCore::QEntity entity;

            QSignalSpy spy(&proximityFilter, SIGNAL(entityChanged(Qt3DCore::QEntity *)));
            Qt3DCore::QEntity *newValue = &entity;
            proximityFilter.setEntity(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(proximityFilter.entity(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            proximityFilter.setEntity(newValue);

            // THEN
            QCOMPARE(proximityFilter.entity(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&proximityFilter, SIGNAL(distanceThresholdChanged(float)));
            const float newValue = 883.0f;
            proximityFilter.setDistanceThreshold(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(proximityFilter.distanceThreshold(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            proximityFilter.setDistanceThreshold(newValue);

            // THEN
            QCOMPARE(proximityFilter.distanceThreshold(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkEntityUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QProximityFilter proximityFilter;
        arbiter.setArbiterOnNode(&proximityFilter);
        Qt3DCore::QEntity entity;

        {
            // WHEN
            proximityFilter.setEntity(&entity);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &proximityFilter);

            arbiter.clear();
        }

        {
            // WHEN
            proximityFilter.setEntity(&entity);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkDistanceThresholdUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QProximityFilter proximityFilter;
        arbiter.setArbiterOnNode(&proximityFilter);

        {
            // WHEN
            proximityFilter.setDistanceThreshold(454.0f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &proximityFilter);

            arbiter.clear();
        }

        {
            // WHEN
            proximityFilter.setDistanceThreshold(454.0f);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkEntityBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QProximityFilter> proximityFilter(new Qt3DRender::QProximityFilter);
        {
            // WHEN
            Qt3DCore::QEntity entity;
            proximityFilter->setEntity(&entity);

            // THEN
            QCOMPARE(entity.parent(), proximityFilter.data());
            QCOMPARE(proximityFilter->entity(), &entity);
        }

        // THEN (Should not crash and parameter be unset)
        QVERIFY(proximityFilter->entity() == nullptr);

        {
            // WHEN
            Qt3DRender::QProximityFilter someOtherProximityFilter;
            QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity(&someOtherProximityFilter));
            proximityFilter->setEntity(entity.data());

            // THEN
            QCOMPARE(entity->parent(), &someOtherProximityFilter);
            QCOMPARE(proximityFilter->entity(), entity.data());

            // WHEN
            proximityFilter.reset();
            entity.reset();

            // THEN Should not crash when the camera is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QProximityFilter)

#include "tst_qproximityfilter.moc"
