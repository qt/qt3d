# Generated from importsrender.pro.

#####################################################################
## quick3drenderplugin:
#####################################################################

qt_add_qml_module(quick3drenderplugin
    URI "Qt3D.Render"
    VERSION "2.${CMAKE_PROJECT_VERSION_MINOR}"
    CLASSNAME Qt3DQuick3DRenderPlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        qt3dquick3drenderplugin.cpp qt3dquick3drenderplugin.h
    PUBLIC_LIBRARIES
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
