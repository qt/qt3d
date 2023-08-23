# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from animation.pro.

#####################################################################
## 3DAnimation Module:
#####################################################################

qt_internal_add_module(3DAnimation
    SOURCES
        animationlogging.cpp animationlogging_p.h
        backend/abstractevaluateclipanimatorjob.cpp backend/abstractevaluateclipanimatorjob_p.h
        backend/additiveclipblend.cpp backend/additiveclipblend_p.h
        backend/animationclip.cpp backend/animationclip_p.h
        backend/animationutils.cpp backend/animationutils_p.h
        backend/backendnode.cpp backend/backendnode_p.h
        backend/bezierevaluator.cpp backend/bezierevaluator_p.h
        backend/blendedclipanimator.cpp backend/blendedclipanimator_p.h
        backend/buildblendtreesjob.cpp backend/buildblendtreesjob_p.h
        backend/channelmapper.cpp backend/channelmapper_p.h
        backend/channelmapping.cpp backend/channelmapping_p.h
        backend/clipanimator.cpp backend/clipanimator_p.h
        backend/clipblendnode.cpp backend/clipblendnode_p.h
        backend/clipblendnodevisitor.cpp backend/clipblendnodevisitor_p.h
        backend/clipblendvalue.cpp backend/clipblendvalue_p.h
        backend/clock.cpp backend/clock_p.h
        backend/evaluateblendclipanimatorjob.cpp backend/evaluateblendclipanimatorjob_p.h
        backend/evaluateclipanimatorjob.cpp backend/evaluateclipanimatorjob_p.h
        backend/fcurve.cpp backend/fcurve_p.h
        backend/findrunningclipanimatorsjob.cpp backend/findrunningclipanimatorsjob_p.h
        backend/functionrangefinder.cpp backend/functionrangefinder_p.h
        backend/gltfimporter.cpp backend/gltfimporter_p.h
        backend/handle_types_p.h
        backend/handler.cpp backend/handler_p.h
        backend/keyframe_p.h
        backend/lerpclipblend.cpp backend/lerpclipblend_p.h
        backend/loadanimationclipjob.cpp backend/loadanimationclipjob_p.h
        backend/managers.cpp backend/managers_p.h
        backend/nodefunctor_p.h
        backend/skeleton.cpp backend/skeleton_p.h
        frontend/qabstractanimation.cpp frontend/qabstractanimation.h frontend/qabstractanimation_p.h
        frontend/qabstractanimationclip.cpp frontend/qabstractanimationclip.h frontend/qabstractanimationclip_p.h
        frontend/qabstractchannelmapping.cpp frontend/qabstractchannelmapping.h frontend/qabstractchannelmapping_p.h
        frontend/qabstractclipanimator.cpp frontend/qabstractclipanimator.h frontend/qabstractclipanimator_p.h
        frontend/qabstractclipblendnode.cpp frontend/qabstractclipblendnode.h frontend/qabstractclipblendnode_p.h
        frontend/qadditiveclipblend.cpp frontend/qadditiveclipblend.h frontend/qadditiveclipblend_p.h
        frontend/qanimationaspect.cpp frontend/qanimationaspect.h frontend/qanimationaspect_p.h
        frontend/qanimationcallback.cpp frontend/qanimationcallback.h
        frontend/qanimationclip.cpp frontend/qanimationclip.h frontend/qanimationclip_p.h
        frontend/qanimationclipdata.cpp frontend/qanimationclipdata.h
        frontend/qanimationcliploader.cpp frontend/qanimationcliploader.h frontend/qanimationcliploader_p.h
        frontend/qanimationcontroller.cpp frontend/qanimationcontroller.h frontend/qanimationcontroller_p.h
        frontend/qanimationgroup.cpp frontend/qanimationgroup.h frontend/qanimationgroup_p.h
        frontend/qblendedclipanimator.cpp frontend/qblendedclipanimator.h frontend/qblendedclipanimator_p.h
        frontend/qcallbackmapping.cpp frontend/qcallbackmapping.h frontend/qcallbackmapping_p.h
        frontend/qchannel.cpp frontend/qchannel.h
        frontend/qchannelcomponent.cpp frontend/qchannelcomponent.h
        frontend/qchannelmapper.cpp frontend/qchannelmapper.h frontend/qchannelmapper_p.h
        frontend/qchannelmapping.cpp frontend/qchannelmapping.h frontend/qchannelmapping_p.h
        frontend/qclipanimator.cpp frontend/qclipanimator.h frontend/qclipanimator_p.h
        frontend/qclipblendvalue.cpp frontend/qclipblendvalue.h frontend/qclipblendvalue_p.h
        frontend/qclock.cpp frontend/qclock.h frontend/qclock_p.h
        frontend/qkeyframe.cpp frontend/qkeyframe.h
        frontend/qkeyframeanimation.cpp frontend/qkeyframeanimation.h frontend/qkeyframeanimation_p.h
        frontend/qlerpclipblend.cpp frontend/qlerpclipblend.h frontend/qlerpclipblend_p.h
        frontend/qmorphinganimation.cpp frontend/qmorphinganimation.h frontend/qmorphinganimation_p.h
        frontend/qmorphtarget.cpp frontend/qmorphtarget.h frontend/qmorphtarget_p.h
        frontend/qskeletonmapping.cpp frontend/qskeletonmapping.h frontend/qskeletonmapping_p.h
        frontend/qvertexblendanimation.cpp frontend/qvertexblendanimation.h frontend/qvertexblendanimation_p.h
        job_common_p.h
        qt3danimation_global.h qt3danimation_global_p.h
    DEFINES
        QT3DANIMATION_LIBRARY
    INCLUDE_DIRECTORIES
        backend
        frontend
    LIBRARIES
        Qt::CorePrivate
        Qt::3DCorePrivate
        Qt::3DRenderPrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DRender
        Qt::Core
        Qt::Gui
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::3DCorePrivate
        Qt::3DRenderPrivate
)

# QTransform conflicts
set_target_properties(3DAnimation PROPERTIES UNITY_BUILD OFF)

#### Keys ignored in scope 1:.:.:animation.pro:<TRUE>:
# MODULE = "3DAnimation"
