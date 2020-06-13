# Generated from importsscene3d.pro.

#####################################################################
## qtquickscene3dplugin Plugin:
#####################################################################

qt_add_qml_module(qtquickscene3dplugin
    URI "QtQuick.Scene3D"
    VERSION "2.${CMAKE_PROJECT_VERSION_MINOR}"
    CLASSNAME QtQuickScene3DPlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        qtquickscene3dplugin.cpp qtquickscene3dplugin.h
        scene3ditem.cpp scene3ditem_p.h
        scene3dlogging.cpp scene3dlogging_p.h
        scene3drenderer.cpp scene3drenderer_p.h
        scene3dsgmaterial.cpp scene3dsgmaterial_p.h
        scene3dsgmaterialshader.cpp scene3dsgmaterialshader_p.h
        scene3dsgnode.cpp scene3dsgnode_p.h
        scene3dview.cpp scene3dview_p.h
    PUBLIC_LIBRARIES
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

qt_extend_target(qtquickscene3dplugin CONDITION QT_FEATURE_qt3d_input
    PUBLIC_LIBRARIES
        Qt::3DInput
)

qt_extend_target(qtquickscene3dplugin CONDITION QT_FEATURE_qt3d_logic
    PUBLIC_LIBRARIES
        3DLogic
)

qt_extend_target(qtquickscene3dplugin CONDITION QT_FEATURE_qt3d_animation
    PUBLIC_LIBRARIES
        Qt::3DAnimation
)

# Resources:
set(shader_resource_files
    "shaders/scene3dmaterial.vert.qsb"
    "shaders/scene3dmaterial.frag.qsb"
)

qt_add_resource(qtquickscene3dplugin "scene3d"
    PREFIX
        "/"
    FILES
        ${shader_resource_files}
)
