# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from importsinput.pro.

#####################################################################
## quick3dinputplugin Plugin:
#####################################################################

qt_internal_add_qml_module(quick3dinputplugin
    URI "Qt3D.Input"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASS_NAME Qt3DQuick3DInputPlugin
    DEPENDENCIES
        QtQml/auto
    PLUGIN_TARGET quick3dinputplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    SOURCES
        qt3dquick3dinputplugin.cpp qt3dquick3dinputplugin.h
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DInputPrivate
        Qt::3DQuickInputPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)

#### Keys ignored in scope 1:.:.:importsinput.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Input"

## Scopes:
#####################################################################

qt_internal_extend_target(quick3dinputplugin CONDITION TARGET Qt::Gamepad
    DEFINES
        HAVE_QGAMEPAD
)
