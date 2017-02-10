/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qt3dquick3danimationplugin.h"
#include <Qt3DAnimation/qanimationclip.h>
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DAnimation/qclipanimator.h>
#include <Qt3DAnimation/qconductedclipanimator.h>
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/qlerpblend.h>
#include <Qt3DAnimation/qadditiveblend.h>

#include <Qt3DAnimation/qkeyframeanimation.h>
#include <Qt3DAnimation/qanimationcontroller.h>
#include <Qt3DAnimation/qabstractanimation.h>
#include <Qt3DAnimation/qmorphinganimation.h>
#include <Qt3DAnimation/qanimationgroup.h>
#include <Qt3DAnimation/qmorphtarget.h>

#include <Qt3DQuickAnimation/private/qt3dquickanimation_global_p.h>
#include <Qt3DQuickAnimation/private/quick3dchannelmapper_p.h>
#include <Qt3DQuickAnimation/private/quick3dabstractclipblendnode_p.h>
#include <Qt3DQuickAnimation/private/quick3dkeyframeanimation_p.h>
#include <Qt3DQuickAnimation/private/quick3danimationgroup_p.h>
#include <Qt3DQuickAnimation/private/quick3danimationcontroller_p.h>
#include <Qt3DQuickAnimation/private/quick3dmorphtarget_p.h>
#include <Qt3DQuickAnimation/private/quick3dmorphinganimation_p.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DAnimationPlugin::registerTypes(const char *uri)
{
    Qt3DAnimation::Quick::Quick3DAnimation_initialize();

    // @uri Qt3D.Animation
    qmlRegisterType<Qt3DAnimation::QAnimationClip>(uri, 2, 2, "AnimationClip");
    qmlRegisterType<Qt3DAnimation::QClipAnimator>(uri, 2, 2, "ClipAnimator");
    qmlRegisterType<Qt3DAnimation::QBlendedClipAnimator>(uri, 2, 2, "BlendedClipAnimator");
    qmlRegisterType<Qt3DAnimation::QConductedClipAnimator>(uri, 2, 2, "ConductedClipAnimator");
    qmlRegisterType<Qt3DAnimation::QChannelMapping>(uri, 2, 2, "ChannelMapping");
    qmlRegisterExtendedType<Qt3DAnimation::QChannelMapper,
                            Qt3DAnimation::Animation::Quick::Quick3DChannelMapper>(uri, 2, 2, "ChannelMapper");
    qmlRegisterExtendedUncreatableType<Qt3DAnimation::QAbstractClipBlendNode,
                                       Qt3DAnimation::Animation::Quick::Quick3DAbstractClipBlendNode>(uri, 2, 2, "AbstractClipBlendNode", QStringLiteral("QAbstractClipBlendNode is abstract"));
    qmlRegisterType<Qt3DAnimation::QLerpBlend>(uri, 2, 2, "LerpBlend");
    qmlRegisterType<Qt3DAnimation::QAdditiveBlend>(uri, 2, 2, "AdditiveBlend");

    qmlRegisterUncreatableType<Qt3DAnimation::QAbstractAnimation>(uri, 2, 2, "AbstractAnimation", QStringLiteral("AbstractAnimation is abstract"));
    qmlRegisterExtendedType<Qt3DAnimation::QKeyframeAnimation, Qt3DAnimation::Quick::QQuick3DKeyframeAnimation>(uri, 2, 2, "KeyframeAnimation");
    qmlRegisterExtendedType<Qt3DAnimation::QAnimationGroup, Qt3DAnimation::Quick::QQuick3DAnimationGroup>(uri, 2, 2, "AnimationGroup");
    qmlRegisterExtendedType<Qt3DAnimation::QAnimationController, Qt3DAnimation::Quick::QQuick3DAnimationController>(uri, 2, 2, "AnimationController");
    qmlRegisterExtendedType<Qt3DAnimation::QMorphingAnimation, Qt3DAnimation::Quick::QQuick3DMorphingAnimation>(uri, 2, 2, "MorphingAnimation");
    qmlRegisterExtendedType<Qt3DAnimation::QMorphTarget, Qt3DAnimation::Quick::QQuick3DMorphTarget>(uri, 2, 2, "MorphTarget");
}

QT_END_NAMESPACE
