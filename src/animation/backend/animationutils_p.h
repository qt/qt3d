/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qscenechange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

struct Channel;
class Handler;
class AnimationClipLoader;
class ChannelMapper;

struct MappingData
{
    Qt3DCore::QNodeId targetId;
    const char *propertyName;
    int type;
    QVector<int> channelIndices;
};

struct BlendingMappingData : public MappingData
{
    QVector<int> channelIndicesClip1;
    QVector<int> channelIndicesClip2;

    enum BlendAction {
        NoBlending, // Use the channel from Clip1 only
        ClipBlending, // Blending 2 clips sharing the same channel
    };
    BlendAction blendAction;
};

struct AnimatorEvaluationData
{
    double globalTime;
    double startTime;
    int loopCount;
    double playbackRate;
};

struct ClipPreEvaluationData
{
    int currentLoop;
    double localTime;
    bool isFinalFrame;
};

template<typename Animator>
AnimatorEvaluationData animatorEvaluationDataForAnimator(Animator animator, qint64 globalTime)
{
    AnimatorEvaluationData data;
    data.loopCount = animator->loops();
    data.playbackRate = 1.0; // should be a property on the animator
    // Convert global time from nsec to sec
    data.startTime = double(animator->startTime()) / 1.0e9;
    data.globalTime = double(globalTime) / 1.0e9;
    return data;
}

Q_AUTOTEST_EXPORT
ClipPreEvaluationData evaluationDataForClip(AnimationClipLoader *clip,
                                            const AnimatorEvaluationData &animatorData);

Q_AUTOTEST_EXPORT
QVector<int> channelComponentsToIndices(const Channel &channelGroup,
                                        int dataType,
                                        int offset = 0);

Q_AUTOTEST_EXPORT
QVector<int> channelComponentsToIndicesHelper(const Channel &channelGroup,
                                              int dataType,
                                              int offset,
                                              const QStringList &suffixes);

Q_AUTOTEST_EXPORT
QVector<float> evaluateClipAtLocalTime(AnimationClipLoader *clip,
                                       float localTime);

Q_AUTOTEST_EXPORT
QVector<Qt3DCore::QSceneChangePtr> preparePropertyChanges(Qt3DCore::QNodeId peerId,
                                                          const QVector<MappingData> &mappingData,
                                                          const QVector<float> &channelResults,
                                                          bool finalFrame);

Q_AUTOTEST_EXPORT
QVector<MappingData> buildPropertyMappings(Handler *handler,
                                           const AnimationClipLoader *clip,
                                           const ChannelMapper *mapper);

Q_AUTOTEST_EXPORT
double localTimeFromGlobalTime(double t_global, double t_start_global,
                               double playbackRate, double duration,
                               int loopCount, int &currentLoop);

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE


#endif // QT3DANIMATION_ANIMATION_ANIMATIONUTILS_P_H
