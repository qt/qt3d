load(qttest_p4.prf)
TEMPLATE=app
CONFIG += unittest warn_on
QT += declarative opengl
SOURCES += tst_qml3d.cpp
DEFINES += QML3D_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
win32:DEFINES+=_CRT_SECURE_NO_WARNINGS

OTHER_FILES += \
    lookat/tst_lookat.qml \
    QtQuickTest/TestCase.qml \
    scale3d/tst_scale3d.qml \
    modelview/tst_modelview.qml \
    sphere/tst_sphere.qml \
    effect/tst_effect.qml \
    viewport/tst_viewport.qml \
    mesh/tst_mesh.qml \
    picking/tst_picking.qml \
    item3d/tst_item3d.qml \
    item3d/tst_missing_texture_coordinates.qml \
    item3d/tst_item3d_local_v_world.qml


