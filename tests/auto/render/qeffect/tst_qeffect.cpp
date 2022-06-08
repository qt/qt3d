// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/private/qeffect_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QEffect : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QEffect effect;

        // THEN
        QCOMPARE(effect.parameters().size(), 0);
        QCOMPARE(effect.techniques().size(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QEffect effect;

        {
            // WHEN
            Qt3DRender::QParameter newValue;
            effect.addParameter(&newValue);

            // THEN
            QCOMPARE(effect.parameters().size(), 1);

            // WHEN
            effect.addParameter(&newValue);

            // THEN
            QCOMPARE(effect.parameters().size(), 1);

            // WHEN
            effect.removeParameter(&newValue);

            // THEN
            QCOMPARE(effect.parameters().size(), 0);
        }
        {
            // WHEN
            Qt3DRender::QTechnique newValue;
            effect.addTechnique(&newValue);

            // THEN
            QCOMPARE(effect.techniques().size(), 1);

            // WHEN
            effect.addTechnique(&newValue);

            // THEN
            QCOMPARE(effect.techniques().size(), 1);

            // WHEN
            effect.removeTechnique(&newValue);

            // THEN
            QCOMPARE(effect.techniques().size(), 0);
        }
    }

    void checkParameterBookkeeping()
    {
        // GIVEN
        Qt3DRender::QEffect effect;

        {
            // WHEN
            Qt3DRender::QParameter parameter;
            effect.addParameter(&parameter);

            QCOMPARE(effect.parameters().size(), 1);

        }

        // THEN -> should not crash
        QCOMPARE(effect.parameters().size(), 0);

    }

    void checkTechniqueBookkeeping()
    {
        // GIVEN
        Qt3DRender::QEffect effect;

        {
            // WHEN
            Qt3DRender::QTechnique technique;
            effect.addTechnique(&technique);

            QCOMPARE(effect.techniques().size(), 1);

        }

        // THEN -> should not crash
        QCOMPARE(effect.techniques().size(), 0);

    }

    void checkParameterUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QEffect effect;
        Qt3DRender::QParameter parameter;
        arbiter.setArbiterOnNode(&effect);

        {
            // WHEN
            effect.addParameter(&parameter);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &effect);
            QVERIFY(effect.parameters().contains(&parameter));
            arbiter.clear();
        }

        {
            // WHEN
            effect.removeParameter(&parameter);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &effect);
            QVERIFY(!effect.parameters().contains(&parameter));
            arbiter.clear();
        }

    }

    void checkTechniqueUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QEffect effect;
        Qt3DRender::QTechnique technique;
        arbiter.setArbiterOnNode(&effect);

        {
            // WHEN
            effect.addTechnique(&technique);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &effect);
            arbiter.clear();
        }

        {
            // WHEN
            effect.removeTechnique(&technique);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &effect);
            arbiter.clear();
        }

    }

};

QTEST_MAIN(tst_QEffect)

#include "tst_qeffect.moc"
