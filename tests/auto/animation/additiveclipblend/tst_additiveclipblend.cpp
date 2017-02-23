/****************************************************************************
**
** Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
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


#include <QtTest/QTest>
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qadditiveclipblend_p.h>
#include <Qt3DAnimation/private/additiveclipblend_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "qbackendnodetester.h"

class tst_AdditiveClipBlend : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DAnimation::Animation::AdditiveClipBlend backendAdditiveBlend;

        // THEN
        QCOMPARE(backendAdditiveBlend.isEnabled(), false);
        QVERIFY(backendAdditiveBlend.peerId().isNull());
        QCOMPARE(backendAdditiveBlend.additiveFactor(), 0.0f);
        QCOMPARE(backendAdditiveBlend.blendType(), Qt3DAnimation::Animation::ClipBlendNode::AdditiveBlendType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QAdditiveClipBlend additiveBlend;
        Qt3DAnimation::QAnimationClipLoader clip;
        additiveBlend.setAdditiveFactor(0.8f);
        additiveBlend.addClip(&clip);

        {
            // WHEN
            Qt3DAnimation::Animation::AdditiveClipBlend backendAdditiveBlend;
            simulateInitialization(&additiveBlend, &backendAdditiveBlend);

            // THEN
            QCOMPARE(backendAdditiveBlend.isEnabled(), true);
            QCOMPARE(backendAdditiveBlend.peerId(), additiveBlend.id());
            QCOMPARE(backendAdditiveBlend.additiveFactor(), 0.8f);
            QCOMPARE(backendAdditiveBlend.clipIds().size(), 1);
            QCOMPARE(backendAdditiveBlend.clipIds().first(), clip.id());
        }
        {
            // WHEN
            Qt3DAnimation::Animation::AdditiveClipBlend backendAdditiveBlend;
            additiveBlend.setEnabled(false);
            simulateInitialization(&additiveBlend, &backendAdditiveBlend);

            // THEN
            QCOMPARE(backendAdditiveBlend.peerId(), additiveBlend.id());
            QCOMPARE(backendAdditiveBlend.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DAnimation::Animation::AdditiveClipBlend backendAdditiveBlend;
        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendAdditiveBlend.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendAdditiveBlend.isEnabled(), newValue);
        }
        {
            // WHEN
            const float newValue = 0.883f;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("additiveFactor");
            change->setValue(QVariant::fromValue(newValue));
            backendAdditiveBlend.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendAdditiveBlend.additiveFactor(), newValue);
        }
    }

    void checkBlend_data()
    {
        QTest::addColumn<float>("value1");
        QTest::addColumn<float>("value2");
        QTest::addColumn<float>("blendFactor");
        QTest::addColumn<float>("result");

        QTest::newRow("0_blending") << 8.0f << 5.0f << 0.0f << 8.0f;
        QTest::newRow("0.5_blending") << 8.0f << 5.0f << 0.5f << 10.5f;
        QTest::newRow("1_blending") << 8.0f << 5.0f << 1.0f << 13.0f;
    }

    void checkBlend()
    {
        // GIVEN
        QFETCH(float, value1);
        QFETCH(float, value2);
        QFETCH(float, blendFactor);
        QFETCH(float, result);
        Qt3DAnimation::Animation::AdditiveClipBlend addBlend;

        // WHEN
        addBlend.setAdditiveFactor(blendFactor);
        const float computed = addBlend.blend(value1, value2);

        // THEN
        QCOMPARE(computed, result);
    }
};

QTEST_MAIN(tst_AdditiveClipBlend)

#include "tst_additiveclipblend.moc"
