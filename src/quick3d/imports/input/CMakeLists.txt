# Generated from importsinput.pro.

#####################################################################
## quick3dinputplugin Plugin:
#####################################################################

qt_internal_add_qml_module(quick3dinputplugin
    URI "Qt3D.Input"
    VERSION "2.${CMAKE_PROJECT_VERSION_MINOR}"
    CLASSNAME Qt3DQuick3DInputPlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        qt3dquick3dinputplugin.cpp qt3dquick3dinputplugin.h
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DInputPrivate
        Qt::3DQuickInputPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)

#### Keys ignored in scope 1:.:.:importsinput.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Input"

## Scopes:
#####################################################################

qt_internal_extend_target(quick3dinputplugin CONDITION TARGET Qt::Gamepad
    DEFINES
        HAVE_QGAMEPAD
)
