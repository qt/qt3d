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
#include <Qt3DAnimation/private/animationcliploader_p.h>
#include <Qt3DAnimation/private/animationutils_p.h>
#include <Qt3DAnimation/private/channelmapper_p.h>
#include <Qt3DAnimation/private/channelmapping_p.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DAnimation/private/managers_p.h>
#include <qbackendnodetester.h>
#include <testpostmanarbiter.h>

using namespace Qt3DAnimation::Animation;

class tst_AnimationUtils : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkBuildPropertyMappings()
    {
        // GIVEN
        Handler handler;

        // Create a channel mapping, set properties, and add it to manager
        auto channelMappingId = Qt3DCore::QNodeId::createId();
        ChannelMapping *channelMapping = handler.channelMappingManager()->getOrCreateResource(channelMappingId);
        setPeerId(channelMapping, channelMappingId);

        auto targetId = Qt3DCore::QNodeId::createId();
        channelMapping->setTargetId(targetId);
        channelMapping->setProperty("translation");
        channelMapping->setChannelName("Location");
        channelMapping->setType(static_cast<int>(QVariant::Vector3D));

        // Create a channel mapper and add mapping to it
        auto channelMapperId = Qt3DCore::QNodeId::createId();
        ChannelMapper *channelMapper = handler.channelMapperManager()->getOrCreateResource(channelMapperId);
        setPeerId(channelMapper, channelMapperId);
        channelMapper->setMappingIds(QVector<Qt3DCore::QNodeId>() << channelMappingId);

        // Create an animation clip
        auto clipId = Qt3DCore::QNodeId::createId();
        AnimationClipLoader *clip = handler.animationClipLoaderManager()->getOrCreateResource(clipId);
        setPeerId(clip, clipId);
        clip->setSource(QUrl("qrc:/clip1.json"));
        clip->loadAnimation();

        // WHEN
        // Build the mapping data for the above configuration
        QVector<AnimationUtils::MappingData> mappingData = AnimationUtils::buildPropertyMappings(&handler, clip, channelMapper);

        // THEN
        QCOMPARE(mappingData.size(), channelMapper->mappingIds().size());
        for (int i = 0; i < mappingData.size(); ++i) {
            const auto mapping = mappingData[i];
            QCOMPARE(mapping.targetId, targetId);
            QCOMPARE(mapping.propertyName, channelMapping->propertyName());
            QCOMPARE(mapping.type, channelMapping->type());
            QCOMPARE(mapping.channelIndices.size(), 3);
            for (int j = 0; j < 3; ++j) {
                QCOMPARE(mapping.channelIndices[j], j);
            }
        }
    }
};

QTEST_MAIN(tst_AnimationUtils)

#include "tst_animationutils.moc"
