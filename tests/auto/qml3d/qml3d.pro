load(qmltestcase.prf)
TEMPLATE=app
TARGET=tst_qml3d
CONFIG += warn_on
SOURCES += tst_qml3d.cpp

DEFINES -= QUICK_TEST_SOURCE_DIR
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
win32:DEFINES+=_CRT_SECURE_NO_WARNINGS

OTHER_FILES += \
    lookat/tst_lookat.qml \
    scale3d/tst_scale3d.qml \
    modelview/tst_modelview.qml \
    sphere/tst_sphere.qml \
    effect/tst_effect.qml \
    viewport/tst_viewport.qml \
    mesh/tst_mesh.qml \
    picking/tst_picking.qml \
    item3d/tst_item3d.qml \
    item3d/tst_missing_texture_coordinates.qml \
    item3d/tst_item3d_local_v_world.qml \

# Don't block while tests are unstable
CONFIG+=insignificant_test
