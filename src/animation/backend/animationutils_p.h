// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_ANIMATIONUTILS_P_H
#define QT3DANIMATION_ANIMATION_ANIMATIONUTILS_P_H

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

#include <Qt3DAnimation/private/qt3danimation_global_p.h>
#include <Qt3DAnimation/private/clock_p.h>
#include <Qt3DAnimation/qanimationcallback.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/sqt_p.h>

#include <QtCore/qbitarray.h>
#include <QtCore/qdebug.h>
#include <QtCore/QVariant>
#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
class QAnimationCallback;
namespace Animation {

struct Channel;
class BlendedClipAnimator;
class Handler;
class AnimationClip;
class ChannelMapper;
class ChannelMapping;

using ComponentIndices = QVector<qsizetype>;

enum JointTransformComponent {
    NoTransformComponent = 0,
    Scale,
    Rotation,
    Translation
};

struct MappingData
{
    Qt3DCore::QNodeId targetId;
    Skeleton *skeleton = nullptr;
    int jointIndex = -1;
    JointTransformComponent jointTransformComponent = NoTransformComponent;
    const char *propertyName;
    QAnimationCallback *callback = nullptr;
    QAnimationCallback::Flags callbackFlags;
    int type;
    ComponentIndices channelIndices;
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const MappingData &mapping)
{
    QDebugStateSaver saver(dbg);
    dbg << "targetId =" << mapping.targetId << Qt::endl
        << "jointIndex =" << mapping.jointIndex << Qt::endl
        << "jointTransformComponent: " << mapping.jointTransformComponent << Qt::endl
        << "propertyName:" << mapping.propertyName << Qt::endl
        << "channelIndices:" << mapping.channelIndices;
    return dbg;
}
#endif

struct AnimatorEvaluationData
{
    double elapsedTime;
    double currentTime;
    int loopCount;
    int currentLoop;
    double playbackRate;
    float normalizedLocalTime;
};

struct ClipEvaluationData
{
    int currentLoop;
    float normalizedLocalTime;
    double localTime;
    bool isFinalFrame;
};

using ClipResults = QVector<float>;

struct ChannelNameAndType
{
    QString jointName;
    QString name;
    int type;
    qsizetype jointIndex;
    Qt3DCore::QNodeId mappingId;
    JointTransformComponent jointTransformComponent;
    qsizetype componentCount;

    static const int invalidIndex = -1;

    ChannelNameAndType()
        : jointName()
        , name()
        , type(-1)
        , jointIndex(-1)
        , mappingId()
        , jointTransformComponent(NoTransformComponent)
        , componentCount(-1)
    {}

    ChannelNameAndType(const QString &_name,
                       int _type,
                       int componentCount,
                       Qt3DCore::QNodeId _mappingId = Qt3DCore::QNodeId(),
                       int _jointIndex = invalidIndex)
        : jointName()
        , name(_name)
        , type(_type)
        , jointIndex(_jointIndex)
        , mappingId(_mappingId)
        , jointTransformComponent(NoTransformComponent)
        , componentCount(componentCount)
    {}

    ChannelNameAndType(const QString &_name,
                       int _type,
                       JointTransformComponent _jointTransformComponent)
        : jointName()
        , name(_name)
        , type(_type)
        , jointIndex(invalidIndex)
        , mappingId()
        , jointTransformComponent(_jointTransformComponent)
        , componentCount(-1)
    {
        switch (_jointTransformComponent) {
        case  NoTransformComponent:
            break;
        case Scale:
        case Translation:
            componentCount = 3;
            break;
        case Rotation:
            componentCount = 4;
            break;
        }
    }

