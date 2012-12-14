TEMPLATE=app
TARGET=tst_qml3d
CONFIG += qmltestcase
QT += qml opengl
SOURCES += tst_qml3d.cpp

win32:DEFINES+=_CRT_SECURE_NO_WARNINGS

OTHER_FILES += \
    QtQuickTest/TestCase.qml \
    lookat/tst_lookat.qml \
    scale3d/tst_scale3d.qml \
    modelview/tst_modelview.qml \
    sphere/tst_sphere.qml \
    viewport/tst_viewport.qml \
    mesh/tst_mesh.qml \
    item3d/tst_item3d.qml \
    item3d/tst_missing_texture_coordinates.qml \
    item3d/tst_item3d_local_v_world.qml
TESTDATA = $$OTHER_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
