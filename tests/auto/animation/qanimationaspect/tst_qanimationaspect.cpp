// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>

#include <Qt3DAnimation/qanimationaspect.h>
#include <Qt3DCore/qaspectengine.h>

class tst_QAnimationAspect: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void checkConstruction()
    {
        // WHEN
        Qt3DAnimation::QAnimationAspect animationAspect;

        // THEN
        QVERIFY(animationAspect.parent() == nullptr);
        QCOMPARE(animationAspect.objectName(), QLatin1String("Animation Aspect"));
    }

    void checkRegistration()
    {
        {
            // GIVEN
            Qt3DCore::QAspectEngine engine;

            // WHEN
            auto animationAspect = new Qt3DAnimation::QAnimationAspect;
            engine.registerAspect(animationAspect);

            // THEN
            QCOMPARE(engine.aspects().size(), 1);
            QCOMPARE(engine.aspects().first(), animationAspect);

            // WHEN
            engine.unregisterAspect(animationAspect);

            // THEN
            QCOMPARE(engine.aspects().size(), 0);
        }

        {
            // GIVEN
            Qt3DCore::QAspectEngine engine;

            // WHEN
            engine.registerAspect(QLatin1String("animation"));

            // THEN
            QCOMPARE(engine.aspects().size(), 1);
            QCOMPARE(engine.aspects().first()->objectName(), QLatin1String("Animation Aspect"));

            // WHEN
            engine.unregisterAspect(QLatin1String("animation"));

            // THEN
            QCOMPARE(engine.aspects().size(), 0);
        }
    }
};

QTEST_MAIN(tst_QAnimationAspect)

#include "tst_qanimationaspect.moc"
