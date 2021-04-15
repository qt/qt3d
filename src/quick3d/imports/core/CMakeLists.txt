# Generated from importscore.pro.

#####################################################################
## quick3dcoreplugin Plugin:
#####################################################################

qt_internal_add_qml_module(quick3dcoreplugin
    URI "Qt3D.Core"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASSNAME Qt3DQuick3DCorePlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        qt3dquick3dcoreplugin.cpp qt3dquick3dcoreplugin.h
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::QmlPrivate
        Qt::Quick
        Qt::QuickPrivate
)

#### Keys ignored in scope 1:.:.:importscore.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Core"
