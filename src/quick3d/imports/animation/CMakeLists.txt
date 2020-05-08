# Generated from importsanimation.pro.

#####################################################################
## quick3danimationplugin Plugin:
#####################################################################

qt_add_qml_module(quick3danimationplugin
    URI "Qt3D.Animation"
    VERSION "2.${CMAKE_PROJECT_VERSION_MINOR}"
    CLASSNAME Qt3DQuick3DAnimationPlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        qt3dquick3danimationplugin.cpp qt3dquick3danimationplugin.h
    PUBLIC_LIBRARIES
        3DAnimation
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
