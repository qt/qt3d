// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_CHANNELMAPPING_P_H
#define QT3DANIMATION_ANIMATION_CHANNELMAPPING_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DAnimation/private/backendnode_p.h>
#include <Qt3DAnimation/private/fcurve_p.h>
#include <Qt3DAnimation/qanimationcallback.h>

#include <Qt3DCore/qnodeid.h>
#include <QtCore/QMetaProperty>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

namespace Animation {

class Handler;

class Q_AUTOTEST_EXPORT ChannelMapping : public BackendNode
{
public:
    enum MappingType {
        ChannelMappingType = 0,
        SkeletonMappingType,
        CallbackMappingType
    };

    ChannelMapping();

    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void setChannelName(const QString &channelName) { m_channelName = channelName; }
    QString channelName() const { return m_channelName; }

    void setTargetId(Qt3DCore::QNodeId targetId) { m_targetId = targetId; }
    Qt3DCore::QNodeId targetId() const { return m_targetId; }

    void setType(int type) { m_type = type; }
    int type() const { return m_type; }

    void setPropertyName(const char *propertyName) { m_propertyName = propertyName; }
    const char *propertyName() const { return m_propertyName; }

    void setComponentCount(int componentCount) { m_componentCount = componentCount; }
    int componentCount() const { return m_componentCount; }

    void setCallback(QAnimationCallback *callback) { m_callback = callback; }
    QAnimationCallback *callback() const { return m_callback; }

    void setCallbackFlags(QAnimationCallback::Flags flags) { m_callbackFlags = flags; }
    QAnimationCallback::Flags callbackFlags() const { return m_callbackFlags; }

    void setSkeletonId(Qt3DCore::QNodeId skeletonId) { m_skeletonId = skeletonId; }
    Qt3DCore::QNodeId skeletonId() const { return m_skeletonId; }
    Skeleton *skeleton() const;

    void setMappingType(MappingType mappingType) { m_mappingType = mappingType; }
    MappingType mappingType() const { return m_mappingType; }

private:
    // Properties from QChannelMapping
    QString m_channelName;
    Qt3DCore::QNodeId m_targetId;
    int m_type;
    int m_componentCount;
    const char *m_propertyName;

    // TODO: Properties from QCallbackMapping
    QAnimationCallback *m_callback;
    QAnimationCallback::Flags m_callbackFlags;

    // Properties from QSkeletonMapping
    Qt3DCore::QNodeId m_skeletonId;

    MappingType m_mappingType;
};

} // namespace Animation
} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_CHANNELMAPPING_P_H
