// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICK3DANIMATIONPLUGIN_P_H
#define QT3DQUICK3DANIMATIONPLUGIN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQml/qqml.h>

#include <Qt3DAnimation/qabstractclipanimator.h>
#include <Qt3DAnimation/qabstractanimationclip.h>
#include <Qt3DAnimation/qanimationclip.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DAnimation/qclipanimator.h>
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/qskeletonmapping.h>
#include <Qt3DAnimation/qlerpclipblend.h>
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/qclipblendvalue.h>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/qkeyframeanimation.h>
#include <Qt3DAnimation/qanimationcontroller.h>
#include <Qt3DAnimation/qabstractanimation.h>
#include <Qt3DAnimation/qmorphinganimation.h>
#include <Qt3DAnimation/qanimationgroup.h>
#include <Qt3DAnimation/qmorphtarget.h>
#include <Qt3DAnimation/qvertexblendanimation.h>

#include <Qt3DQuickAnimation/private/qt3dquickanimation_global_p.h>
#include <Qt3DQuickAnimation/private/quick3dchannelmapper_p.h>
#include <Qt3DQuickAnimation/private/quick3dkeyframeanimation_p.h>
#include <Qt3DQuickAnimation/private/quick3danimationgroup_p.h>
#include <Qt3DQuickAnimation/private/quick3danimationcontroller_p.h>
#include <Qt3DQuickAnimation/private/quick3dmorphtarget_p.h>
#include <Qt3DQuickAnimation/private/quick3dmorphinganimation_p.h>
#include <Qt3DQuickAnimation/private/quick3dvertexblendanimation_p.h>

QT_BEGIN_NAMESPACE

class QAbstractClipAnimatorForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractClipAnimator)
    QML_FOREIGN(Qt3DAnimation::QAbstractClipAnimator)
    QML_UNCREATABLE("QAbstractClipAnimator is abstract")
    QML_ADDED_IN_VERSION(2, 9);
};

class QClipAnimatorForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ClipAnimator)
    QML_FOREIGN(Qt3DAnimation::QClipAnimator)
    QML_ADDED_IN_VERSION(2, 9)
};

class QBlendedClipAnimatorForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(BlendedClipAnimator)
    QML_FOREIGN(Qt3DAnimation::QBlendedClipAnimator)
    QML_ADDED_IN_VERSION(2, 9)
};

class QChannelMappingForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ChannelMapping)
    QML_FOREIGN(Qt3DAnimation::QChannelMapping)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAbstractAnimationClipForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractAnimationClip)
    QML_FOREIGN(Qt3DAnimation::QAbstractAnimationClip)
    QML_UNCREATABLE("QAbstractAnimationClip is abstract")
    QML_ADDED_IN_VERSION(2, 9);
};

class QAnimationClipLoaderForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AnimationClipLoader)
    QML_FOREIGN(Qt3DAnimation::QAnimationClipLoader)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAnimationClipForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AnimationClip)
    QML_FOREIGN(Qt3DAnimation::QAnimationClip)
    QML_ADDED_IN_VERSION(2, 9)
};

class QChannelMapperForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ChannelMapper)
    QML_FOREIGN(Qt3DAnimation::QChannelMapper)
    QML_EXTENDED(Qt3DAnimation::Animation::Quick::Quick3DChannelMapper)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAbstractClipBlendNodeForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractClipBlendNode)
    QML_FOREIGN(Qt3DAnimation::QAbstractClipBlendNode)
    QML_UNCREATABLE("QAbstractClipBlendNode is abstract")
    QML_ADDED_IN_VERSION(2, 9);
};

class QLerpClipBlendForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(LerpClipBlend)
    QML_FOREIGN(Qt3DAnimation::QLerpClipBlend)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAdditiveClipBlendForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AdditiveClipBlend)
    QML_FOREIGN(Qt3DAnimation::QAdditiveClipBlend)
    QML_ADDED_IN_VERSION(2, 9)
};

class QClipBlendValueForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ClipBlendValue)
    QML_FOREIGN(Qt3DAnimation::QClipBlendValue)
    QML_ADDED_IN_VERSION(2, 9)
};

class QClockForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Clock)
    QML_FOREIGN(Qt3DAnimation::QClock)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAbstractAnimationForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractAnimation)
    QML_FOREIGN(Qt3DAnimation::QAbstractAnimation)
    QML_UNCREATABLE("AbstractAnimation is abstract")
    QML_ADDED_IN_VERSION(2, 9);
};

class QKeyframeAnimationForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(KeyframeAnimation)
    QML_FOREIGN(Qt3DAnimation::QKeyframeAnimation)
    QML_EXTENDED(Qt3DAnimation::Quick::QQuick3DKeyframeAnimation)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAnimationGroupForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AnimationGroup)
    QML_FOREIGN(Qt3DAnimation::QAnimationGroup)
    QML_EXTENDED(Qt3DAnimation::Quick::QQuick3DAnimationGroup)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAnimationControllerForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AnimationController)
    QML_FOREIGN(Qt3DAnimation::QAnimationController)
    QML_EXTENDED(Qt3DAnimation::Quick::QQuick3DAnimationController)
    QML_ADDED_IN_VERSION(2, 9)
};

class QMorphingAnimationForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(MorphingAnimation)
    QML_FOREIGN(Qt3DAnimation::QMorphingAnimation)
    QML_EXTENDED(Qt3DAnimation::Quick::QQuick3DMorphingAnimation)
    QML_ADDED_IN_VERSION(2, 9)
};

class QMorphTargetForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(MorphTarget)
    QML_FOREIGN(Qt3DAnimation::QMorphTarget)
    QML_EXTENDED(Qt3DAnimation::Quick::QQuick3DMorphTarget)
    QML_ADDED_IN_VERSION(2, 9)
};

class QVertexBlendAnimationForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(VertexBlendAnimation)
    QML_FOREIGN(Qt3DAnimation::QVertexBlendAnimation)
    QML_EXTENDED(Qt3DAnimation::Quick::QQuick3DVertexBlendAnimation)
    QML_ADDED_IN_VERSION(2, 9)
};

class QAbstractChannelMappingForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractChannelMapping)
    QML_FOREIGN(Qt3DAnimation::QAbstractChannelMapping)
    QML_UNCREATABLE("QAbstractChannelMapping is abstract")
    QML_ADDED_IN_VERSION(2, 10);
};

class QSkeletonMappingForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SkeletonMapping)
    QML_FOREIGN(Qt3DAnimation::QSkeletonMapping)
    QML_ADDED_IN_VERSION(2, 10)
};

QT_END_NAMESPACE

#endif // QT3DQUICK3DANIMATIONPLUGIN_P_H
