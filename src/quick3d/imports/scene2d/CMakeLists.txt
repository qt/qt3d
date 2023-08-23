# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from importsscene2d.pro.

#####################################################################
## qtquickscene2dplugin Plugin:
#####################################################################

qt_internal_add_qml_module(qtquickscene2dplugin
    URI "QtQuick.Scene2D"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASS_NAME QtQuickScene2DPlugin
    PLUGIN_TARGET qtquickscene2dplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    SOURCES
        qtquickscene2dplugin.cpp qtquickscene2dplugin.h
    LIBRARIES
        Qt::3DCore
        Qt::3DQuickScene2D
        Qt::3DQuickScene2DPrivate
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

#### Keys ignored in scope 1:.:.:importsscene2d.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "QtQuick/Scene2D"
