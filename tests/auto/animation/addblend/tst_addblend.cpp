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
#include <Qt3DAnimation/qaddblend.h>
#include <Qt3DAnimation/qanimationclip.h>
#include <Qt3DAnimation/private/qaddblend_p.h>
#include <Qt3DAnimation/private/addblend_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "qbackendnodetester.h"

class tst_AddBlend : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DAnimation::Animation::AddBlend backendAddBlend;

        // THEN
        QCOMPARE(backendAddBlend.isEnabled(), false);
        QVERIFY(backendAddBlend.peerId().isNull());
        QCOMPARE(backendAddBlend.blendFactor(), 0.0f);
        QCOMPARE(backendAddBlend.blendType(), Qt3DAnimation::Animation::ClipBlendNode::AddBlendType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QAddBlend AddBlend;
        Qt3DAnimation::QAnimationClip clip;
        AddBlend.setBlendFactor(0.8f);
        AddBlend.addClip(&clip);

        {
            // WHEN
            Qt3DAnimation::Animation::AddBlend backendAddBlend;
            simulateInitialization(&AddBlend, &backendAddBlend);

            // THEN
            QCOMPARE(backendAddBlend.isEnabled(), true);
            QCOMPARE(backendAddBlend.peerId(), AddBlend.id());
            QCOMPARE(backendAddBlend.blendFactor(), 0.8f);
            QCOMPARE(backendAddBlend.clipIds().size(), 1);
            QCOMPARE(backendAddBlend.clipIds().first(), clip.id());
        }
        {
            // WHEN
            Qt3DAnimation::Animation::AddBlend backendAddBlend;
            AddBlend.setEnabled(false);
            simulateInitialization(&AddBlend, &backendAddBlend);

            // THEN
            QCOMPARE(backendAddBlend.peerId(), AddBlend.id());
            QCOMPARE(backendAddBlend.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DAnimation::Animation::AddBlend backendAddBlend;
        {
            // WHEN
            const bool newValue = false;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("enabled");
            change->setValue(newValue);
            backendAddBlend.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendAddBlend.isEnabled(), newValue);
        }
        {
            // WHEN
            const float newValue = 0.883f;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("blendFactor");
            change->setValue(QVariant::fromValue(newValue));
            backendAddBlend.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendAddBlend.blendFactor(), newValue);
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
        Qt3DAnimation::Animation::AddBlend addBlend;

        // WHEN
        addBlend.setBlendFactor(blendFactor);
        const float computed = addBlend.blend(value1, value2);

        // THEN
        QCOMPARE(computed, result);
    }
};

QTEST_MAIN(tst_AddBlend)

#include "tst_addblend.moc"
