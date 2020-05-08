# Generated from updateshaderdatatransformjob.pro.

#####################################################################
## tst_updateshaderdatatransformjob Test:
#####################################################################

qt_add_test(tst_updateshaderdatatransformjob
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_updateshaderdatatransformjob.cpp
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
set(updateshaderdatatransformjob_resource_files
    "test_scene_model_to_eye.qml"
    "test_scene_model_to_world.qml"
)

qt_add_resource(tst_updateshaderdatatransformjob "updateshaderdatatransformjob"
    PREFIX
        "/"
    FILES
        ${updateshaderdatatransformjob_resource_files}
)


#### Keys ignored in scope 1:.:.:updateshaderdatatransformjob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_updateshaderdatatransformjob CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)

qt_extend_target(tst_updateshaderdatatransformjob CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
