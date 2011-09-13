load(qttest_p4.prf)
TEMPLATE=app
CONFIG += unittest warn_on
QT += declarative opengl
QT += qtquick1
SOURCES += tst_qml3d.cpp
DEFINES += QML3D_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
win32:DEFINES+=_CRT_SECURE_NO_WARNINGS
