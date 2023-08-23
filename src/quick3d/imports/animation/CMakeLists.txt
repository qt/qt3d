# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from importsanimation.pro.

#####################################################################
## quick3danimationplugin Plugin:
#####################################################################

qt_internal_add_qml_module(quick3danimationplugin
    URI "Qt3D.Animation"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASS_NAME Qt3DQuick3DAnimationPlugin
    DEPENDENCIES
        QtQml/auto
        Qt3D.Render/auto
    PLUGIN_TARGET quick3danimationplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    SOURCES
        qt3dquick3danimationplugin.cpp qt3dquick3danimationplugin.h
    LIBRARIES
        Qt::3DAnimation
        Qt::3DCore
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::3DQuickAnimation
        Qt::3DQuickAnimationPrivate
        Qt::3DQuickRender
        Qt::3DRender
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::QmlPrivate
)

#### Keys ignored in scope 1:.:.:importsanimation.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Animation"
