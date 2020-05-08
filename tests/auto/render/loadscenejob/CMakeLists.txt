# Generated from loadscenejob.pro.

#####################################################################
## tst_loadscenejob Test:
#####################################################################

qt_add_test(tst_loadscenejob
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_loadscenejob.cpp
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

#### Keys ignored in scope 1:.:.:loadscenejob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_loadscenejob CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)

qt_extend_target(tst_loadscenejob CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
