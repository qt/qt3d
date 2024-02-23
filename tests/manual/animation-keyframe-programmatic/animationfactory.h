// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef ANIMATIONFACTORY_H
#define ANIMATIONFACTORY_H

#include <QObject>
#include <Qt3DAnimation/qanimationclipdata.h>

class AnimationFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DAnimation::QAnimationClipData clipData READ clipData NOTIFY clipDataChanged)

public:
    explicit AnimationFactory(QObject *parent = 0);

    Qt3DAnimation::QAnimationClipData clipData() const
    {
        return m_clipData;
    }

signals:
    void clipDataChanged(Qt3DAnimation::QAnimationClipData clipData);

private:
    void updateClipData();

    Qt3DAnimation::QAnimationClipData m_clipData;
};

#endif // ANIMATIONFACTORY_H
