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

using namespace Qt3DAnimation::Animation;

namespace {

class TestClipBlendNode : public ClipBlendNode
{
public:
    TestClipBlendNode(double duration)
        : ClipBlendNode(ClipBlendNode::LerpBlendType)
        , m_duration(duration)
    {}

    float blend(float, float) const Q_DECL_FINAL { return 0.0f; }

    QVector<Qt3DCore::QNodeId> dependencyIds() const Q_DECL_FINAL
    {
        return QVector<Qt3DCore::QNodeId>();
    }

    using ClipBlendNode::setClipResults;

    double duration() const Q_DECL_FINAL { return m_duration; }

    double m_duration;
};

} // anonymous

class tst_AdditiveClipBlend : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
public:
    TestClipBlendNode *createTestBlendNode(Handler *handler,
                                           double duration)
    {
        auto id = Qt3DCore::QNodeId::createId();
        TestClipBlendNode *node = new TestClipBlendNode(duration);
        setPeerId(node, id);
        handler->clipBlendNodeManager()->appendNode(id, node);
        return node;
    }

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        AdditiveClipBlend backendAdditiveBlend;

        // THEN
        QCOMPARE(backendAdditiveBlend.isEnabled(), false);
        QVERIFY(backendAdditiveBlend.peerId().isNull());
        QCOMPARE(backendAdditiveBlend.baseClipId(), Qt3DCore::QNodeId());
        QCOMPARE(backendAdditiveBlend.additiveClipId(), Qt3DCore::QNodeId());
        QCOMPARE(backendAdditiveBlend.additiveFactor(), 0.0f);
        QCOMPARE(backendAdditiveBlend.blendType(), ClipBlendNode::AdditiveBlendType);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DAnimation::QAdditiveClipBlend additiveBlend;
        Qt3DAnimation::QAdditiveClipBlend baseClip;
        Qt3DAnimation::QAdditiveClipBlend additiveClip;
        Qt3DAnimation::QAnimationClipLoader clip;
        additiveBlend.setBaseClip(&baseClip);
        additiveBlend.setAdditiveClip(&additiveClip);
        additiveBlend.setAdditiveFactor(0.8f);
        additiveBlend.addClip(&clip);

        {
            // WHEN
            AdditiveClipBlend backendAdditiveBlend;
            simulateInitialization(&additiveBlend, &backendAdditiveBlend);

            // THEN
            QCOMPARE(backendAdditiveBlend.isEnabled(), true);
            QCOMPARE(backendAdditiveBlend.peerId(), additiveBlend.id());
            QCOMPARE(backendAdditiveBlend.baseClipId(), baseClip.id());
            QCOMPARE(backendAdditiveBlend.additiveClipId(), additiveClip.id());
            QCOMPARE(backendAdditiveBlend.additiveFactor(), 0.8f);
            QCOMPARE(backendAdditiveBlend.clipIds().size(), 1);
            QCOMPARE(backendAdditiveBlend.clipIds().first(), clip.id());
        }
        {
            // WHEN
            AdditiveClipBlend backendAdditiveBlend;
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
        AdditiveClipBlend backendAdditiveBlend;
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
        {
            // WHEN
            const Qt3DAnimation::QAdditiveClipBlend newValue;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("baseClip");
            change->setValue(QVariant::fromValue(newValue.id()));
            backendAdditiveBlend.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendAdditiveBlend.baseClipId(), newValue.id());
        }
        {
            // WHEN
            const Qt3DAnimation::QAdditiveClipBlend newValue;
            const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            change->setPropertyName("additiveClip");
            change->setValue(QVariant::fromValue(newValue.id()));
            backendAdditiveBlend.sceneChangeEvent(change);

            // THEN
            QCOMPARE(backendAdditiveBlend.additiveClipId(), newValue.id());
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
        AdditiveClipBlend addBlend;

        // WHEN
        addBlend.setAdditiveFactor(blendFactor);
        const float computed = addBlend.blend(value1, value2);

        // THEN
        QCOMPARE(computed, result);
    }

    void checkDependencyIds()
    {
        // GIVEN
        AdditiveClipBlend addBlend;
        auto baseClipId = Qt3DCore::QNodeId::createId();
        auto additiveClipId = Qt3DCore::QNodeId::createId();

        // WHEN
        addBlend.setBaseClipId(baseClipId);
        addBlend.setAdditiveClipId(additiveClipId);
        QVector<Qt3DCore::QNodeId> actualIds = addBlend.dependencyIds();

        // THEN
        QCOMPARE(actualIds.size(), 2);
        QCOMPARE(actualIds[0], baseClipId);
        QCOMPARE(actualIds[1], additiveClipId);

        // WHEN
        auto anotherAdditiveClipId = Qt3DCore::QNodeId::createId();
        addBlend.setAdditiveClipId(anotherAdditiveClipId);
        actualIds = addBlend.dependencyIds();

        // THEN
        QCOMPARE(actualIds.size(), 2);
        QCOMPARE(actualIds[0], baseClipId);
        QCOMPARE(actualIds[1], anotherAdditiveClipId);
    }

    void checkDuration()
    {
        // GIVEN
        auto handler = new Handler();
        const double expectedDuration = 123.5;
        const double baseNodeDuration = expectedDuration;
        const double additiveNodeDuration = 5.0;

        auto baseNode = createTestBlendNode(handler, baseNodeDuration);
        auto additiveNode = createTestBlendNode(handler, additiveNodeDuration);

        AdditiveClipBlend blendNode;
        blendNode.setHandler(handler);
        blendNode.setClipBlendNodeManager(handler->clipBlendNodeManager());
        blendNode.setBaseClipId(baseNode->peerId());
        blendNode.setAdditiveClipId(additiveNode->peerId());

        // WHEN
        double actualDuration = blendNode.duration();

        // THEN
        QCOMPARE(actualDuration, expectedDuration);
    }
};

QTEST_MAIN(tst_AdditiveClipBlend)

#include "tst_additiveclipblend.moc"