    bool operator==(const ChannelNameAndType &rhs) const
    {
        return name == rhs.name
            && type == rhs.type
            && jointIndex == rhs.jointIndex
            && mappingId == rhs.mappingId
            && jointTransformComponent == rhs.jointTransformComponent
            && componentCount == rhs.componentCount;
    }
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const ChannelNameAndType &nameAndType)
{
    QDebugStateSaver saver(dbg);
    dbg << "name =" << nameAndType.name
        << "type =" << nameAndType.type
        << "mappingId =" << nameAndType.mappingId
        << "jointIndex =" << nameAndType.jointIndex
        << "jointName =" << nameAndType.jointName
        << "jointTransformComponent =" << nameAndType.jointTransformComponent
        << "componentCount =" << nameAndType.componentCount;
    return dbg;
}
#endif

struct ComponentValue
{
    qsizetype componentIndex;
    float value;
};
QT3D_DECLARE_TYPEINFO_2(Qt3DAnimation, Animation, ComponentValue, Q_PRIMITIVE_TYPE)

struct ClipFormat
{
    // TODO: Remove the mask and store both the sourceClipIndices and
    // formattedComponentIndices in flat vectors. This will require a
    // way to look up the offset and number of elements for each channel.
    ComponentIndices sourceClipIndices;
    QVector<QBitArray> sourceClipMask;
    QVector<ComponentIndices> formattedComponentIndices;
    QVector<ChannelNameAndType> namesAndTypes;
    QVector<ComponentValue> defaultComponentValues;
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const ClipFormat &format)
{
    QDebugStateSaver saver(dbg);
    qsizetype sourceIndex = 0;
    for (qsizetype i = 0; i < format.namesAndTypes.size(); ++i) {
        dbg << i
            << format.namesAndTypes[i].jointIndex
            << format.namesAndTypes[i].jointName
            << format.namesAndTypes[i].name
            << format.namesAndTypes[i].type
            << "formatted results dst indices =" << format.formattedComponentIndices[i];
        const qsizetype componentCount = format.formattedComponentIndices[i].size();

        dbg << "clip src indices =";
        for (qsizetype j = sourceIndex; j < sourceIndex + componentCount; ++j)
            dbg << format.sourceClipIndices[j] << "";

        dbg << "src clip mask =" << format.sourceClipMask[i];
        dbg << Qt::endl;
        sourceIndex += componentCount;
    }
    return dbg;
}
#endif

struct AnimationCallbackAndValue
{
    QAnimationCallback *callback;
    QAnimationCallback::Flags flags;
    QVariant value;
};

struct AnimationRecord {
    struct TargetChange {
        TargetChange(Qt3DCore::QNodeId id, const char *name, QVariant v)
            : targetId(id), propertyName(name), value(v) {

        }

        Qt3DCore::QNodeId targetId;
        const char *propertyName = nullptr;
        QVariant value;
    };

