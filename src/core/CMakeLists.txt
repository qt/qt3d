# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from core.pro.

#####################################################################
## Qt::3DCore Module:
#####################################################################

qt_internal_add_module(3DCore
    SOURCES
        aspect/qcoreaspect.cpp aspect/qcoreaspect.h aspect/qcoreaspect_p.h
        aspect/qcoresettings.cpp aspect/qcoresettings.h aspect/qcoresettings_p.h
        aspect/coresettings.cpp aspect/coresettings_p.h
        aspects/aspectcommanddebugger.cpp aspects/aspectcommanddebugger_p.h
        aspects/qabstractaspect.cpp aspects/qabstractaspect.h aspects/qabstractaspect_p.h
        aspects/qaspectengine.cpp aspects/qaspectengine.h aspects/qaspectengine_p.h
        aspects/qaspectfactory.cpp aspects/qaspectfactory_p.h
        aspects/qaspectmanager.cpp aspects/qaspectmanager_p.h
        corelogging.cpp corelogging_p.h
        geometry/bufferutils_p.h
        geometry/buffervisitor_p.h
        geometry/qabstractfunctor.cpp geometry/qabstractfunctor.h
        geometry/qattribute.cpp geometry/qattribute.h geometry/qattribute_p.h
        geometry/qboundingvolume.cpp geometry/qboundingvolume.h geometry/qboundingvolume_p.h
        geometry/qbuffer.cpp geometry/qbuffer.h geometry/qbuffer_p.h
        geometry/qgeometry.cpp geometry/qgeometry.h geometry/qgeometry_p.h
        geometry/qgeometryfactory_p.h
        geometry/qgeometryview.cpp geometry/qgeometryview.h geometry/qgeometryview_p.h
        jobs/calcboundingvolumejob.cpp jobs/calcboundingvolumejob_p.h
        jobs/job_common_p.h
        jobs/qabstractaspectjobmanager.cpp jobs/qabstractaspectjobmanager_p.h
        jobs/qaspectjob.cpp jobs/qaspectjob.h jobs/qaspectjob_p.h
        jobs/qaspectjobmanager.cpp jobs/qaspectjobmanager_p.h
        jobs/qaspectjobproviderinterface_p.h
        jobs/qthreadpooler.cpp jobs/qthreadpooler_p.h
        jobs/task.cpp jobs/task_p.h
        nodes/propertychangehandler.cpp nodes/propertychangehandler_p.h
        nodes/qabstractnodefactory.cpp nodes/qabstractnodefactory_p.h
        nodes/qbackendnode.cpp nodes/qbackendnode.h nodes/qbackendnode_p.h
        nodes/qcomponent.cpp nodes/qcomponent.h nodes/qcomponent_p.h
        nodes/qdestructionidandtypecollector.cpp nodes/qdestructionidandtypecollector_p.h
        nodes/qentity.cpp nodes/qentity.h nodes/qentity_p.h
        nodes/qnode.cpp nodes/qnode.h nodes/qnode_p.h
        nodes/qnodeid.cpp nodes/qnodeid.h
        nodes/qnodevisitor.cpp nodes/qnodevisitor_p.h
        qabstractfrontendnodemanager.cpp qabstractfrontendnodemanager_p.h
        qchangearbiter.cpp qchangearbiter_p.h
        qscene.cpp qscene_p.h
        qscheduler.cpp qscheduler_p.h
        qt3dcore_global.h qt3dcore_global_p.h
        qtickclock.cpp qtickclock_p.h
        qurlhelper.cpp qurlhelper_p.h
        resources/qhandle_p.h
        resources/qloadgltf_p.h
        services/nullservices_p.h
        services/qabstractframeadvanceservice.cpp services/qabstractframeadvanceservice_p.h
        services/qabstractframeadvanceservice_p_p.h
        services/qabstractserviceprovider_p.h
        services/qdownloadhelperservice.cpp services/qdownloadhelperservice_p.h
        services/qdownloadnetworkworker.cpp services/qdownloadnetworkworker_p.h
        services/qeventfilterservice.cpp services/qeventfilterservice_p.h
        services/qopenglinformationservice.cpp services/qopenglinformationservice_p.h
        services/qopenglinformationservice_p_p.h
        services/qservicelocator.cpp services/qservicelocator_p.h
        services/qsysteminformationservice.cpp services/qsysteminformationservice_p.h
        services/qsysteminformationservice_p_p.h
        services/qtickclockservice.cpp services/qtickclockservice_p.h
        transforms/matrix4x4_p.h
        transforms/qabstractskeleton.cpp transforms/qabstractskeleton.h transforms/qabstractskeleton_p.h
        transforms/qarmature.cpp transforms/qarmature.h transforms/qarmature_p.h
        transforms/qjoint.cpp transforms/qjoint.h transforms/qjoint_p.h
        transforms/qmath3d_p.h
        transforms/qskeleton.cpp transforms/qskeleton.h transforms/qskeleton_p.h
        transforms/qskeletonloader.cpp transforms/qskeletonloader.h transforms/qskeletonloader_p.h
        transforms/qtransform.cpp transforms/qtransform.h transforms/qtransform_p.h
        transforms/sqt_p.h
        transforms/vector3d_p.h
        transforms/vector4d_p.h
        vector_helper_p.h
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        aspect
        aspects
        geometry
        jobs
        nodes
        services
        transforms
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
    PUBLIC_LIBRARIES
        Qt::Concurrent # TODO: QTBUG-86533 workaround, Qt::Concurrent should be linked privately.
        Qt::Core
        Qt::Gui
        Qt::Network
    PRIVATE_MODULE_INTERFACE
        Qt::Concurrent
        Qt::CorePrivate
        Qt::GuiPrivate
)

