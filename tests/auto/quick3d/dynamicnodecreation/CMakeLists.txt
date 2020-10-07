# Generated from dynamicnodecreation.pro.

#####################################################################
## tst_dynamicnodecreation Test:
#####################################################################

# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    data/*)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_dynamicnodecreation
    SOURCES
        ../../shared/util.cpp ../../shared/util.h
        tst_dynamicnodecreation.cpp
    DEFINES
        QT_QMLTEST_DATADIR=\\\"${CMAKE_CURRENT_SOURCE_DIR}/data\\\"
    INCLUDE_DIRECTORIES
        ../../shared
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::GuiPrivate
    TESTDATA ${test_data}
)

#### Keys ignored in scope 1:.:.:dynamicnodecreation.pro:<TRUE>:
# OTHER_FILES = "data/createDynamicChild.qml" "data/createMultiple.qml" "data/createSingle.qml" "data/inactive.qml" "data/dynamicEntity.qml"

## Scopes:
#####################################################################
