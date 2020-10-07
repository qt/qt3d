# Generated from quick3d.pro.

#####################################################################
## Qt::3DQuick Module:
#####################################################################

qt_add_module(3DQuick
    SOURCES
        items/quick3dbuffer.cpp items/quick3dbuffer_p.h
        items/quick3dentity.cpp items/quick3dentity_p.h
        items/quick3dentityloader.cpp items/quick3dentityloader_p.h
        items/quick3dentityloader_p_p.h
        items/quick3dgeometry.cpp items/quick3dgeometry_p.h
        items/quick3djoint.cpp items/quick3djoint_p.h
        items/quick3dnode.cpp items/quick3dnode_p.h
        items/quick3dnodeinstantiator.cpp items/quick3dnodeinstantiator_p.h
        qqmlaspectengine.cpp qqmlaspectengine.h qqmlaspectengine_p.h
        qquaternionanimation.cpp qquaternionanimation_p.h
        qt3dquick_global.cpp qt3dquick_global.h qt3dquick_global_p.h
        qt3dquicknodefactory.cpp qt3dquicknodefactory_p.h
        qt3dquickvaluetypes.cpp qt3dquickvaluetypes_p.h
    INCLUDE_DIRECTORIES
        items
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickPrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::QmlPrivate
        Qt::QuickPrivate
)

#### Keys ignored in scope 1:.:.:quick3d.pro:<TRUE>:
# MODULE = "3DQuick"

## Scopes:
#####################################################################

qt_extend_target(3DQuick CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)

set_target_properties(3DQuick PROPERTIES
    QT_QML_MODULE_VERSION ${CMAKE_PROJECT_VERSION}
    QT_QML_MODULE_URI Qt3D.Core
    QT_QML_MODULE_INSTALL_QMLTYPES TRUE
    QT_QMLTYPES_FILENAME plugins.qmltypes
    QT_QML_MODULE_INSTALL_DIR "${INSTALL_QMLDIR}/Qt3D/Core"
)

qt6_qml_type_registration(3DQuick)
