# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from importslogic.pro.

#####################################################################
## quick3dlogicplugin Plugin:
#####################################################################

qt_internal_add_qml_module(quick3dlogicplugin
    URI "Qt3D.Logic"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASS_NAME Qt3DQuick3DLogicPlugin
    PLUGIN_TARGET quick3dlogicplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    SOURCES
        qt3dquick3dlogicplugin.cpp qt3dquick3dlogicplugin.h
    LIBRARIES
        Qt::3DCore
        3DLogic
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)

#### Keys ignored in scope 1:.:.:importslogic.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Logic"
