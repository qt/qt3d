TEMPLATE = app
TARGET = tst_translation

QT += 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/AxesVsVectorTestCase.qml \
    qml/TranslationTestCase.qml \
    qml/TranslationVectorTestCase.qml \
    qml/TestTranslation.qml

QML_INFRA_FILES = \
    $$QML_FILES

CATEGORY = manual
include(../../../pkg.pri)

OTHER_FILES += \
    $$QML_INFRA_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
