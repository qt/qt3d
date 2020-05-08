# Generated from setfence.pro.

#####################################################################
## tst_setfence Test:
#####################################################################

qt_add_test(tst_setfence
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_setfence.cpp
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

#### Keys ignored in scope 1:.:.:setfence.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_setfence CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)

qt_extend_target(tst_setfence CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
