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

public:
    ChannelMapping *createChannelMapping(Handler &handler,
                                         const QString &channelName,
                                         const Qt3DCore::QNodeId targetId,
                                         const QString &propertyName,
                                         int type)
    {
        auto channelMappingId = Qt3DCore::QNodeId::createId();
        ChannelMapping *channelMapping = handler.channelMappingManager()->getOrCreateResource(channelMappingId);
        setPeerId(channelMapping, channelMappingId);
        channelMapping->setTargetId(targetId);
        channelMapping->setProperty(propertyName);
        channelMapping->setChannelName(channelName);
        channelMapping->setType(type);
        return channelMapping;
    }

    ChannelMapper *createChannelMapper(Handler &handler,
                                       const QVector<Qt3DCore::QNodeId> &mappingIds)
    {
        auto channelMapperId = Qt3DCore::QNodeId::createId();
        ChannelMapper *channelMapper = handler.channelMapperManager()->getOrCreateResource(channelMapperId);
        setPeerId(channelMapper, channelMapperId);
        channelMapper->setMappingIds(mappingIds);
        return channelMapper;
    }

    AnimationClipLoader *createAnimationClipLoader(Handler &handler,
                                                   const QUrl &source)
    {
        auto clipId = Qt3DCore::QNodeId::createId();
        AnimationClipLoader *clip = handler.animationClipLoaderManager()->getOrCreateResource(clipId);
        setPeerId(clip, clipId);
        clip->setSource(source);
        clip->loadAnimation();
        return clip;
    }

private Q_SLOTS:
    void checkBuildPropertyMappings()
    {
        // GIVEN
        Handler handler;

        // Create a channel mapping...
        auto channelMapping = createChannelMapping(handler,
                                                   QLatin1String("Location"),
                                                   Qt3DCore::QNodeId::createId(),
                                                   QLatin1String("translation"),
                                                   static_cast<int>(QVariant::Vector3D));

        // ... a channel mapper...
        auto channelMapper = createChannelMapper(handler, QVector<Qt3DCore::QNodeId>() << channelMapping->peerId());

        // ...and an animation clip
        auto clip = createAnimationClipLoader(handler, QUrl("qrc:/clip1.json"));

        // WHEN
        // Build the mapping data for the above configuration
        QVector<AnimationUtils::MappingData> mappingData = AnimationUtils::buildPropertyMappings(&handler, clip, channelMapper);

        // THEN
        QCOMPARE(mappingData.size(), channelMapper->mappingIds().size());
        for (int i = 0; i < mappingData.size(); ++i) {
            const auto mapping = mappingData[i];
            QCOMPARE(mapping.targetId, channelMapping->targetId());
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
