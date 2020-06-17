# Generated from importsscene2d.pro.

#####################################################################
## qtquickscene2dplugin Plugin:
#####################################################################

qt_add_qml_module(qtquickscene2dplugin
    URI "QtQuick.Scene2D"
    VERSION "2.${CMAKE_PROJECT_VERSION_MINOR}"
    CLASSNAME QtQuickScene2DPlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        qtquickscene2dplugin.cpp qtquickscene2dplugin.h
    PUBLIC_LIBRARIES
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
