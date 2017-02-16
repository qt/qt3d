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
#include <Qt3DAnimation/qadditiveblend.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qadditiveblend_p.h>
#include <Qt3DAnimation/private/additiveblend_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "qbackendnodetester.h"

class tst_AdditiveBlend : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DAnimation::Animation::AdditiveBlend backendAdditiveBlend;

        // THEN
        QCOMPARE(backendAdditiveBlend.isEnabled(), false);
        QVERIFY(backendAdditiveBlend.peerId().isNull());
        QCOMPARE(backendAdditiveBlend.blendFactor(), 0.0f);
        QCOMPARE(backendAdditiveBlend.blendType(), Qt3DAnimation::Animation::ClipBlendNode::AdditiveBlendType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QAdditiveBlend additiveBlend;
        Qt3DAnimation::QAnimationClipLoader clip;
        additiveBlend.setBlendFactor(0.8f);
        additiveBlend.addClip(&clip);

        {
            // WHEN
            Qt3DAnimation::Animation::AdditiveBlend backendAdditiveBlend;
            simulateInitialization(&additiveBlend, &backendAdditiveBlend);

            // THEN
            QCOMPARE(backendAdditiveBlend.isEnabled(), true);
            QCOMPARE(backendAdditiveBlend.peerId(), additiveBlend.id());
            QCOMPARE(backendAdditiveBlend.blendFactor(), 0.8f);
            QCOMPARE(backendAdditiveBlend.clipIds().size(), 1);
            QCOMPARE(backendAdditiveBlend.clipIds().first(), clip.id());
        }
        {
            // WHEN
            Qt3DAnimation::Animation::AdditiveBlend backendAdditiveBlend;
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
        Qt3DAnimation::Animation::AdditiveBlend backendAdditiveBlend;
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
            change->setPropertyName("blendFactor");
            change->setValue(QVariant::fromValue(newValue));
            backendAdditiveBlend.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendAdditiveBlend.blendFactor(), newValue);
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
        Qt3DAnimation::Animation::AdditiveBlend addBlend;

        // WHEN
        addBlend.setBlendFactor(blendFactor);
        const float computed = addBlend.blend(value1, value2);

        // THEN
        QCOMPARE(computed, result);
    }
};

QTEST_MAIN(tst_AdditiveBlend)

#include "tst_additiveblend.moc"
