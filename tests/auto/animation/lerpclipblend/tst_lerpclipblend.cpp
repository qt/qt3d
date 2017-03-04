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
#include <Qt3DAnimation/qlerpclipblend.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qlerpclipblend_p.h>
#include <Qt3DAnimation/private/lerpclipblend_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "qbackendnodetester.h"

class tst_LerpClipBlend : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DAnimation::Animation::LerpClipBlend backendLerpBlend;

        // THEN
        QCOMPARE(backendLerpBlend.isEnabled(), false);
        QVERIFY(backendLerpBlend.peerId().isNull());
        QCOMPARE(backendLerpBlend.blendFactor(), 0.0f);
        QCOMPARE(backendLerpBlend.blendType(), Qt3DAnimation::Animation::ClipBlendNode::LerpBlendType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QLerpClipBlend lerpBlend;
        Qt3DAnimation::QAnimationClipLoader clip;
        lerpBlend.setBlendFactor(0.8f);
        lerpBlend.addClip(&clip);

        {
            // WHEN
            Qt3DAnimation::Animation::LerpClipBlend backendLerpBlend;
            simulateInitialization(&lerpBlend, &backendLerpBlend);

            // THEN
            QCOMPARE(backendLerpBlend.isEnabled(), true);
            QCOMPARE(backendLerpBlend.peerId(), lerpBlend.id());
            QCOMPARE(backendLerpBlend.blendFactor(), 0.8f);
            QCOMPARE(backendLerpBlend.clipIds().size(), 1);
            QCOMPARE(backendLerpBlend.clipIds().first(), clip.id());
        }
        {
            // WHEN
            Qt3DAnimation::Animation::LerpClipBlend backendLerpBlend;
            lerpBlend.setEnabled(false);
            simulateInitialization(&lerpBlend, &backendLerpBlend);

            // THEN
            QCOMPARE(backendLerpBlend.peerId(), lerpBlend.id());
            QCOMPARE(backendLerpBlend.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DAnimation::Animation::LerpClipBlend backendLerpBlend;
        {
             // WHEN
             const bool newValue = false;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("enabled");
             change->setValue(newValue);
             backendLerpBlend.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendLerpBlend.isEnabled(), newValue);
        }
        {
             // WHEN
             const float newValue = 0.883f;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("blendFactor");
             change->setValue(QVariant::fromValue(newValue));
             backendLerpBlend.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendLerpBlend.blendFactor(), newValue);
        }
    }

    void checkBlend_data()
    {
        QTest::addColumn<float>("value1");
        QTest::addColumn<float>("value2");
        QTest::addColumn<float>("blendFactor");
        QTest::addColumn<float>("result");

        QTest::newRow("0_blending") << 8.0f << 5.0f << 0.0f << 8.0f;
        QTest::newRow("0.5_blending") << 8.0f << 5.0f << 0.5f << 6.5f;
        QTest::newRow("1_blending") << 8.0f << 5.0f << 1.0f << 5.0f;
    }

    void checkBlend()
    {
        // GIVEN
        QFETCH(float, value1);
        QFETCH(float, value2);
        QFETCH(float, blendFactor);
        QFETCH(float, result);
        Qt3DAnimation::Animation::LerpClipBlend lerpBlend;

        // WHEN
        lerpBlend.setBlendFactor(blendFactor);
        const float computed = lerpBlend.blend(value1, value2);

        // THEN
        QCOMPARE(computed, result);
    }

    void checkDependencyIds()
    {
        // GIVEN
        Qt3DAnimation::Animation::LerpClipBlend lerpBlend;
        auto startClipId = Qt3DCore::QNodeId::createId();
        auto endClipId = Qt3DCore::QNodeId::createId();

        // WHEN
        lerpBlend.setStartClipId(startClipId);
        lerpBlend.setEndClipId(endClipId);
        QVector<Qt3DCore::QNodeId> actualIds = lerpBlend.dependencyIds();

        // THEN
        QCOMPARE(actualIds.size(), 2);
        QCOMPARE(actualIds[0], startClipId);
        QCOMPARE(actualIds[1], endClipId);

        // WHEN
        auto anotherEndClipId = Qt3DCore::QNodeId::createId();
        lerpBlend.setEndClipId(anotherEndClipId);
        actualIds = lerpBlend.dependencyIds();

        // THEN
        QCOMPARE(actualIds.size(), 2);
        QCOMPARE(actualIds[0], startClipId);
        QCOMPARE(actualIds[1], anotherEndClipId);
    }
};

QTEST_MAIN(tst_LerpClipBlend)

#include "tst_lerpclipblend.moc"
