# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quick3dextras.pro.

#####################################################################
## Qt::3DQuickExtras Module:
#####################################################################

qt_internal_add_qml_module(3DQuickExtras
    URI "Qt3D.Extras"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 2
    DEPENDENCIES
        QtQuick/auto
        Qt3D.Logic/auto
        Qt3D.Render/auto
    PLUGIN_TARGET quick3dextrasplugin
    NO_GENERATE_PLUGIN_SOURCE

    CLASS_NAME Qt3DQuick3DExtrasPlugin
    SOURCES
        items/quick3dlevelofdetailloader.cpp items/quick3dlevelofdetailloader_p.h
        items/quick3dlevelofdetailloader_p_p.h
        items/quick3dspritesheet.cpp items/quick3dspritesheet_p.h
        qt3dquickextras_global.cpp qt3dquickextras_global.h qt3dquickextras_global_p.h
        qt3dquickextrasnodefactory.cpp qt3dquickextrasnodefactory_p.h
        qt3dquickwindow.cpp qt3dquickwindow.h qt3dquickwindow_p.h
        qt3dquickwindowlogging.cpp qt3dquickwindowlogging_p.h
        qt3dquick3dextrasforeign_p.h
    INCLUDE_DIRECTORIES
        items
    LIBRARIES
        Qt::3DExtrasPrivate
        Qt::3DQuickPrivate
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::QmlPrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DLogic
        Qt::3DQuick
        Qt::3DRender
        Qt::Core
        Qt::Gui
        Qt::Qml
    PRIVATE_MODULE_INTERFACE
        Qt::3DExtrasPrivate
        Qt::3DQuickPrivate
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::QmlPrivate
)

#### Keys ignored in scope 1:.:.:quick3dextras.pro:<TRUE>:
# MODULE = "3DQuickExtras"

## Scopes:
#####################################################################

qt_internal_extend_target(3DQuickExtras CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)

qt_internal_extend_target(quick3dextrasplugin
    SOURCES
        qt3dquick3dextrasplugin.cpp qt3dquick3dextrasplugin_p.h
    LIBRARIES
        Qt::3DQuickExtrasPrivate
)
