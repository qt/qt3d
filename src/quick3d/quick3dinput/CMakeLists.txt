# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quick3dinput.pro.

#####################################################################
## Qt::3DQuickInput Module:
#####################################################################

qt_internal_add_module(3DQuickInput
    SOURCES
        items/quick3daction.cpp items/quick3daction_p.h
        items/quick3daxis.cpp items/quick3daxis_p.h
        items/quick3dinputchord.cpp items/quick3dinputchord_p.h
        items/quick3dinputsequence.cpp items/quick3dinputsequence_p.h
        items/quick3dlogicaldevice.cpp items/quick3dlogicaldevice_p.h
        items/quick3dphysicaldevice.cpp items/quick3dphysicaldevice_p.h
        qt3dquickinput_global.cpp qt3dquickinput_global.h qt3dquickinput_global_p.h
        qt3dquickinputnodefactory.cpp qt3dquickinputnodefactory_p.h
    INCLUDE_DIRECTORIES
        items
    LIBRARIES
        Qt::3DCorePrivate
        Qt::3DInputPrivate
        Qt::3DQuickPrivate
        Qt::CorePrivate
        Qt::QmlPrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::Core
        Qt::Gui
        Qt::Qml
    PRIVATE_MODULE_INTERFACE
        Qt::3DCorePrivate
        Qt::3DInputPrivate
        Qt::3DQuickPrivate
        Qt::CorePrivate
        Qt::QmlPrivate
)

#### Keys ignored in scope 1:.:.:quick3dinput.pro:<TRUE>:
# MODULE = "3DQuickInput"

## Scopes:
#####################################################################

qt_internal_extend_target(3DQuickInput CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
