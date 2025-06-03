// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/qtest.h>
#include <Qt3DAnimation/qanimationcontroller.h>
#include <Qt3DAnimation/qkeyframeanimation.h>
#include <Qt3DAnimation/qmorphinganimation.h>
#include <qobject.h>
#include <qsignalspy.h>

class tst_QAnimationController : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QAnimationController animationController;

        // THEN
        QCOMPARE(animationController.activeAnimationGroup(), 0);
        QCOMPARE(animationController.position(), 0.0f);
        QCOMPARE(animationController.positionScale(), 1.0f);
        QCOMPARE(animationController.positionOffset(), 0.0f);
        QCOMPARE(animationController.entity(), nullptr);
        QCOMPARE(animationController.recursive(), true);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QAnimationController animationController;

        {
            // WHEN
            QSignalSpy spy(&animationController, SIGNAL(activeAnimationGroupChanged(int)));
            const int newValue = 1;
            animationController.setActiveAnimationGroup(newValue);

            // THEN
            QCOMPARE(animationController.activeAnimationGroup(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationController.setActiveAnimationGroup(newValue);

            // THEN
            QCOMPARE(animationController.activeAnimationGroup(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&animationController, SIGNAL(positionChanged(float)));
            const float newValue = 2.0f;
            animationController.setPosition(newValue);

            // THEN
            QCOMPARE(animationController.position(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationController.setPosition(newValue);

            // THEN
            QCOMPARE(animationController.position(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&animationController, SIGNAL(positionScaleChanged(float)));
            const float newValue = 3.0f;
            animationController.setPositionScale(newValue);

            // THEN
            QCOMPARE(animationController.positionScale(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationController.setPositionScale(newValue);

            // THEN
            QCOMPARE(animationController.positionScale(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&animationController, SIGNAL(positionOffsetChanged(float)));
            const float newValue = -1.0f;
            animationController.setPositionOffset(newValue);

            // THEN
            QCOMPARE(animationController.positionOffset(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationController.setPositionOffset(newValue);

            // THEN
            QCOMPARE(animationController.positionOffset(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity);
            QSignalSpy spy(&animationController, SIGNAL(entityChanged(Qt3DCore::QEntity*)));
            Qt3DCore::QEntity * newValue = entity.data();
            animationController.setEntity(newValue);

            // THEN
            QCOMPARE(animationController.entity(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationController.setEntity(newValue);

            // THEN
            QCOMPARE(animationController.entity(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&animationController, SIGNAL(recursiveChanged(bool)));
            const bool newValue = false;
            animationController.setRecursive(newValue);

            // THEN
            QCOMPARE(animationController.recursive(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationController.setRecursive(newValue);

            // THEN
            QCOMPARE(animationController.recursive(), newValue);
            QCOMPARE(spy.size(), 0);

        }
    }

    void testSetEntity()
    {
        // GIVEN
        Qt3DAnimation::QAnimationController animationController;
        Qt3DAnimation::QKeyframeAnimation *keyframeAnimation;
        Qt3DAnimation::QKeyframeAnimation *keyframeAnimation2;
        Qt3DAnimation::QMorphingAnimation *morphingAnimation;

        QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity);
        keyframeAnimation = new Qt3DAnimation::QKeyframeAnimation(entity.data());
        keyframeAnimation2 = new Qt3DAnimation::QKeyframeAnimation(entity.data());
        morphingAnimation = new Qt3DAnimation::QMorphingAnimation(entity.data());

        const QString animName1 = QString("animation1");
        const QString animName2 = QString("animation2");
        morphingAnimation->setAnimationName(animName1);
        keyframeAnimation->setAnimationName(animName1);
        keyframeAnimation2->setAnimationName(animName2);

        {
            // WHEN
            animationController.setEntity(entity.data());

            // THEN
            const QList<Qt3DAnimation::QAnimationGroup *> list = animationController.animationGroupList();
            QCOMPARE(list.size(), 2);

            QCOMPARE(list.at(0)->name(), animName1);
            QCOMPARE(list.at(1)->name(), animName2);

            QCOMPARE(list.at(0)->animationList().size(), 2);
            QCOMPARE(list.at(1)->animationList().size(), 1);
        }
    }

    void testSetEntityRecursive()
    {
        // GIVEN
        Qt3DAnimation::QAnimationController animationController;
        Qt3DAnimation::QKeyframeAnimation *keyframeAnimation;
        Qt3DAnimation::QMorphingAnimation *morphingAnimation;

        QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity);
        keyframeAnimation = new Qt3DAnimation::QKeyframeAnimation(entity.data());
        Qt3DCore::QEntity *childEntity = new Qt3DCore::QEntity(entity.data());
        morphingAnimation = new Qt3DAnimation::QMorphingAnimation(childEntity);

        const QString animName1 = QString("animation1");
        const QString animName2 = QString("animation2");

        keyframeAnimation->setAnimationName(animName1);
        morphingAnimation->setAnimationName(animName2);

        {
            // WHEN
            animationController.setEntity(entity.data());

            // THEN
            const QList<Qt3DAnimation::QAnimationGroup *> list = animationController.animationGroupList();
            QCOMPARE(list.size(), 2);

            QCOMPARE(list.at(0)->name(), animName1);
            QCOMPARE(list.at(1)->name(), animName2);

            QCOMPARE(list.at(0)->animationList().size(), 1);
            QCOMPARE(list.at(1)->animationList().size(), 1);

            animationController.setEntity(nullptr);
        }

        {
            // WHEN
            animationController.setRecursive(false);
            animationController.setEntity(entity.data());

            // THEN
            const QList<Qt3DAnimation::QAnimationGroup *> list = animationController.animationGroupList();
            QCOMPARE(list.size(), 1);

            QCOMPARE(list.at(0)->name(), animName1);

            QCOMPARE(list.at(0)->animationList().size(), 1);
        }
    }


    void testPropagatePosition()
    {
        // GIVEN
        Qt3DAnimation::QAnimationController animationController;
        Qt3DAnimation::QKeyframeAnimation *keyframeAnimation;

        QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity);
        keyframeAnimation = new Qt3DAnimation::QKeyframeAnimation(entity.data());

        const QString animName1 = QString("animation1");
        keyframeAnimation->setAnimationName(animName1);

        {
            // WHEN
            animationController.setEntity(entity.data());
            animationController.setPosition(2.0f);

            // THEN
            QCOMPARE(animationController.animationGroupList().at(0)->position(), 2.0f);
            QCOMPARE(keyframeAnimation->position(), 2.0f);
        }

        {
            // WHEN
            animationController.setPositionOffset(1.0);
            animationController.setPositionScale(2.0f);
            animationController.setPosition(2.0f);

            // THEN
            QCOMPARE(animationController.animationGroupList().at(0)->position(), 5.0f);
            QCOMPARE(keyframeAnimation->position(), 5.0f);
        }
    }

};

QTEST_APPLESS_MAIN(tst_QAnimationController)

#include "tst_qanimationcontroller.moc"
