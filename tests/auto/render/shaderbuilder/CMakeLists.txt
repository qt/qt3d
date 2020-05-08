# Generated from shaderbuilder.pro.

#####################################################################
## tst_shaderbuilder Test:
#####################################################################

qt_add_test(tst_shaderbuilder
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_shaderbuilder.cpp
    INCLUDE_DIRECTORIES
        ../../core/common
        ../commons
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

# Resources:
set(shaderbuilder_resource_files
    "input.json"
    "lightmodel.es2.inc"
    "lightmodel.gl3.inc"
    "output.es2"
    "output.gl3"
    "prototypes.json"
)

qt_add_resource(tst_shaderbuilder "shaderbuilder"
    PREFIX
        "/"
    FILES
        ${shaderbuilder_resource_files}
)


#### Keys ignored in scope 1:.:.:shaderbuilder.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_shaderbuilder CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)

qt_extend_target(tst_shaderbuilder CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
