# Generated from pickboundingvolumejob.pro.

#####################################################################
## tst_pickboundingvolumejob Test:
#####################################################################

qt_add_test(tst_pickboundingvolumejob
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_pickboundingvolumejob.cpp
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
set(pickboundingvolumejob_resource_files
    "testscene_cameraposition.qml"
    "testscene_childentity.qml"
    "testscene_dragdisabled.qml"
    "testscene_dragenabled.qml"
    "testscene_dragenabledhoverenabled.qml"
    "testscene_dragenabledoverlapping.qml"
    "testscene_improperframegraph.qml"
    "testscene_nopicking.qml"
    "testscene_parententity.qml"
    "testscene_pickersdisabled.qml"
    "testscene_priorityoverlapping.qml"
    "testscene_viewports.qml"
)

qt_add_resource(tst_pickboundingvolumejob "pickboundingvolumejob"
    PREFIX
        "/"
    FILES
        ${pickboundingvolumejob_resource_files}
)


#### Keys ignored in scope 1:.:.:pickboundingvolumejob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_pickboundingvolumejob CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)

qt_extend_target(tst_pickboundingvolumejob CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
