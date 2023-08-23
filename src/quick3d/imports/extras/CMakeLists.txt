# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from importsextras.pro.

#####################################################################
## quick3dextrasplugin Plugin:
#####################################################################

qt_internal_add_qml_module(quick3dextrasplugin
    URI "Qt3D.Extras"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASS_NAME Qt3DQuick3DExtrasPlugin
    PLUGIN_TARGET quick3dextrasplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    DEPENDENCIES
        QtQuick/auto
        Qt3D.Logic/auto
    SOURCES
        qt3dquick3dextrasplugin.cpp qt3dquick3dextrasplugin.h
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        3DLogic
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::3DQuickExtras
        Qt::3DQuickExtrasPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::QmlPrivate
        Qt::Quick
        Qt::QuickPrivate
)

#### Keys ignored in scope 1:.:.:importsextras.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Extras"
