// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "animationfactory.h"

using namespace Qt3DAnimation;

AnimationFactory::AnimationFactory(QObject *parent)
    : QObject(parent)
{
    updateClipData();
}

void AnimationFactory::updateClipData()
{
    m_clipData.clearChannels();

    // Add a channel for a Location animation
    QChannel location(QLatin1String("Location"));

    QChannelComponent locationX(QLatin1String("Location X"));
    locationX.appendKeyFrame(QKeyFrame({0.0f, 0.0f}, {-1.0f, 0.0f}, {1.0f, 0.0f}));
    locationX.appendKeyFrame(QKeyFrame({2.45f, 5.0f}, {1.45f, 5.0f}, {3.45f, 5.0f}));

    QChannelComponent locationY(QLatin1String("Location Y"));
    locationY.appendKeyFrame(QKeyFrame({0.0f, 0.0f}, {-1.0f, 0.0f}, {1.0f, 0.0f}));
    locationY.appendKeyFrame(QKeyFrame({2.45f, 0.0f}, {1.45f, 0.0f}, {3.45f, 0.0f}));

    QChannelComponent locationZ(QLatin1String("Location Z"));
    locationZ.appendKeyFrame(QKeyFrame({0.0f, 0.0f}, {-1.0f, 0.0f}, {1.0f, 0.0f}));
    locationZ.appendKeyFrame(QKeyFrame({2.45f, 0.0f}, {1.45f, 0.0f}, {3.45f, 0.0f}));

    location.appendChannelComponent(locationX);
    location.appendChannelComponent(locationY);
    location.appendChannelComponent(locationZ);

    m_clipData.appendChannel(location);
}
