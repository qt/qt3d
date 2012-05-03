TEMPLATE=app
TARGET=tst_qml3d_visual
CONFIG += warn_on qmltestcase
QT += qml opengl
SOURCES += tst_qml3d_visual.cpp

win32:DEFINES+=_CRT_SECURE_NO_WARNINGS

OTHER_FILES += \
    modelview/tst_modelview.qml \
    effect/tst_effect.qml \
    picking/tst_picking.qml

TESTDATA = $$OTHER_FILES
