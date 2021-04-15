# Generated from importslogic.pro.

#####################################################################
## quick3dlogicplugin Plugin:
#####################################################################

qt_internal_add_qml_module(quick3dlogicplugin
    URI "Qt3D.Logic"
    VERSION "2.${PROJECT_VERSION_MINOR}"
    CLASSNAME Qt3DQuick3DLogicPlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        qt3dquick3dlogicplugin.cpp qt3dquick3dlogicplugin.h
    PUBLIC_LIBRARIES
        Qt::3DCore
        3DLogic
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)

#### Keys ignored in scope 1:.:.:importslogic.pro:<TRUE>:
# CXX_MODULE = "qml"
# IMPORT_VERSION = "2.$$QT_MINOR_VERSION"
# OTHER_FILES = "qmldir"
# TARGETPATH = "Qt3D/Logic"
