// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_ANIMATIONCLIP_P_H
#define QT3DANIMATION_ANIMATION_ANIMATIONCLIP_P_H

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
#include <Qt3DAnimation/qanimationclipdata.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/fcurve_p.h>
#include <QtCore/qurl.h>
#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Handler;

class Q_AUTOTEST_EXPORT AnimationClip : public BackendNode
{
public:
    AnimationClip();

    void cleanup();
    void setSource(const QUrl &source) { m_source = source; }
    QUrl source() const { return m_source; }
    void setStatus(QAnimationClipLoader::Status status);
    QAnimationClipLoader::Status status() const { return m_status; }
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void addDependingClipAnimator(const Qt3DCore::QNodeId &id);
    void addDependingBlendedClipAnimator(const Qt3DCore::QNodeId &id);

    QString name() const { return m_name; }
    const QVector<Channel> &channels() const { return m_channels; }

    // Called from jobs
    void loadAnimation();
    void setDuration(float duration);
    float duration() const { return m_duration; }
    qsizetype channelIndex(const QString &channelName, qsizetype jointIndex) const;
    qsizetype channelCount() const { return m_channelComponentCount; }
    qsizetype channelComponentBaseIndex(qsizetype channelGroupIndex) const;

    // Allow unit tests to set the data type
#if !defined(QT_BUILD_INTERNAL)
private:
#endif
    enum ClipDataType {
        Unknown,
        File,
        Data
    };
#if defined(QT_BUILD_INTERNAL)
public:
    void setDataType(ClipDataType dataType) { m_dataType = dataType; }
#endif

private:
    void loadAnimationFromUrl();
    void loadAnimationFromData();
    void clearData();
    float findDuration();
    qsizetype findChannelComponentCount();

    QMutex m_mutex;

    QUrl m_source;
    QAnimationClipLoader::Status m_status;
    QAnimationClipData m_clipData;
    ClipDataType m_dataType;

    QString m_name;
    QVector<Channel> m_channels;
    float m_duration;
    qsizetype m_channelComponentCount;

    Qt3DCore::QNodeIdVector m_dependingAnimators;
    Qt3DCore::QNodeIdVector m_dependingBlendedAnimators;
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const AnimationClip &animationClip)
{
    QDebugStateSaver saver(dbg);
    dbg << "QNodeId =" << animationClip.peerId() << Qt::endl
        << "Name =" << animationClip.name() << Qt::endl
        << "Duration: " << animationClip.duration() << Qt::endl
        << "Channels:" << Qt::endl;

    const auto &channels = animationClip.channels();
    for (const auto &channel : channels)
        dbg << channel;

    return dbg;
}
#endif

} // namespace Animation
} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_ANIMATIONCLIP_P_H
