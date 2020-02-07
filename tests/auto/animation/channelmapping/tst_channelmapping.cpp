/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
#include <qbackendnodetester.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/channelmapping_p.h>
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/qskeletonmapping.h>
#include <Qt3DAnimation/private/qchannelmapping_p.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qskeleton.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testpostmanarbiter.h"

class tst_TargetEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(QVector2D foo MEMBER m_foo NOTIFY fooChanged)

signals:
    void fooChanged();

private:
    QVector2D m_foo;
};

class tst_ChannelMapping : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DAnimation::Animation::Handler handler;
        Qt3DAnimation::Animation::ChannelMapping backendMapping;
        backendMapping.setHandler(&handler);
        Qt3DAnimation::QChannelMapping mapping;
        auto target = new tst_TargetEntity;

        mapping.setChannelName(QLatin1String("Location"));
        mapping.setTarget(target);
        mapping.setProperty(QLatin1String("foo"));

        // WHEN
        simulateInitializationSync(&mapping, &backendMapping);

        // THEN
        QCOMPARE(backendMapping.peerId(), mapping.id());
        QCOMPARE(backendMapping.isEnabled(), mapping.isEnabled());
        QCOMPARE(backendMapping.channelName(), mapping.channelName());
        QCOMPARE(backendMapping.targetId(), mapping.target()->id());
        QVERIFY(qstrcmp(backendMapping.propertyName(), mapping.property().toLatin1().constData()) == 0);
        QVERIFY(qstrcmp(backendMapping.propertyName(), "foo") == 0);
        QCOMPARE(backendMapping.componentCount(), 2);
        QCOMPARE(backendMapping.type(), static_cast<int>(QVariant::Vector2D));
        QCOMPARE(backendMapping.mappingType(), Qt3DAnimation::Animation::ChannelMapping::ChannelMappingType);

        // GIVEN
        Qt3DAnimation::Animation::ChannelMapping backendSkeletonMapping;
        backendSkeletonMapping.setHandler(&handler);
        Qt3DAnimation::QSkeletonMapping skeletonMapping;
        auto skeleton = new Qt3DCore::QSkeleton;
        skeletonMapping.setSkeleton(skeleton);

        // WHEN
        simulateInitializationSync(&skeletonMapping, &backendSkeletonMapping);

        // THEN
        QCOMPARE(backendSkeletonMapping.peerId(), skeletonMapping.id());
        QCOMPARE(backendSkeletonMapping.isEnabled(), skeletonMapping.isEnabled());
        QCOMPARE(backendSkeletonMapping.skeletonId(), skeletonMapping.skeleton()->id());
        QCOMPARE(backendSkeletonMapping.mappingType(), Qt3DAnimation::Animation::ChannelMapping::SkeletonMappingType);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DAnimation::Animation::Handler handler;
        Qt3DAnimation::Animation::ChannelMapping backendMapping;
        backendMapping.setHandler(&handler);

        // THEN
        QVERIFY(backendMapping.peerId().isNull());
        QCOMPARE(backendMapping.isEnabled(), false);
        QCOMPARE(backendMapping.channelName(), QString());
        QCOMPARE(backendMapping.targetId(), Qt3DCore::QNodeId());
        QCOMPARE(backendMapping.propertyName(), nullptr);
        QCOMPARE(backendMapping.componentCount(), 0);
        QCOMPARE(backendMapping.type(), static_cast<int>(QVariant::Invalid));
        QCOMPARE(backendMapping.skeletonId(), Qt3DCore::QNodeId());
        QCOMPARE(backendMapping.mappingType(), Qt3DAnimation::Animation::ChannelMapping::ChannelMappingType);

        // GIVEN
        Qt3DAnimation::QChannelMapping mapping;
        auto target = new tst_TargetEntity;
        mapping.setChannelName(QLatin1String("Location"));
        mapping.setTarget(target);
        mapping.setProperty(QLatin1String("foo"));

        // WHEN
        simulateInitializationSync(&mapping, &backendMapping);
        backendMapping.setSkeletonId(Qt3DCore::QNodeId::createId());
        backendMapping.cleanup();

        // THEN
        QCOMPARE(backendMapping.isEnabled(), false);
        QCOMPARE(backendMapping.channelName(), QString());
        QCOMPARE(backendMapping.targetId(), Qt3DCore::QNodeId());
        QCOMPARE(backendMapping.propertyName(), nullptr);
        QCOMPARE(backendMapping.componentCount(), 0);
        QCOMPARE(backendMapping.type(), static_cast<int>(QVariant::Invalid));
        QCOMPARE(backendMapping.skeletonId(), Qt3DCore::QNodeId());
        QCOMPARE(backendMapping.mappingType(), Qt3DAnimation::Animation::ChannelMapping::ChannelMappingType);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QChannelMapping mapping;
        Qt3DAnimation::Animation::Handler handler;
        Qt3DAnimation::Animation::ChannelMapping backendMapping;
        backendMapping.setHandler(&handler);
        simulateInitializationSync(&mapping, &backendMapping);

        // WHEN
        mapping.setEnabled(false);
        backendMapping.syncFromFrontEnd(&mapping, false);

        // THEN
        QCOMPARE(backendMapping.isEnabled(), false);

        // WHEN
        const QString channelName(QLatin1String("Translation"));
        mapping.setChannelName(channelName);
        backendMapping.syncFromFrontEnd(&mapping, false);

        // THEN
        QCOMPARE(backendMapping.channelName(), channelName);

        // WHEN
        const auto target = new Qt3DCore::QTransform();
        mapping.setTarget(target);
        mapping.setProperty("translation");
        backendMapping.syncFromFrontEnd(&mapping, false);

        // THEN
        QCOMPARE(backendMapping.targetId(), target->id());
        QCOMPARE(backendMapping.type(), static_cast<int>(QVariant::Vector3D));
        QCOMPARE(backendMapping.componentCount(), 3);

        const char *testName = "translation";
        QCOMPARE(qstrcmp(testName, backendMapping.propertyName()), 0);
    }

    void checkSkeletonPropertyUpdate()
    {
        // GIVEN
        Qt3DAnimation::QSkeletonMapping mapping;
        Qt3DAnimation::Animation::Handler handler;
        Qt3DAnimation::Animation::ChannelMapping backendMapping;
        backendMapping.setHandler(&handler);
        simulateInitializationSync(&mapping, &backendMapping);

        // WHEN
        mapping.setEnabled(false);
        backendMapping.syncFromFrontEnd(&mapping, false);

        // THEN
        QCOMPARE(backendMapping.isEnabled(), false);

        // WHEN
        auto skeleton = new Qt3DCore::QSkeleton;
        mapping.setSkeleton(skeleton);
        backendMapping.syncFromFrontEnd(&mapping, false);

        // THEN
        QCOMPARE(backendMapping.skeletonId(), skeleton->id());
    }
};

QTEST_MAIN(tst_ChannelMapping)

#include "tst_channelmapping.moc"