qt_internal_extend_target(3DCore CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)

function(qt3d_add_simd_part target)
    qt_parse_all_arguments(arg "qt3d_add_simd_part" "" ""
       "NAME;SIMD;${__default_private_args};COMPILE_FLAGS" ${ARGN})
    if ("x${arg_SIMD}" STREQUAL x)
        message(FATAL_ERROR "qt3d_add_simd_part needs a SIMD type to be set.")
    endif()

    set(condition "QT_FEATURE_${arg_SIMD}")
    if("${arg_SIMD}" STREQUAL arch_haswell)
        set(condition "TEST_subarch_avx2 AND TEST_subarch_bmi AND TEST_subarch_bmi2 AND TEST_subarch_f16c AND TEST_subarch_fma AND TEST_subarch_lzcnt AND TEST_subarch_popcnt")
    elseif("${arg_SIMD}" STREQUAL avx512common)
        set(condition "TEST_subarch_avx512cd")
    elseif("${arg_SIMD}" STREQUAL avx512core)
        set(condition "TEST_subarch_avx512cd AND TEST_subarch_avx512bw AND TEST_subarch_avx512dq AND TEST_subarch_avx512vl")
    endif()

    qt_evaluate_config_expression(result ${condition})
    if(${result})
        if(QT_CMAKE_DEBUG_EXTEND_TARGET)
            message("qt3d_add_simd_part(${target} SIMD ${arg_SIMD} ...): Evaluated")
        endif()
        string(TOUPPER "QT_CFLAGS_${arg_SIMD}" simd_flags)

        foreach(source IN LISTS arg_SOURCES)
            set_property(SOURCE "${source}" APPEND
                PROPERTY COMPILE_OPTIONS
                ${${simd_flags}}
                ${arg_COMPILE_FLAGS}
            )
        endforeach()
        set_source_files_properties(${arg_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS TRUE)
        target_sources(${target} PRIVATE ${arg_SOURCES})
        target_compile_options("${target}Private" INTERFACE ${${simd_flags}} ${arg_COMPILE_FLAGS})
        target_compile_definitions("${target}Private" INTERFACE ${simd_flags})
    else()
        if(QT_CMAKE_DEBUG_EXTEND_TARGET)
            message("qt3d_add_simd_part(${target} SIMD ${arg_SIMD} ...): Skipped")
        endif()
    endif()
endfunction()

if(QT_FEATURE_qt3d_simd_sse2)
    qt3d_add_simd_part(3DCore SIMD sse2
        SOURCES
            transforms/matrix4x4_sse.cpp transforms/matrix4x4_sse_p.h
    )
endif()

if(QT_FEATURE_qt3d_simd_sse2 AND NOT QT_FEATURE_qt3d_simd_avx2)
    qt3d_add_simd_part(3DCore SIMD sse2
        SOURCES
            transforms/vector3d_sse.cpp transforms/vector3d_sse_p.h
            transforms/vector4d_sse.cpp transforms/vector4d_sse_p.h
            aligned_malloc_p.h
            resources/qresourcemanager.cpp resources/qresourcemanager_p.h
    )
endif()

if(QT_FEATURE_qt3d_simd_avx2)
    qt3d_add_simd_part(3DCore SIMD avx2
        SOURCES
            transforms/matrix4x4_avx2.cpp transforms/matrix4x4_avx2_p.h
            transforms/vector3d_sse.cpp transforms/vector3d_sse_p.h
            transforms/vector4d_sse.cpp transforms/vector4d_sse_p.h
            aligned_malloc_p.h
            resources/qresourcemanager.cpp resources/qresourcemanager_p.h
    )
endif()

qt_internal_extend_target(3DCore
    CONDITION
        NOT QT_FEATURE_qt3d_simd_sse2 AND NOT QT_FEATURE_qt3d_simd_avx2
    SOURCES
        aligned_malloc_p.h
        resources/qresourcemanager.cpp resources/qresourcemanager_p.h
)

qt_internal_add_docs(3DCore
    doc/qt3d.qdocconf
)
