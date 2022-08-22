# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from logic.pro.

#####################################################################
## 3DLogic Module:
#####################################################################

qt_internal_add_module(3DLogic
    SOURCES
        callbackjob.cpp callbackjob_p.h
        executor.cpp executor_p.h
        handle_types_p.h
        handler.cpp handler_p.h
        job_common_p.h
        manager.cpp manager_p.h
        managers_p.h
        qframeaction.cpp qframeaction.h qframeaction_p.h
        qlogicaspect.cpp qlogicaspect.h qlogicaspect_p.h
        qt3dlogic_global.h
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::3DCorePrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::Core
        Qt::Gui
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::3DCorePrivate
)

#### Keys ignored in scope 1:.:.:logic.pro:<TRUE>:
# MODULE = "3DLogic"

## Scopes:
#####################################################################

qt_internal_extend_target(3DLogic CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
