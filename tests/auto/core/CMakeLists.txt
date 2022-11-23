# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from core.pro.

add_subdirectory(handle)
if(NOT (UNIX AND NOT APPLE AND NOT WIN32 AND CMAKE_CROSSCOMPILING))
    # that test is flacky on QEMU (crashes on app exit QTBUG-107693),
    # so disabling when cross-compiling on linux
    add_subdirectory(qresourcemanager)
endif()
add_subdirectory(nodes)
add_subdirectory(qaspectengine)
add_subdirectory(qaspectfactory)
add_subdirectory(qaspectjob)
add_subdirectory(qchangearbiter)
add_subdirectory(qscene)
add_subdirectory(qservicelocator)
add_subdirectory(qjoint)
add_subdirectory(qskeletonloader)
add_subdirectory(qskeleton)
add_subdirectory(qarmature)
if(QT_FEATURE_private_tests)
    add_subdirectory(qentity)
    add_subdirectory(qtransform)
    add_subdirectory(threadpooler)
    add_subdirectory(vector4d_base)
    add_subdirectory(vector3d_base)
    add_subdirectory(aspectcommanddebugger)
    add_subdirectory(qscheduler)
endif()
if(QT_FEATURE_private_tests AND NOT (CMAKE_OSX_ARCHITECTURES MATCHES ";") AND
        (TEST_architecture_arch STREQUAL i386 OR TEST_architecture_arch STREQUAL x86_64))
    add_subdirectory(vector4d_sse)
    add_subdirectory(vector3d_sse)
    add_subdirectory(matrix4x4_sse)
endif()