    Qt3DCore::QNodeId animatorId;
    QList<TargetChange> targetChanges;
    QList<QPair<Qt3DCore::QNodeId, QVector<Qt3DCore::Sqt>>> skeletonChanges;
    float normalizedTime = -1.f;
    bool finalFrame = false;
};

Q_AUTOTEST_EXPORT
AnimationRecord prepareAnimationRecord(Qt3DCore::QNodeId animatorId,
                                       const QVector<MappingData> &mappingDataVec,
                                       const QVector<float> &channelResults,
                                       bool finalFrame,
                                       float normalizedLocalTime);

inline constexpr double toSecs(qint64 nsecs) { return nsecs / 1.0e9; }
inline qint64 toNsecs(double seconds) { return qRound64(seconds * 1.0e9); }

template<typename Animator>
AnimatorEvaluationData evaluationDataForAnimator(Animator animator,
                                                 Clock* clock,
                                                 qint64 nsSincePreviousFrame)
{
    const bool seeking = animator->isSeeking();
    AnimatorEvaluationData data;
    data.loopCount = animator->loops();
    data.currentLoop = animator->currentLoop();
    // The playback-rate is always 1.0 when seeking
    data.playbackRate = ((clock != nullptr) && !seeking) ? clock->playbackRate() : 1.0;
    // Convert global time from nsec to sec
    data.elapsedTime = toSecs(nsSincePreviousFrame);
    // When seeking we base it on the current time being at the start of the clip
    data.currentTime = seeking ? 0.0 : animator->lastLocalTime();
    // If we're not seeking the local normalized time will be calculate in
    // evaluationDataForClip().
    data.normalizedLocalTime = seeking ? animator->normalizedLocalTime() : -1.0;
    return data;
}

inline bool isFinalFrame(double localTime,
                         double duration,
                         int currentLoop,
                         int loopCount,
                         double playbackRate)
{
    // We must be on the final loop and
    // - if playing forward, localTime must be equal or above the duration
    // - if playing backward, localTime must be equal or below 0
    if (playbackRate >= 0.0)
        return (loopCount != 0 && currentLoop >= loopCount - 1 && localTime >= duration);
    return (loopCount != 0  && currentLoop <= 0 && localTime <= 0);
}

inline bool isValidNormalizedTime(float t)
{
    return !(t < 0.0f) && !(t > 1.0f);
}

Q_AUTOTEST_EXPORT
ClipEvaluationData evaluationDataForClip(AnimationClip *clip,
                                         const AnimatorEvaluationData &animatorData);

Q_AUTOTEST_EXPORT
ComponentIndices channelComponentsToIndices(const Channel &channel,
                                            int dataType,
                                            qsizetype expectedComponentCount,
                                            qsizetype offset);

Q_AUTOTEST_EXPORT
ComponentIndices channelComponentsToIndicesHelper(const Channel &channelGroup,
                                                  qsizetype expectedComponentCount,
                                                  qsizetype offset,
                                                  const QList<char> &suffixes);

Q_AUTOTEST_EXPORT
ClipResults evaluateClipAtLocalTime(AnimationClip *clip,
                                    float localTime);

Q_AUTOTEST_EXPORT
ClipResults evaluateClipAtPhase(AnimationClip *clip,
                                float phase);

Q_AUTOTEST_EXPORT
QVector<AnimationCallbackAndValue> prepareCallbacks(const QVector<MappingData> &mappingDataVec,
                                                    const QVector<float> &channelResults);

Q_AUTOTEST_EXPORT
QVector<MappingData> buildPropertyMappings(const QVector<ChannelMapping *> &channelMappings,
                                           const QVector<ChannelNameAndType> &channelNamesAndTypes,
                                           const QVector<ComponentIndices> &channelComponentIndices,
                                           const QVector<QBitArray> &sourceClipMask);

Q_AUTOTEST_EXPORT
QVector<ChannelNameAndType> buildRequiredChannelsAndTypes(Handler *handler,
                                                          const ChannelMapper *mapper);

Q_AUTOTEST_EXPORT
QVector<ComponentIndices> assignChannelComponentIndices(const QVector<ChannelNameAndType> &namesAndTypes);

Q_AUTOTEST_EXPORT
double localTimeFromElapsedTime(double t_current_local, double t_elapsed_global,
                                double playbackRate, double duration,
                                int loopCount, int &currentLoop);

Q_AUTOTEST_EXPORT
double phaseFromElapsedTime(double t_current_local, double t_elapsed_global,
                           double playbackRate, double duration,
                           int loopCount, int &currentLoop);

Q_AUTOTEST_EXPORT
QVector<Qt3DCore::QNodeId> gatherValueNodesToEvaluate(Handler *handler,
                                                      Qt3DCore::QNodeId blendTreeRootId);

Q_AUTOTEST_EXPORT
ClipFormat generateClipFormatIndices(const QVector<ChannelNameAndType> &targetChannels,
                                     const QVector<ComponentIndices> &targetIndices,
                                     const AnimationClip *clip);

Q_AUTOTEST_EXPORT
ClipResults formatClipResults(const ClipResults &rawClipResults,
                              const ComponentIndices &format);

Q_AUTOTEST_EXPORT
ClipResults evaluateBlendTree(Handler *handler,
                              BlendedClipAnimator *animator,
                              Qt3DCore::QNodeId blendNodeId);

Q_AUTOTEST_EXPORT
QVector<float> defaultValueForChannel(Handler *handler, const ChannelNameAndType &channelDescription);

Q_AUTOTEST_EXPORT
void applyComponentDefaultValues(const QVector<ComponentValue> &componentDefaults,
                                 ClipResults &formattedClipResults);

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DAnimation::Animation::AnimationRecord) // LCOV_EXCL_LINE


#endif // QT3DANIMATION_ANIMATION_ANIMATIONUTILS_P_H
