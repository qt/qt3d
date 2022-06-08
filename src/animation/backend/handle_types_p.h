// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_HANDLE_TYPES_P_H
#define QT3DANIMATION_ANIMATION_HANDLE_TYPES_P_H

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

#include <Qt3DCore/private/qhandle_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class AnimationClip;
class ClipAnimator;
class BlendedClipAnimator;
class ChannelMapping;
class ChannelMapper;
class Skeleton;

typedef Qt3DCore::QHandle<AnimationClip> HAnimationClip;
typedef Qt3DCore::QHandle<ClipAnimator> HClipAnimator;
typedef Qt3DCore::QHandle<BlendedClipAnimator> HBlendedClipAnimator;
typedef Qt3DCore::QHandle<ChannelMapping> HChannelMapping;
typedef Qt3DCore::QHandle<ChannelMapper> HChannelMapper;
typedef Qt3DCore::QHandle<Skeleton> HSkeleton;

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_HANDLE_TYPES_P_H
