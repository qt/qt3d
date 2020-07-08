/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/qtest.h>
#include <Qt3DAnimation/qmorphinganimation.h>
#include <qobject.h>
#include <qsignalspy.h>

class tst_QMorphingAnimation : public QObject
{
    Q_OBJECT

    bool verifyAttribute(Qt3DCore::QGeometry *geometry, QString name,
                         Qt3DCore::QAttribute *attribute)
    {
        const QList<Qt3DCore::QAttribute *> attributes = geometry->attributes();
        for (const Qt3DCore::QAttribute *attr : attributes) {
            if (attr->name() == name) {
                if (attr == attribute)
                    return true;
                return false;
            }
        }
        return false;
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DAnimation::QMorphingAnimation::Method>("QMorphingAnimation::Method");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QMorphingAnimation morphingAnimation;

        // THEN
        QCOMPARE(morphingAnimation.interpolator(), 0.0f);
        QCOMPARE(morphingAnimation.target(), nullptr);
        QCOMPARE(morphingAnimation.targetName(), QString());
        QCOMPARE(morphingAnimation.method(), Qt3DAnimation::QMorphingAnimation::Relative);
        QCOMPARE(morphingAnimation.easing(), QEasingCurve());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QMorphingAnimation morphingAnimation;

        {
            // WHEN
            QScopedPointer<Qt3DRender::QGeometryRenderer> gr(new Qt3DRender::QGeometryRenderer);
            QSignalSpy spy(&morphingAnimation,
                           SIGNAL(targetChanged(Qt3DRender::QGeometryRenderer *)));
            Qt3DRender::QGeometryRenderer *newValue = gr.data();
            morphingAnimation.setTarget(newValue);

            // THEN
            QCOMPARE(morphingAnimation.target(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            morphingAnimation.setTarget(newValue);

            // THEN
            QCOMPARE(morphingAnimation.target(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&morphingAnimation, SIGNAL(targetNameChanged(QString)));
            const QString newValue = QString("target");
            morphingAnimation.setTargetName(newValue);

            // THEN
            QCOMPARE(morphingAnimation.targetName(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            morphingAnimation.setTargetName(newValue);

            // THEN
            QCOMPARE(morphingAnimation.targetName(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&morphingAnimation, SIGNAL(methodChanged(QMorphingAnimation::Method)));
            const Qt3DAnimation::QMorphingAnimation::Method newValue
                    = Qt3DAnimation::QMorphingAnimation::Normalized;
            morphingAnimation.setMethod(newValue);

            // THEN
            QCOMPARE(morphingAnimation.method(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            morphingAnimation.setMethod(newValue);

            // THEN
            QCOMPARE(morphingAnimation.method(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&morphingAnimation, SIGNAL(easingChanged(QEasingCurve)));
            const QEasingCurve newValue = QEasingCurve(QEasingCurve::InBounce);
            morphingAnimation.setEasing(newValue);

            // THEN
            QCOMPARE(morphingAnimation.easing(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            morphingAnimation.setEasing(newValue);

            // THEN
            QCOMPARE(morphingAnimation.easing(), newValue);
            QCOMPARE(spy.count(), 0);

        }
    }

    void testAnimation()
    {
        // GIVEN
        const QString baseName("position");
        const QString targetName("positionTarget");
        Qt3DAnimation::QMorphingAnimation morphingAnimation;
        Qt3DCore::QAttribute *base = new Qt3DCore::QAttribute;

        Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry;
        Qt3DAnimation::QMorphTarget *mt1 = new Qt3DAnimation::QMorphTarget(&morphingAnimation);
        Qt3DAnimation::QMorphTarget *mt2 = new Qt3DAnimation::QMorphTarget(&morphingAnimation);
        Qt3DAnimation::QMorphTarget *mt3 = new Qt3DAnimation::QMorphTarget(&morphingAnimation);
        Qt3DCore::QAttribute *a1 = new Qt3DCore::QAttribute(geometry);
        Qt3DCore::QAttribute *a2 = new Qt3DCore::QAttribute(geometry);
        Qt3DCore::QAttribute *a3 = new Qt3DCore::QAttribute(geometry);
        Qt3DRender::QGeometryRenderer gr;

        base->setName(baseName);
        geometry->addAttribute(base);
        gr.setGeometry(geometry);
        morphingAnimation.setTarget(&gr);
        a1->setName(baseName);
        a2->setName(baseName);
        a3->setName(baseName);
        mt1->addAttribute(a1);
        mt2->addAttribute(a2);
        mt3->addAttribute(a3);
        morphingAnimation.addMorphTarget(mt1);
        morphingAnimation.addMorphTarget(mt2);
        morphingAnimation.addMorphTarget(mt3);

        const QList<float> positions = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f };
        morphingAnimation.setTargetPositions(positions);

        morphingAnimation.setWeights(0, QList<float> { 1.0f, 0.0f, 0.0f });
        morphingAnimation.setWeights(1, QList<float> { 0.0f, 0.0f, 0.0f });
        morphingAnimation.setWeights(2, QList<float> { 0.0f, 1.0f, 0.0f });
        morphingAnimation.setWeights(3, QList<float> { 0.0f, 0.0f, 0.0f });
        morphingAnimation.setWeights(4, QList<float> { 0.0f, 0.0f, 1.0f });

        morphingAnimation.setMethod(Qt3DAnimation::QMorphingAnimation::Relative);
        {
            // WHEN
            morphingAnimation.setPosition(0.0f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), -1.0f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a1));

            // WHEN
            morphingAnimation.setPosition(0.5f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), -0.5f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a1));

            // WHEN
            morphingAnimation.setPosition(1.0f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), -0.0f));
            QVERIFY(verifyAttribute(geometry, baseName, base));

            // WHEN
            morphingAnimation.setPosition(1.5f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), -0.5f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a2));

            // WHEN
            morphingAnimation.setPosition(4.0f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), -1.0f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a3));
        }

        morphingAnimation.setMethod(Qt3DAnimation::QMorphingAnimation::Normalized);
        {
            // WHEN
            morphingAnimation.setPosition(0.0f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), 1.0f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a1));

            // WHEN
            morphingAnimation.setPosition(0.5f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), 0.5f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a1));

            // WHEN
            morphingAnimation.setPosition(1.0f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), 0.0f));
            QVERIFY(verifyAttribute(geometry, baseName, base));

            // WHEN
            morphingAnimation.setPosition(1.5f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), 0.5f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a2));

            // WHEN
            morphingAnimation.setPosition(4.0f);

            // THEN
            QVERIFY(qFuzzyCompare(morphingAnimation.interpolator(), 1.0f));
            QVERIFY(verifyAttribute(geometry, baseName, base));
            QVERIFY(verifyAttribute(geometry, targetName, a3));
        }
    }
};

QTEST_APPLESS_MAIN(tst_QMorphingAnimation)

#include "tst_qmorphinganimation.moc"
