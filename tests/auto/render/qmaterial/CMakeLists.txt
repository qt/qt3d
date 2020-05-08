# Generated from qmaterial.pro.

#####################################################################
## tst_qmaterial Test:
#####################################################################

qt_add_test(tst_qmaterial
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_qmaterial.cpp
    INCLUDE_DIRECTORIES
        ../../core/common
        ../commons
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qmaterial.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_qmaterial CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)

qt_extend_target(tst_qmaterial CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
