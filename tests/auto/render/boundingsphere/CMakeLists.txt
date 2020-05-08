# Generated from boundingsphere.pro.

#####################################################################
## tst_boundingsphere Test:
#####################################################################

qt_add_test(tst_boundingsphere
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_boundingsphere.cpp
    INCLUDE_DIRECTORIES
        ../../core/common
        ../commons
        ../qmlscenereader
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(boundingsphere_resource_files
    "cube.qml"
    "sphere.qml"
)

qt_add_resource(tst_boundingsphere "boundingsphere"
    PREFIX
        "/"
    FILES
        ${boundingsphere_resource_files}
)


#### Keys ignored in scope 1:.:.:boundingsphere.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_boundingsphere CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)

qt_extend_target(tst_boundingsphere CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
