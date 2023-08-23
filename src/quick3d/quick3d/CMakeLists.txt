# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quick3d.pro.

#####################################################################
## Qt::3DQuick Module:
#####################################################################

qt_internal_add_qml_module(3DQuick
    URI "Qt3D.Core"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 2
    CLASS_NAME Qt3DQuick3DCorePlugin
    DEPENDENCIES
        QtQuick/auto
    PLUGIN_TARGET quick3dcoreplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES
        items/quick3dbuffer.cpp items/quick3dbuffer_p.h
        items/quick3dentity.cpp items/quick3dentity_p.h
        items/quick3dentityloader.cpp items/quick3dentityloader_p.h
        items/quick3dentityloader_p_p.h
        items/quick3dgeometry.cpp items/quick3dgeometry_p.h
        items/quick3djoint.cpp items/quick3djoint_p.h
        items/quick3dnode.cpp items/quick3dnode_p.h
        items/quick3dnodeinstantiator.cpp items/quick3dnodeinstantiator_p.h
        qqmlaspectengine.cpp qqmlaspectengine.h qqmlaspectengine_p.h
        qquaternionanimation.cpp qquaternionanimation_p.h
        qt3dquick_global.cpp qt3dquick_global.h qt3dquick_global_p.h
        qt3dquickforeign_p.h
        qt3dquicknodefactory.cpp qt3dquicknodefactory_p.h
        qt3dquickvaluetypes.cpp qt3dquickvaluetypes_p.h
    INCLUDE_DIRECTORIES
        items
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickPrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickPrivate
)

#### Keys ignored in scope 1:.:.:quick3d.pro:<TRUE>:
# MODULE = "3DQuick"

## Scopes:
#####################################################################

qt_internal_extend_target(3DQuick CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)

# We can't move these sources into here because the directory structure needs
# to be maintained to support Qt5 qmake builds.
qt_internal_extend_target(quick3dcoreplugin
    SOURCES
        ../imports/core/qt3dquick3dcoreplugin.cpp
        ../imports/core/qt3dquick3dcoreplugin.h
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::QmlPrivate
        Qt::Quick
        Qt::QuickPrivate
)
