# Generated from qshaderprogram.pro.

#####################################################################
## tst_qshaderprogram Test:
#####################################################################

qt_add_test(tst_qshaderprogram
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_qshaderprogram.cpp
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
set(shaders_resource_files
    "included.frag"
    "main.frag"
    "mainabsolute.frag"
)

qt_add_resource(tst_qshaderprogram "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)


#### Keys ignored in scope 1:.:.:qshaderprogram.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_qshaderprogram CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)

qt_extend_target(tst_qshaderprogram CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
