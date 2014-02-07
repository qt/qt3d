TEMPLATE = app
TARGET = tst_transformations

QT += 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/ComposedItemTransformationsTestCase.qml \
    qml/TransformationListTestCase.qml \
    qml/TestTransformations.qml

QML_INFRA_FILES = \
    $$QML_FILES

CATEGORY = manual
include(../../../pkg.pri)

OTHER_FILES += \
    $$QML_INFRA_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
