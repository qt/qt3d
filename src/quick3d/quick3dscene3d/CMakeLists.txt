# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_qml_module(3DQuickScene3D
    URI "QtQuick.Scene3D"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 2
    PLUGIN_TARGET qtquickscene3dplugin
    SOURCES
        scene3ditem.cpp scene3ditem_p.h
        scene3dlogging.cpp scene3dlogging_p.h
        scene3drenderer.cpp scene3drenderer_p.h
        scene3dsgmaterial.cpp scene3dsgmaterial_p.h
        scene3dsgmaterialshader.cpp scene3dsgmaterialshader_p.h
        scene3dsgnode.cpp scene3dsgnode_p.h
    DEPENDENCIES
        Qt3D.Core/auto
        QtQuick/auto
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::Core
        Qt::Quick
    LIBRARIES
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
        Qt::Qml
        Qt::CorePrivate
        Qt::QuickPrivate
)

qt_internal_extend_target(3DQuickScene3D CONDITION QT_FEATURE_qt3d_input
    LIBRARIES
        Qt::3DInput
)

qt_internal_extend_target(3DQuickScene3D CONDITION QT_FEATURE_qt3d_logic
    LIBRARIES
        Qt::3DLogic
)

qt_internal_extend_target(3DQuickScene3D CONDITION QT_FEATURE_qt3d_animation
    LIBRARIES
        Qt::3DAnimation
)

# Resources:
set(shader_resource_files
    "shaders/scene3dmaterial.vert.qsb"
    "shaders/scene3dmaterial.frag.qsb"
)
qt_internal_add_resource(3DQuickScene3D "scene3d"
    PREFIX
        "/"
    FILES
        ${shader_resource_files}
)
