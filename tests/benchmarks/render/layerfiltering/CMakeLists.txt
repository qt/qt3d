# Generated from layerfiltering.pro.

#####################################################################
## tst_bench_layerfiltering Test:
#####################################################################

qt_add_test(tst_bench_layerfiltering
    SOURCES
        ../../../auto/render/commons/testrenderer.cpp ../../../auto/render/commons/testrenderer.h
        tst_bench_layerfiltering.cpp
    DEFINES
        QT_BUILD_INTERNAL
    INCLUDE_DIRECTORIES
        ../../../auto/render/commons
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:layerfiltering.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_bench_layerfiltering CONDITION useCommonTestAspect
    SOURCES
        ../../../auto/render/commons/testaspect.cpp ../../../auto/render/commons/testaspect.h
)
