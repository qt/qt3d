# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from importsrender.pro.

#####################################################################
## quick3drenderplugin:
#####################################################################

qt_internal_add_qml_module(quick3drenderplugin
    URI "Qt3D.Render"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASS_NAME Qt3DQuick3DRenderPlugin
    DEPENDENCIES
        QtQml/auto
    PLUGIN_TARGET quick3drenderplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    SOURCES
        qt3dquick3drenderplugin.cpp qt3dquick3drenderplugin.h
    LIBRARIES
        Qt::3DCore
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::3DQuickRenderPrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::QmlPrivate
)

#### Keys ignored in scope 1:.:.:importsrender.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Render"
