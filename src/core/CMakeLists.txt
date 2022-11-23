# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from core.pro.

#####################################################################
## Qt::3DCore Module:
#####################################################################

qt_internal_add_module(3DCore
    SOURCES
        aligned_malloc_p.h
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
        resources/qresourcemanager.cpp resources/qresourcemanager_p.h
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

qt_internal_extend_target(3DCore CONDITION
    CONDITION ( TEST_architecture_arch STREQUAL i386 ) OR
        ( TEST_architecture_arch STREQUAL x86_64 ) OR
        ( CMAKE_OSX_ARCHITECTURES MATCHES "x86_64" ) OR
        ( CMAKE_OSX_ARCHITECTURES MATCHES "x86_64h" ) OR
        ( CMAKE_OSX_ARCHITECTURES MATCHES "i386" )
    SOURCES
        transforms/matrix4x4_avx2_p.h
        transforms/matrix4x4_sse.cpp transforms/matrix4x4_sse_p.h
        transforms/vector3d_sse.cpp transforms/vector3d_sse_p.h
        transforms/vector4d_sse.cpp transforms/vector4d_sse_p.h
)

qt_internal_add_docs(3DCore
    doc/qt3d.qdocconf
)
