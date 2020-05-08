# Generated from raycastingjob.pro.

#####################################################################
## tst_raycastingjob Test:
#####################################################################

qt_add_test(tst_raycastingjob
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_raycastingjob.cpp
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
set(raycastingjob_resource_files
    "testscene_screenraycasting.qml"
    "testscene_worldraycasting.qml"
    "testscene_worldraycastingalllayers.qml"
    "testscene_worldraycastinglayer.qml"
    "testscene_worldraycastingparentlayer.qml"
)

qt_add_resource(tst_raycastingjob "raycastingjob"
    PREFIX
        "/"
    FILES
        ${raycastingjob_resource_files}
)


#### Keys ignored in scope 1:.:.:raycastingjob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_raycastingjob CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)

qt_extend_target(tst_raycastingjob CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
