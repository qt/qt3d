# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from importsscene3d.pro.

#####################################################################
## qtquickscene3dplugin Plugin:
#####################################################################

qt_internal_add_qml_module(qtquickscene3dplugin
    URI "QtQuick.Scene3D"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASS_NAME QtQuickScene3DPlugin
    PLUGIN_TARGET qtquickscene3dplugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    SOURCES
        qtquickscene3dplugin.cpp qtquickscene3dplugin.h
        scene3ditem.cpp scene3ditem_p.h
        scene3dlogging.cpp scene3dlogging_p.h
        scene3drenderer.cpp scene3drenderer_p.h
        scene3dsgmaterial.cpp scene3dsgmaterial_p.h
        scene3dsgmaterialshader.cpp scene3dsgmaterialshader_p.h
        scene3dsgnode.cpp scene3dsgnode_p.h
    LIBRARIES
        Qt::3DCore
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::QuickPrivate
)

#### Keys ignored in scope 1:.:.:importsscene3d.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "QtQuick/Scene3D"

## Scopes:
#####################################################################

qt_internal_extend_target(qtquickscene3dplugin CONDITION QT_FEATURE_qt3d_input
    LIBRARIES
        Qt::3DInput
)

qt_internal_extend_target(qtquickscene3dplugin CONDITION QT_FEATURE_qt3d_logic
    LIBRARIES
        Qt::3DLogic
)

qt_internal_extend_target(qtquickscene3dplugin CONDITION QT_FEATURE_qt3d_animation
    LIBRARIES
        Qt::3DAnimation
)

# Resources:
set(shader_resource_files
    "shaders/scene3dmaterial.vert.qsb"
    "shaders/scene3dmaterial.frag.qsb"
)

qt_internal_add_resource(qtquickscene3dplugin "scene3d"
    PREFIX
        "/"
    FILES
        ${shader_resource_files}
)
